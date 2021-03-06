#include <Arduino.h>

// Import required libraries
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "Localization.h"
#include "Config.h"
#include "Quotes.h"
#include "Display.h"


// Create DNSServer and AsyncWebServer object on port 80
DNSServer dnsServer;
AsyncWebServer webServer(80);



///////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS
///////////////////////////////////////////////////////////////

void ConnectToWiFi(const char *STAssid, const char *STApassword);
void HandleRequestDeleteQuote(AsyncWebServerRequest *request);
void HandleRequestEditQuote(AsyncWebServerRequest *request);
void HandleRequestSaveQuote(AsyncWebServerRequest *request);
void HandleRequestReboot(AsyncWebServerRequest *request);
void HandleRequestSettings(AsyncWebServerRequest *request);
void HandleRequestImExport(AsyncWebServerRequest *request);
void DeepSleep_Begin();
void Touch_Callback();
void DeepSleep_PrintWakeupReason();
void DeepSleep_PrintWakeupTouchpad();
void DebugPrintRequestParameter(AsyncWebServerRequest *request);
bool InitWiFiAccessPoint();
void TouchEndConfigMode_Callback();
void FillTouchValuesArray(int value);
int MadianFromArray(int* array, int size);
void PrintArray(int* arr, int size);


// Replaces placeholder with button section in your web page
String processor(const String& var)
{
    //Serial.println("processor()");

    if(var == "HTMLHEAD")
    {
        String q = "";
        q += "<title>" + String(Loca::S(STR_QUOTEFRAME)) + "</title>";
        q += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
        q += "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">";
        q += "<link rel=\"icon\" href=\"data:,\">";
        q += "<link rel=\"stylesheet\" type=\"text/css\" href=\"mini-default.min.css\">";
        return q;
    }
    else if(var == "MENU")
    {
        String q = "";
        q += "<header class=\"sticky\">";
        q += "<a class=\"logo\" href=\"/\">" + String(Loca::S(STR_QUOTEFRAME)) + "</a>";
        q += "<a class=\"button col-sm col-md\" href=\"/new\">&#10133;</a>";
        q += "<a class=\"button col-sm col-md\" href=\"/reboot\">&#x21bb</a>";
        q += "<a class=\"button col-sm col-md\" href=\"/imexport\"><span class=\"icon-upload\"></span></a>";
        q += "<a class=\"button col-sm col-md\" href=\"/settings\">&#9881</a>";

        q += "</header>";
        return q;
    }
    else if(var == "QUOTESTABLE")
    {
        int cntQuotes = Quotes::GetNumberOfQuotes();
        String q = "";

        if ( cntQuotes > 0)
        {
            for (size_t i = 0; i < cntQuotes; i++) 
            { 
                q += "<div class=\"section\">";
                    q += "<div class=\"col-sm-12\"><h3>#" + String(i+1) + "</h3></div>";
                    q += "<div class=\"col-sm-12\"><blockquote cite=\"" + Quotes::q[i].author + "\">" + Quotes::q[i].txt + "</blockquote></div>";
                    q += "<div class=\"col-sm-12\" style=\"text-align: right;\">";
                        q += "<a href=\"/edit?id=" + String(i) + "\" class=\"button small\"><span class=\"icon-edit\"></span></a>";
                        q += "<a href=\"/?delete&id=" + String(i) + "\" class=\"button small\">&#10060;</a>";
                    q += "</div>"; 
                q += "</div>";
            }
        }
        else
        {
            q += "<h3>" + String(Loca::S(STR_NO_QUOTES_FOUND_1)) + "</h3>";
            q += "<p>" + String(Loca::S(STR_ADD_SOME_QUOTES)) + "</p>";
            q += "<a href=\"/new\" class=\"button primary col-sm\" style=\"text-align: center;\">" + String(Loca::S(STR_ADD_FIRST_QUOTE)) + "</a>";
        }
        return q;
    }
    else if(var == "EXPORTJSON")
    {
        if (EXPORT_TO_JSON)
        {
            EXPORT_TO_JSON = false;
            return Quotes::ExportQuotesToJSONString();
        }else{
            return "";
        }    
    }
    else if(var == "QUOTE")
    {
        return Quotes::q[QUOTEID].txt;
    }
    else if(var == "AUTHOR")
    {
        return Quotes::q[QUOTEID].author;
    }
    else if(var == "ID")
    {
        return String(QUOTEID);
    }

    // SETTINGS PAGE
    else if(var == "SETTINGS_INTERVAL_VALUE")
    {
        if ( Settings::interval_seconds > 0 ) { return String(Settings::interval_seconds); }
        if ( Settings::interval_minutes > 0 ) { return String(Settings::interval_minutes); }
        if ( Settings::interval_hours > 0 ) { return String(Settings::interval_hours); }
        if ( Settings::interval_days > 0 ) { return String(Settings::interval_days); }
    }
    else if(var == "SETTINGS_INTERVAL_SECONDS")
    {
        if ( Settings::interval_seconds > 0 ) { return String("selected"); }
        return String("");
    }
    else if(var == "SETTINGS_INTERVAL_MINUTES")
    {
        if ( Settings::interval_minutes > 0 ) { return String("selected"); }
        return String("");   
    }
    else if(var == "SETTINGS_INTERVAL_HOURS")
    {
        if ( Settings::interval_hours > 0 ) { return String("selected"); }
        return String("");
    }
    else if(var == "SETTINGS_INTERVAL_DAYS")
    {
        if ( Settings::interval_days > 0 ) { return String("selected"); }
        return String("");
    }
    else if(var == "SETTINGS_LANGUAGE_EN")
    {
        if ( Settings::language_id == Loca::LANG_EN ) { return String("checked"); }
        return String("");
    }
    else if(var == "SETTINGS_LANGUAGE_DE")
    {
        if ( Settings::language_id == Loca::LANG_DE ) { return String("checked"); }
        return String("");
    }

    // here we exchange the local strings in the HTML pages

    // index
    else if(var == "STR_QUOTES") { return Loca::S(STR_QUOTES); }

    // settings
    else if(var == "STR_SETTINGS") { return Loca::S(STR_SETTINGS); }
    else if(var == "STR_SELECT_LANG") { return Loca::S(STR_SELECT_LANG); }
    else if(var == "STR_QUOTE_INTERVAL") { return Loca::S(STR_QUOTE_INTERVAL); }
    else if(var == "STR_SECONDS") { return Loca::S(STR_SECONDS); }
    else if(var == "STR_MINUTES") { return Loca::S(STR_MINUTES); }
    else if(var == "STR_HOURS") { return Loca::S(STR_HOURS); }
    else if(var == "STR_DAYS") { return Loca::S(STR_DAYS); }
    else if(var == "STR_SAVE") { return Loca::S(STR_SAVE); }

    // new & edit
    else if(var == "STR_ADD_NEW_QUOTE") { return Loca::S(STR_ADD_NEW_QUOTE); }
    else if(var == "STR_EDIT_QUOTE") { return Loca::S(STR_EDIT_QUOTE); }
    else if(var == "STR_QUOTE") { return Loca::S(STR_QUOTE); }
    else if(var == "STR_NEW_QUOTE_HERE") { return Loca::S(STR_NEW_QUOTE_HERE); }
    else if(var == "STR_NEW_AUTHOR_HERE") { return Loca::S(STR_NEW_AUTHOR_HERE); }

    // import/export
    else if(var == "STR_IMPORT_QUOTES") { return Loca::S(STR_IMPORT_QUOTES); }
    else if(var == "STR_JSON_CODE_HERE") { return Loca::S(STR_JSON_CODE_HERE); }
    else if(var == "STR_REPLACE_ALL_QUOTES") { return Loca::S(STR_REPLACE_ALL_QUOTES); }
    else if(var == "STR_EXPORT_QUOTES") { return Loca::S(STR_EXPORT_QUOTES); }

    // save
    else if(var == "STR_QUOTE_SAVED") { return Loca::S(STR_QUOTE_SAVED); }

    return String();
}


