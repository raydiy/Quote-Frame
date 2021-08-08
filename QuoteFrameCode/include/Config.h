// Here you can configure some default values

#define MODE_FRAME 0                        // mode 0 is normal quotes mode
#define MODE_CONFIG 1                       // mode 1 is config mode

#define TOUCH_THRESHOLD 40                  // Define touch sensitivity. Greater the value, more the sensitivity. 20
#define TOUCH_PIN 32                        // the pin of the touch sensor, default like in YouTube Video is 32. eThing Pulse uses 33.
#define SCREEN_ROTATION 0                   // if for some reasons you want to rotate the display, use 0 or 2


const char *AP_SSID     = "QuoteFrame";     // the SSID of the gateway
const char *AP_PASSWORD = "12345678";       // tha password of the gateway
uint16_t QUOTEID        = 0;                // a global qupote id use by the setings to form for deleting and editing quotes
uint16_t LASTQUOTEID    = 0;                // the last quote id that has been displayed
bool EXPORT_TO_JSON     = false;            // needed for the JSON export in config mode
uint TIME_TO_DEEPSLEEP  = 120;              // in seconds, how long after a new quote gets loaded. 60*60*24 would be once per day. 60 would be every minute
uint8_t MODE            = MODE_FRAME;       // MODE does contain the current active mode, per default the fraem starts in MODE_FRAME
uint8_t LANGUAGE        = 0;                // Current Langauge ID. 0: EN, 1: DE

IPAddress local_IP(192,168,4,1);            // the default IP adress & subnet mask you find the gateway at 
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS DECLARATION
//////////////////////////////////////////////////////////////////////////////////////////////////////////


class Settings 
{
    public:
        static uint16_t interval_seconds;
        static uint16_t interval_minutes;
        static uint16_t interval_hours;
        static uint16_t interval_days;
        static byte inactivity_restart;
        static uint16_t last_quote_id;
        static uint8_t language_id;

        static void SaveToSPIFFS();
        static void LoadFromSPIFFS();
        static uint GetIntervallInSeconds();
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLASS DEFINITIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// default values

uint16_t Settings::interval_seconds = 0;
uint16_t Settings::interval_minutes = 2;
uint16_t Settings::interval_hours = 0;
uint16_t Settings::interval_days = 0;
byte Settings::inactivity_restart = 5;
uint16_t Settings::last_quote_id = 0;
uint8_t Settings::language_id = Loca::LANG_EN;


/**************************************************
* SaveToSPIFFS()
* The current Settings gets converted to a JSON string which gets written 
* to SPIFFS
*/
void Settings::SaveToSPIFFS()
{
    // write Settings to SPIFFS file stream

    Serial.println(F("Settings::SaveToSPIFFS()"));

    StaticJsonDocument<128> doc;

    doc["interval_seconds"]     = Settings::interval_seconds;
    doc["interval_minutes"]     = Settings::interval_minutes;
    doc["interval_hours"]       = Settings::interval_hours;
    doc["interval_days"]        = Settings::interval_days;
    doc["inactivity_restart"]   = Settings::inactivity_restart;
    doc["last_quote_id"]        = Settings::last_quote_id;
    doc["language_id"]          = Settings::language_id;

    // serialize the object and send the result to Serial
    serializeJsonPretty(doc, Serial);
    Serial.println(F(""));

    File fileSettings = SPIFFS.open("/settings.json", "w");

    // Serialize JSON to file
    if (serializeJson(doc, fileSettings) == 0) {
        Serial.println(F("Failed to write to file"));
    }
      
    // Close the file
    fileSettings.close();

    Serial.println(F("Settings saved to SPIFFS"));
}


/**************************************************
* LoadFromSPIFFS()
* Loads all the settings from a /settings.json file
*/
void Settings::LoadFromSPIFFS()
{
    Serial.println(F("Settings::LoadFromSPIFFS()"));

    // create JsonDocument
    StaticJsonDocument<256> doc;

    // check if file exist in SPIFFS
    Serial.println(F("check if file exist"));
    bool doesFileExist = SPIFFS.exists("/settings.json");

    if (doesFileExist)
    {
        Serial.println(F("file does exist"));

        // open settings file
        File fileSettings = SPIFFS.open("/settings.json", "r");

        if (!fileSettings) 
        {
            Serial.println(F("Failed to open file for reading"));
            return;
        }

        DeserializationError error = deserializeJson(doc, fileSettings);
        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return;
        }

        // close the file
        fileSettings.close();


        // get the json s´data that contains all the quotes
        Settings::interval_seconds      = doc["interval_seconds"].as<uint16_t>();
        Settings::interval_minutes      = doc["interval_minutes"].as<uint16_t>();
        Settings::interval_hours        = doc["interval_hours"].as<uint16_t>();
        Settings::interval_days         = doc["interval_days"].as<uint16_t>();
        Settings::inactivity_restart    = doc["inactivity_restart"].as<byte>();
        Settings::last_quote_id         = doc["last_quote_id"].as<uint16_t>();
        Settings::language_id           = doc["language_id"].as<uint8_t>();

        Serial.println( F("#Settings loaded from SPIFFS: ") );
        Serial.print( "interval_seconds: " );
        Serial.print( String(Settings::interval_seconds) );
        Serial.println("");
        Serial.print( "interval_minutes: " );
        Serial.print( String(Settings::interval_minutes) );
        Serial.println("");
        Serial.print( "interval_hours: " );
        Serial.print( String(Settings::interval_hours) );
        Serial.println("");
        Serial.print( "interval_days: " );
        Serial.print( String(Settings::interval_days) );
        Serial.println("");
        Serial.print( "inactivity_restart: " );
        Serial.print( String(Settings::inactivity_restart) );
        Serial.println("");
        Serial.print( "last_quote_id: " );
        Serial.print( String(Settings::last_quote_id) );
        Serial.println("");
        Serial.print( "language_id: " );
        Serial.print( String(Settings::language_id) );
        Serial.println("");

        TIME_TO_DEEPSLEEP = Settings::GetIntervallInSeconds();
        LASTQUOTEID = Settings::last_quote_id;
        LANGUAGE = Settings::language_id;
        
        // set loca language
        Loca::setLanguage(LANGUAGE);

    }else{
        Serial.println(F("file settings.json does NOT exist. Using default values."));
    }
}


/**************************************************
* GetIntervallInSeconds()
* Returns the current intervall settings to seconds
*/
uint Settings::GetIntervallInSeconds()
{
    Serial.println(F("Settings::GetIntervallInSeconds()"));

    if ( Settings::interval_seconds > 0 ) 
        return Settings::interval_seconds;
    
    if ( Settings::interval_minutes > 0 ) 
        return Settings::interval_minutes * 60;

    if ( Settings::interval_hours > 0 ) 
        return Settings::interval_hours * 60 * 60;

    if ( Settings::interval_days > 0 ) 
        return Settings::interval_days * 24 * 60 * 60;

    return 0;
}