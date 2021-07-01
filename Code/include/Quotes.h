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
        static quote GetRandomQuote();
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
    }

    // clear the very last quote
    q[cntMaxQuotes-1].txt = "";
    q[cntMaxQuotes-1].author = "";

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
    }
}


/**************************************************
* LoadQuotesFromSPIFFS()
* Loads all the quotes from a /quotes.json file to the QUOTES array
*/
void Quotes::LoadQuotesFromSPIFFS()
{
    Serial.println(F("LoadQuotesFromSPIFFS()"));

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
                q[cnt].txt = txt;
                q[cnt].author = author;
                cnt++;
            }
        }

        Serial.print("#Quotes loaded from SPIFFS: ");
        Serial.println(arr.size());
    }else{
        Serial.println(F("file does NOT exist!"));
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

                if (txt != "")
                {
                    q[index].txt = txt;
                    q[index].author = author;
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
* GetRandomQuote()
*/
quote Quotes::GetRandomQuote()
{
    Serial.println("Quotes::GetRandomQuote()");
    quote result;

    uint16_t maxID = Quotes::GetNumberOfQuotes();
    uint16_t randomID = random(0, maxID);

    //Serial.print("maxID: ");
    //Serial.println(maxID);

    if ( maxID > 0 )
    {
        result = Quotes::q[randomID];
    }else{
        result.txt = "No quotes found.\nRestart in config mode\nand add some quotes!";
    }

    //Serial.print("random quote: ");
    Serial.println(result.txt);

    return result;
}