class CaptiveRequestHandler : public AsyncWebHandler 
{
    public:
    CaptiveRequestHandler() 
    {
        // Route for root / web page
        webServer.on("/", HTTP_ANY, [](AsyncWebServerRequest *request){
            HandleRequestDeleteQuote(request);
            request->send(SPIFFS, "/index.html", String(), false, processor);
        });

        // Route for /edit web page
        webServer.on("/edit", HTTP_ANY, [](AsyncWebServerRequest *request){
            HandleRequestEditQuote(request);
            request->send(SPIFFS, "/edit.html", String(), false, processor);
        });

        // Route for /new web page
        webServer.on("/new", HTTP_ANY, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/new.html", String(), false, processor);
        });

        // Route for /save web page
        webServer.on("/save", HTTP_ANY, [](AsyncWebServerRequest *request){
            HandleRequestSaveQuote(request);
            request->send(SPIFFS, "/save.html", String(), false, processor);
        });

        // Route for /imexport web page
        webServer.on("/imexport", HTTP_ANY, [](AsyncWebServerRequest *request){
            HandleRequestImExport(request);
            request->send(SPIFFS, "/imexport.html", String(), false, processor);
        });

        // Route for /reboot web page
        webServer.on("/reboot", HTTP_ANY, [](AsyncWebServerRequest *request){
            HandleRequestReboot(request);
            request->send(SPIFFS, "/index.html", String(), false, processor);
        });

