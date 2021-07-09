#include <Arduino.h>
#include <Adafruit_GFX.h>

const uint16_t cntMaxQuotes = 365;

/*
static Quote q[cntMaxQuotes] = {
    {"Ich bin nicht gescheitert.\nIch habe nur 10.000\nWege entdeckt, die\nnicht funktioniert haben.", "Thomas A. Edison"},
    {"Das wahre Geheimnis\ndes Erfolgs ist\ndie Begeisterung.", "Walter P. Chrysler"},
    {"Phantasie ist wichtiger\nals Wissen, denn\nWissen ist begrenzt.", "Albert Einstein"},
    {"Was ich will,\ndas nehm ich mir", ""},
    {"Mir tun alle leid,\ndie nicht in\nBullerbü leben.", "Inga, Immer lustig in Bullerbü"},
    {"Dumm ist der,\nder Dummes tut.", "Forrest Gump"}
};
*/

// the lines fo text array contains one quote broken down into pieces at every '\n' linebreak character.
// this means one line of the quote is in one array field. if the quote does not have 10 lines, the rest of the array
// is empty. if you need more lines than 10 then just raise the array.
// fontOfLines holds the font for every line of text
String linesOfText[10];

typedef struct {
    String txt;
    String author;
    byte count;   // currently unused since we removed real random quote selection, see commented part in Quotes::GetRandomQuote()
} quote;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS DECLARATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class Quotes 
{
    public:
        static quote q[cntMaxQuotes];

        static uint16_t GetNumberOfQuotes();
        static uint16_t GetNextFreeQuoteID();

        static void DeleteQuote(uint16_t quoteID);
        static void DeleteAllQuotes();
        static void AddQuotesToJsonDocument(DynamicJsonDocument &doc);
        static void SaveQuotesToSPIFFS();
        static void LoadQuotesFromSPIFFS();
        static String ExportQuotesToJSONString();
        static void ImportQuotesFromJSONString(const String& jsonString, bool replace);
        static byte GetLowestCountValue();
        static quote GetRandomQuote();
        static void ResetQuoteCounter();
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////


quote Quotes::q[cntMaxQuotes];


/**************************************************
* GetNumberOfQuotes()
* Retuns the number of quotes
*/
uint16_t Quotes::GetNumberOfQuotes()
{
    uint16_t result = 0;

    for (size_t i = 0; i < cntMaxQuotes; i++)
    {
        if ( q[i].txt != "" )
        {
            result++;
        }
    }

    return result;
}


/**************************************************
* GetNextFreeQuoteID()
* Get the next free quote id
*/
uint16_t Quotes::GetNextFreeQuoteID()
{
    uint16_t result = 0;

    for (size_t i = 0; i < cntMaxQuotes; i++)
    {
        if ( q[i].txt == "" )
        {
            return result;
        }else{
            result++;
        }
    }

    if ( result > cntMaxQuotes)
    {
        result = cntMaxQuotes;
    }

    return result;
}


/**************************************************
* DeleteQuote(uint16_t quoteID)
* Delete the quote with the id
* To avoid empty quotes, all follow up quotes are move up one id
*/
void Quotes::DeleteQuote(uint16_t quoteID)
{
    for (size_t i = quoteID+1; i < cntMaxQuotes-1; i++)
    {
        q[i-1].txt = q[i].txt;
        q[i-1].author = q[i].author;
        q[i-1].count = q[i].count;
    }

    // clear the very last quote
    q[cntMaxQuotes-1].txt = "";
    q[cntMaxQuotes-1].author = "";
    q[cntMaxQuotes-1].count = 0;

    Quotes::SaveQuotesToSPIFFS();
}


/**************************************************
* DeleteAllQuotes()
* Delete all quotes
*/
void Quotes::DeleteAllQuotes()
{
    Serial.println(F("DeleteAllQuotes()"));

    for (size_t i = 0; i < cntMaxQuotes; i++)
    {
        q[i].txt = "";
        q[i].author = "";
        q[i].count = 0;
    }
}


/**************************************************
* LoadQuotesFromSPIFFS()
* Loads all the quotes from a /quotes.json file to the QUOTES array
*/
void Quotes::LoadQuotesFromSPIFFS()
{
    Serial.println(F("Quotes::LoadQuotesFromSPIFFS()"));

    // create JsonDocument
    DynamicJsonDocument doc(ESP.getMaxAllocHeap());

   // check if file exist in SPIFFS
    bool doesFileExist = SPIFFS.exists("/quotes.json");
    if (doesFileExist)
    {
        // open quotes file
        File fileQuotes = SPIFFS.open("/quotes.json", "r");

        if (!fileQuotes) 
        {
            Serial.println(F("Failed to open file for reading"));
            return;
        }

        DeserializationError error = deserializeJson(doc, fileQuotes);
        if (error)
            return;

        //JsonObject object = doc.to<JsonObject>();
        
        // print whole json 
        /*
        String output;
        serializeJsonPretty(doc, output);
        Serial.print(output);
        Serial.println(F(""));
        */

        // close the file
        fileQuotes.close();

        // get the jsonArray that contains all the quotes
        JsonArray arr = doc["quotes"].as<JsonArray>();

        // iterate the array and add each quote the the QUOTES global 
        int cnt = 0;
        for (JsonObject value : arr) 
        {
            if (cnt < cntMaxQuotes)
            {
                const char* txt = value["txt"];
                const char* author = value["author"];
                const byte count = value["count"]; 
                q[cnt].txt = txt;
                q[cnt].author = author;
                q[cnt].count = count;
                cnt++;
            }
        }

        Serial.print("#Quotes loaded from SPIFFS: ");
        Serial.println(arr.size());
    }else{
        Serial.println(F("file quotes.json does NOT exist!"));
    }
}


/**************************************************
* AddQuotesToJsonDocument(DynamicJsonDocument &doc)
* Used by all function where a JsonDocument should be created from the quotes
* This way we have only one place, where the JSON structure is created
*/
void Quotes::AddQuotesToJsonDocument(DynamicJsonDocument &doc)
{
    JsonObject object = doc.to<JsonObject>();
    JsonArray quoteArray = object.createNestedArray("quotes");
    
    for (size_t i = 0; i < cntMaxQuotes; i++)
    {
        if (q[i].txt != "")
        {
            JsonObject newQuote = quoteArray.createNestedObject();
            newQuote["txt"] = q[i].txt;
            newQuote["author"] = q[i].author;
            newQuote["count"] = q[i].count;
        }
    }
}


/**************************************************
* SaveQuotesToSPIFFS()
* The current QUOTES gets converted to a JSON string which gets written 
* to SPIFFS
*/
void Quotes::SaveQuotesToSPIFFS()
{
    // convert QUOTES to JSON and
    // write JSON to SPIFFS file stream

    Serial.println(F("SaveQuotesToSPIFFS()"));

    DynamicJsonDocument doc(ESP.getMaxAllocHeap());

    AddQuotesToJsonDocument(doc);

    // serialize the object and send the result to Serial
    serializeJsonPretty(doc, Serial);
    Serial.println(F(""));

    File fileQuotes = SPIFFS.open("/quotes.json", "w");

    // Serialize JSON to file
    if (serializeJson(doc, fileQuotes) == 0) {
        Serial.println(F("Failed to write to file"));
    }
      
    // Close the file
    fileQuotes.close();

    Serial.println(F("Quotes saved to SPIFFS"));
}


/**************************************************
* ExportQuotesToJSONString()
* The current QUOTES gets converted to a JSON string 
* Returns: a json String
*/
String Quotes::ExportQuotesToJSONString()
{
    // convert QUOTES to JSON string

    Serial.println(F("ExportQuotesToJSON()"));

    // create json doc and add the quotes to it
    DynamicJsonDocument doc(ESP.getMaxAllocHeap());
    AddQuotesToJsonDocument(doc);

    // write json to a string adn return it
    String output;
    serializeJsonPretty(doc, output);
    return output;
}


/**************************************************
* ImportQuotesFromJSONString(String &json)
* Import quotes via json string
* if replace is true, then all quotes are replaced
* otherwise the import is appended
*/
void Quotes::ImportQuotesFromJSONString(const String& jsonString, bool replace)
{
    // convert QUOTES to JSON string

    Serial.println(F("ImportQuotesFromJSONString()"));

    DynamicJsonDocument doc(ESP.getMaxAllocHeap());
    deserializeJson(doc, jsonString);

    // get the jsonArray that contains all the quotes
    JsonArray arr = doc["quotes"].as<JsonArray>();

    // if quotes should be replaced
    if (replace == true)
    {
        Serial.println(F("REPLACE"));
        DeleteAllQuotes();

    // if quotes should be appended
    }else{
        Serial.println(F("APPEND"));
    }

    int minQuoteID = GetNextFreeQuoteID();
    int cnt = 0;

    for (JsonObject value : arr) 
    {
        int index = minQuoteID + cnt;
        Serial.print("#");
        Serial.println(index);

        if (index < cntMaxQuotes)
        {
            // only add if txt is not empty
            if ( !value["txt"].isNull() )
            {
                String txt = value["txt"];
                String author = value["author"];
                byte count = value["count"];

                if (txt != "")
                {
                    q[index].txt = txt;
                    q[index].author = author;
                    q[index].count = count;
                    cnt++;
                }else{
                    Serial.println(F("Quote txt is empty  - SKIP"));
                }
            }else{
                Serial.println(F("Field txt is not found  - SKIP"));
            }
        }else{
            Serial.println(F("Maximum number of quotes is reached - ABORT"));
            break;
        }
    }

    Serial.print("Imported quotes: ");
    Serial.println(cnt);
}


/**************************************************
* GetLowestCountValue()
* iteraet all quotes and get the lowest count value
*/
byte Quotes::GetLowestCountValue()
{
    byte result = 255;

    for (size_t i = 0; i < cntMaxQuotes; i++)
    {
        if (q[i].txt != "")
        {
            if ( q[i].count < result )
                result = q[i].count;
            
            if (result == 0)
                return 0;
        }
    }

    return result;
}

/**************************************************
* GetRandomQuote()
*/
quote Quotes::GetRandomQuote()
{
    Serial.println("Quotes::GetRandomQuote()");
    quote result;
    uint16_t randomID = 0;
    uint16_t numberOfQuotes = Quotes::GetNumberOfQuotes();
    uint16_t maxID = numberOfQuotes;

    Serial.print("numberOfQuotes: ");
    Serial.println(String(numberOfQuotes));

    // NO QUOTES
    // if no quotes
    if ( numberOfQuotes == 0 )
    {
        result.txt = "No quotes found.\nRestart in config mode\nand add some quotes!";
    }

    // 1 QUOTES
    // if we only have one quote, display it
    else if ( numberOfQuotes == 1 )
    {
        randomID = 0;
    }

    // 2 QUOTES
    // if we have two quotes, just display the on that is NOT the LASTQUOTEID
    else if ( numberOfQuotes == 2 )
    {
        if ( LASTQUOTEID == 0 )
        {
            randomID = 1;
        }else{
            randomID = 0;
        }
    }

    /* REAL RANOM ATTEMPT

    // 3+ QUOTES
    // if we have more than three quotes then chose a quote which has the lowest count value ans is NOT the LASTQUOTEID
    else if ( numberOfQuotes >= 3 )
    {
        // get the lowest count value
        byte lowestCountValue = Quotes::GetLowestCountValue();
        uint attempts = 0;

        do
        {
            attempts++;

            // 2. create a random index value
            randomID = random(0, maxID);

            // do not display the same quotes two times
            if ( randomID != LASTQUOTEID)
            {
                // 3. if quote with random index count value is not = lowest value then repeat from step 2.
                if ( Quotes::q[randomID].count == lowestCountValue )
                {
                    Serial.println("random quote found");
                    LASTQUOTEID = randomID;
                    break;
                }
            }

            if ( attempts > 6500000)
            {
                Quotes::ResetQuoteCounter();
                LASTQUOTEID = randomID;
                break;
            }
        } while ( true );
        

        // if count is 255, then reset all count values to zero!!
        // casue the maximum value for count is 255
        if ( Quotes::q[randomID].count >= 255 ) { Quotes::ResetQuoteCounter(); }

        // raise counter of the choosen quopte
        Quotes::q[randomID].count = Quotes::q[randomID].count + 1;

        // save quotes due to new counter value
        Quotes::SaveQuotesToSPIFFS();

        Serial.print("attempts: ");
        Serial.println(String(attempts))
    }
    */

    // 3+ QUOTES
    // if we have more than three quotes then chose the next quote after LASTQUOTEID
    else if ( numberOfQuotes >= 3 )
    {
        randomID = LASTQUOTEID + 1;
        if ( randomID >= maxID )
            randomID = 0;
    }

    // update LASTQUOTEID
    LASTQUOTEID = randomID;
    Settings::last_quote_id = LASTQUOTEID;
    
    // save settings due to change of LASTQUOTEID
    Settings::SaveToSPIFFS();

    result = Quotes::q[randomID];

    Serial.print("last quote id: ");
    Serial.println(String(LASTQUOTEID));
    Serial.print("random quote: ");
    Serial.println(result.txt);
    Serial.print("count: ");
    Serial.println(result.count);
    
    return result;
}


/**************************************************
* Quotes::ResetQuoteCounter()
* sets all count of each quote to 0
*/
void Quotes::ResetQuoteCounter()
{
    for (size_t i = 0; i < cntMaxQuotes; i++)
    {
        q[i].count = 0;
    }
}