        // Route for /settings web page
        webServer.on("/settings", HTTP_ANY, [](AsyncWebServerRequest *request){
            HandleRequestSettings(request);
            request->send(SPIFFS, "/settings.html", String(), false, processor);
        });

        // Route to load mini-default.min.css file
        webServer.on("/mini-default.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
            request->send(SPIFFS, "/mini-default.min.css", "text/css");
        });
    }
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request)
    {
        //request->addInterestingHeader("ANY");
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request)
    {
       request->send(SPIFFS, "/hello.html", String(), false);
    }
};


#define TOUCH_ARRAY_SIZE 100
int touch_value_counter = 0;
int touch_values[TOUCH_ARRAY_SIZE];

/**************************************************
* setup()
*/
void setup() {
    // Serial port for debugging purposes
    Serial.begin(115200);
    while(!Serial);

    // Initialize SPIFFS
    Serial.println("main::setup() Initialize SPIFFS ...");
    if(!SPIFFS.begin(true)){
        Serial.println("main::setup() An Error has occurred while mounting SPIFFS");
        return;
    }

    WiFi.disconnect(true);             // that no old information is stored  
    WiFi.mode(WIFI_OFF);               // switch WiFi off  
    delay(1000);                       // short wait to ensure WIFI_OFF  
    WiFi.persistent(false);            // avoid that WiFi-parameters will be stored in persistent memory  


    // Set Canvas size
    Display::SetCanvasSize(58,35,680,450,60);


    // print wake up reasons to console
    DeepSleep_PrintWakeupReason();
    DeepSleep_PrintWakeupTouchpad();

    // Load Settings from SPIFFS
    Settings::LoadFromSPIFFS();

    // Load quotes from SPIFFS
    Quotes::LoadQuotesFromSPIFFS();
    
    delay(100);
    Display::Init();

    // SETTINGS MODE
    if ( MODE == MODE_CONFIG )
    {

        Display::ShowMessage( Loca::S(STR_START_CONFIG_MODE) );
        
        // START ACCESS POINT MODE //////////////////////////////////////////////////
        // Init WiFi Access Point
        bool successWiFi = InitWiFiAccessPoint();

        if (successWiFi)
        {
            Serial.println("main::setup() WiFi started successfully ...");
            Serial.println("main::setup() Start DNS Server ...");

            // Start DNS server for captive portal. Route all requests to the ESP IP address.
            dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
            dnsServer.start(53, "*", WiFi.softAPIP());

            Serial.println("main::setup() Add handler ...");
            webServer.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP

            // Start the web server
            delay(1000);
            Serial.println("main::setup() Webserver started.");
            webServer.begin();
        }
        // END ACCESS POINT MODE //////////////////////////////////////////////////
    }

    // Draw Quotes or Config Infos to the display
    Display::Draw();

    // in quote frame mode we start deep sleep
    if ( MODE == MODE_FRAME )
    {
        DeepSleep_Begin();
    }
}


/**************************************************
* loop()
*/
void loop() 
{
    if ( MODE == MODE_CONFIG )
    {
        dnsServer.processNextRequest();

        // get current touch value from touch sensor wire 
        int touch_value = touchRead(TOUCH_PIN); 

        // fill the touch_values array
        FillTouchValuesArray(touch_value);

        // if array is filled create an average median from all values
        if (touch_value_counter == TOUCH_ARRAY_SIZE)
        {
            // create median
            int median_touch_value = MadianFromArray(touch_values, TOUCH_ARRAY_SIZE);
            

            // check for touch pin to restart frame

            //Serial.print("touch_value/median_touch_value/THRESHOLD: ");
            //Serial.print(touch_value);
            //Serial.print("/");
            //Serial.print(median_touch_value);
            //Serial.print("/");
            //Serial.println(TOUCH_THRESHOLD);

            if ( median_touch_value < TOUCH_THRESHOLD )
            {
                Serial.println("TOUCH DETECTED");
                //Serial.print("last_touch_pin_value: ");
                //Serial.println(last_touch_pin_value);
                //PrintArray(touch_values, TOUCH_ARRAY_SIZE);
                //Serial.print("median_touch_value: ");
                //Serial.println(median_touch_value);

                ESP.restart();
            }
        }
    }
}



///////////////////////////////////////////////////////////////
// FUNCTION DEFINITIONS
///////////////////////////////////////////////////////////////


/**************************************************
* FillTouchValuesArray(int value)
* Fills the touch_values array with new values from the touch sensor
* This array is needed to create a moving avaerage to remove spikes from touch sensor
* to prevent fals positiveve touches
*/
void FillTouchValuesArray(int value)
{
    if (touch_value_counter < TOUCH_ARRAY_SIZE)
    {
        touch_values[touch_value_counter] = value;
        touch_value_counter++;
    }
    else
    {
        // copy array to the left without the very first one that gest overwritten
        for(size_t i=0;i<TOUCH_ARRAY_SIZE-1;i++)
        {
            touch_values[i] = touch_values[i+1]; //move all elements to the left except first one
        }

        // insert new value to the last array element
        touch_values[TOUCH_ARRAY_SIZE-1] = value;
    }
}


/**************************************************
* MadianFromArray(int* arr, int size)
* Creates a median average from an int array
*/
int MadianFromArray(int* arr, int size)
{
    // create median
    int median = 0;
    for (size_t i = 0; i < size; i++)
    {
        median += arr[i];
    }
    median = median/size;
    
    return median;
}


/**************************************************
* PrintArray(int* arr, int size)
* Prints an integer Array to console
*/
void PrintArray(int* arr, int size)
{
    Serial.println( "PrintArray() ---------------" );

    for(size_t i=0;i<size;i++)
    {
        Serial.print(i);
        Serial.print(": ");
        Serial.println( arr[i] );
    }
}


/**************************************************
* HandleRequestImExport(AsyncWebServerRequest *request)
*/
void HandleRequestImExport(AsyncWebServerRequest *request)
{
    // check if a export param is sent via GET
    // if yes, we set the EXPORT_TO_JSON bool to true
    // EXPORT_TO_JSON bool is used in processor() function to export the quotes 
    if (request->hasParam("export")) 
    {
        Serial.println("HandleRequestImExport() EXPORT");
        EXPORT_TO_JSON = true;
    }

    // check if a import param is sent via POST
    // if yes, import param contains the json code for import
    // get the json code add the quotes to QUOTES
    else if (request->hasParam("import", true)) 
    {
        Serial.println("HandleRequestImExport() IMPORT");
        
        DebugPrintRequestParameter(request);

        String importJSON = request->getParam("import", true)->value();
        //Serial.println(importJSON);

        if (request->hasParam("replace", true))
        {
            Quotes::ImportQuotesFromJSONString(importJSON, true);
        }else{
            Quotes::ImportQuotesFromJSONString(importJSON, false);
        }

        Quotes::ResetQuoteCounter();
        Quotes::SaveQuotesToSPIFFS();

        // immediate redirect to home 
        request->redirect("/");
    }
}


/**************************************************
* HandleRequestDeleteQuote(AsyncWebServerRequest *request)
*/
void HandleRequestDeleteQuote(AsyncWebServerRequest *request)
{
    // check if a delete param is sent via GET
    // if yes, check for a quote id parameter and delete that wuote
    if (request->hasParam("delete")) 
    {
        Serial.println("main::HandleRequestDeleteQuote()");

        uint16_t quoteIDtoDelete = request->getParam("id")->value().toInt();
        Quotes::DeleteQuote(quoteIDtoDelete);

        Serial.println("DELETE QUOTE: ");
        Serial.println(quoteIDtoDelete);

        // immediate redirect to home 
        request->redirect("/");
    }
}


/**************************************************
* HandleRequestReboot(AsyncWebServerRequest *request)
*/
void HandleRequestReboot(AsyncWebServerRequest *request)
{
    Serial.println("HandleRequestReboot()");

    // immediate redirect to home 
    request->redirect("/");
    delay(1000);
    ESP.restart();
}

/**************************************************
* HandleRequestSettings(AsyncWebServerRequest *request)
*/
void HandleRequestSettings(AsyncWebServerRequest *request)
{
    Serial.println("main::HandleRequestSettings()");

    if (request->hasParam("save", true)) 
    {
        Serial.println("Save settings ...");

        // reset all intervall values to 0
        Settings::interval_seconds = 0;
        Settings::interval_minutes = 0;
        Settings::interval_hours = 0;
        Settings::interval_days = 0;
    
        // get the intervall value from the form
        int value = request->getParam("interval_value", true)->value().toInt();
    
        if ( request->getParam("interval_unit", true)->value().toInt() == 0 ){ Settings::interval_seconds = value; }
        else if ( request->getParam("interval_unit", true)->value().toInt() == 1 ){ Settings::interval_minutes = value; }
        else if ( request->getParam("interval_unit", true)->value().toInt() == 2 ){ Settings::interval_hours = value; }
        else if ( request->getParam("interval_unit", true)->value().toInt() == 3 ){ Settings::interval_days = value; }

        // gte the language ID
        Settings::language_id = request->getParam("language_value", true)->value().toInt();
        Loca::setLanguage( Settings::language_id );

        // Save to SPIFFS
        Settings::SaveToSPIFFS();

        // immediate redirect to home 
        request->redirect("/");
    }else{
        Serial.println("no save");
    }
}

/**************************************************
* HandleRequestEditQuote(AsyncWebServerRequest *request)
*/
void HandleRequestEditQuote(AsyncWebServerRequest *request)
{
    Serial.println("main::HandleRequestEditQuote()");

    if (request->hasParam("id")) 
    {
        if ( request->getParam("id")->value() == "new" )
        {
            Serial.println("ADD NEW QUOTE");
        }
        else
        {
            QUOTEID = request->getParam("id")->value().toInt();
            Serial.println("EDIT QUOTE");
            Serial.print("QUOTEID set to: ");
            Serial.println(QUOTEID);
        }
    }else{
        Serial.println("No quote id found.");
    }
}


/**************************************************
* HandleRequestSaveQuote(AsyncWebServerRequest *request)
*/
void HandleRequestSaveQuote(AsyncWebServerRequest *request)
{
    Serial.println("main::HandleRequestSaveQuote()");

    String quoteText;
    String quoteAuthor;
    bool isNew = false;

    // if a quote text is sent
    if (request->hasParam("quote", true)) 
    {
        // if a new parameter is sent we add a new quote
        if (request->hasParam("new", true)) 
        {
            QUOTEID = Quotes::GetNextFreeQuoteID();
            isNew = true;
        
            Serial.print("New QUOTEID is: ");
            Serial.println(QUOTEID);
        }

        // if no new parameter is sent we edit an existing quote
        else 
        {
            QUOTEID = request->getParam("id", true)->value().toInt();
            Serial.print("QUOTEID is: ");
            Serial.println(QUOTEID);
        }

        // get the quote text
        quoteText = request->getParam("quote", true)->value();

        // if quote text is not empty
        if ( !quoteText.isEmpty() )
        {
            // check if an author text is sent
            if (request->hasParam("author", true)) 
            {
                quoteAuthor = request->getParam("author", true)->value();
            }

            Quotes::q[QUOTEID].txt = quoteText;
            Quotes::q[QUOTEID].author = quoteAuthor;

            if ( isNew ){ Quotes::ResetQuoteCounter(); }
            Quotes::SaveQuotesToSPIFFS();

            // immediatley redirect
            // if you comment the following line, the user
            // gets a Saved Quote page displayed, where he adas to click OK
            // before the returning to home
            request->redirect("/");
        }
        else 
        {
            Serial.println("Quote text is empty - ABORT");
            request->redirect("/");
        }
    }
}

/**************************************************
* ConnectToWiFi(const char *ssid, const char *password)
*/
void ConnectToWiFi(const char *ssid, const char *password)
{
    Serial.println("main::ConnectToWiFi()");

    // prepare WiFi
    WiFi.disconnect(true);             // that no old information is stored  
    WiFi.mode(WIFI_OFF);               // switch WiFi off  
    delay(1000);                       // short wait to ensure WIFI_OFF  
    WiFi.persistent(false);            // avoid that WiFi-parameters will be stored in persistent memory  

    // Connect to Wi-Fi
    WiFi.mode(WIFI_STA);
    delay(1000);  
    WiFi.begin(ssid, password);

    // try 15 times, then do a reconnect if still not conected
    int i = 0;
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) 
    {
        Serial.print(".");

        if ( i == 15 )
        {
            Serial.print("_reconnect_");
            WiFi.reconnect();
        }

        if (i > 30)
            break;
        
        delay(500);
        i++;
    }

    // Print ESP Local IP Address
    Serial.println(WiFi.localIP());
}


/**************************************************
* DeepSleep_Begin() 
*/
void DeepSleep_Begin()
{
    Serial.println("main::DeepSleep_Begin()");

    //display.powerOff();
    //display.hibernate(); // only for reset pin wakeup

    #ifdef BUILTIN_LED
    pinMode(BUILTIN_LED, INPUT); // If it's On, turn it off and some boards use GPIO-5 for SPI-SS, which remains low after screen use
    digitalWrite(BUILTIN_LED, HIGH);
    #endif
    Serial.println("Starting deep-sleep period...");
    Serial.print("Next wakup in ");
    Serial.print(String(TIME_TO_DEEPSLEEP));
    Serial.println(" secs");

    //Setup interrupt on Touch Pad 8 (GPIO33)
    touchAttachInterrupt(TOUCH_PIN, Touch_Callback, TOUCH_THRESHOLD);

    //Configure Touchpad as wakeup source
    esp_sleep_enable_touchpad_wakeup();

    // start deep sleep timer
    uint64_t microToSecondsFactor = 1000000;
    esp_sleep_enable_timer_wakeup(TIME_TO_DEEPSLEEP * microToSecondsFactor); // in microseconds
    //esp_sleep_enable_timer_wakeup(20 * microToSecondsFactor); // in microseconds
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON); // all RTC Peripherals are powered

    // sleep!
    esp_deep_sleep_start();
}


/**************************************************
* Touch_Callback() 
*/
void Touch_Callback()
{
    Serial.println("main::Touch_Callback()");
}



/**************************************************
* DeepSleep_PrintWakeupReason() 
* Function that prints the reason by which ESP32 has been awaken from sleep
*/
void DeepSleep_PrintWakeupReason()
{
    Serial.println("main::DeepSleep_PrintWakeupReason()");

    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch(wakeup_reason)
    {
        case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
        case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
        case 3  : Serial.println("Wakeup caused by ???"); break;
        case 4  : Serial.println("Wakeup caused by timer"); break;
        case 5  : Serial.println("Wakeup caused by ULP program"); break;
        default : Serial.println("Wakeup was not caused by deep sleep"); break;
    }
}


/**************************************************
* DeepSleep_PrintWakeupReason() 
* Function that prints the touchpad by which ESP32 has been awaken from sleep
*/
void DeepSleep_PrintWakeupTouchpad()
{
    Serial.println("main::DeepSleep_PrintWakeupTouchpad()");

    touch_pad_t touchPin = esp_sleep_get_touchpad_wakeup_status();
    switch(touchPin)
    {
        case 0  : Serial.println("Touch detected on GPIO 4"); break;
        case 1  : Serial.println("Touch detected on GPIO 0"); break;
        case 2  : Serial.println("Touch detected on GPIO 2"); break;
        case 3  : Serial.println("Touch detected on GPIO 15"); break;
        case 4  : Serial.println("Touch detected on GPIO 13"); break;
        case 5  : Serial.println("Touch detected on GPIO 12"); break;
        case 6  : Serial.println("Touch detected on GPIO 14"); break;
        case 7  : Serial.println("Touch detected on GPIO 27"); break;
        case 8  : Serial.println("Touch detected on GPIO 33"); break;
        case 9  : Serial.println("Touch detected on GPIO 32"); break;
        default : Serial.println("Wakeup not by touchpad"); break;
    }

    if (touchPin == 9 || touchPin == 8)
    {
        if (MODE == MODE_CONFIG)
        {
            Serial.println("RESTART FRAME ...");
            MODE = MODE_FRAME;
            ESP.restart();
        }
        else if (MODE == MODE_FRAME)
        {
            Serial.println("START IN CONFIG MODE NOW ...");
            MODE = MODE_CONFIG;
            esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
        }
    }
}



/**************************************************
* DebugPrintRequestParameter() 
*/
void DebugPrintRequestParameter(AsyncWebServerRequest *request)
{
    Serial.println("main::DebugPrintRequestParameter()");

    //List all parameters
    int params = request->params();
    for(int i=0;i<params;i++)
    {
        AsyncWebParameter* p = request->getParam(i);
        if(p->isFile()){ //p->isPost() is also true
            Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if(p->isPost()){
            Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
            Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
    }
}

/**************************************************
* InitWiFiAccessPoint() 
*/
bool InitWiFiAccessPoint()
{
    //Serial.println("main::InitWiFiAccessPoint()");

    WiFi.disconnect(true);             // that no old information is stored  
    WiFi.mode(WIFI_OFF);               // switch WiFi off  
    delay(1000);                       // short wait to ensure WIFI_OFF  
    WiFi.persistent(false);            // avoid that WiFi-parameters will be stored in persistent memory  

    Serial.println("Start soft-AP ...");  

    // configure AP with fixed IP-address
    bool successAPConfig = false;
    successAPConfig = WiFi.softAPConfig(local_IP, gateway, subnet);
    
    // if config-setting not successful  
    if ( !successAPConfig ) 
    {         
        Serial.println("###ERR: softAPConfig failed with SSID: ");  
        Serial.print (AP_SSID);  
    }
    delay(1000);  

    //Set Access Point Only 
    WiFi.mode(WIFI_AP);
    delay(1000);  

    // avoid that congifuration is written to persistent memory  
    WiFi.persistent(false);

    // Start ESP as Access-Point  
    bool successAPCreated = WiFi.softAP(AP_SSID, AP_PASSWORD);

    if ( successAPCreated ) 
    {  
        Serial.print("WLAN-AP start successful; IP-Address: ");  
        Serial.println( WiFi.softAPIP().toString() );    
    }
    else  
    {  
        Serial.print("### Start AccessPoint failed!");  
    }

    return successAPCreated;
}