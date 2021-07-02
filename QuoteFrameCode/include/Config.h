// Here you can configure some default values

const char *AP_SSID = "QuoteFrame";         // the SSID of the gateway
const char *AP_PASSWORD = "12345678";       // tha password of the gateway

uint16_t QUOTEID = 0;                       // start displaying at quote id 0
bool EXPORT_TO_JSON = false;                // needed for the JSON export in config mode


#define TIME_TO_DEEPSLEEP 60*2              // in seconds, how long after a new quote gets loaded. 60*60*24 would be once per day. 60 would be every minute
#define TOUCH_THRESHOLD 20                  // Define touch sensitivity. Greater the value, more the sensitivity.
#define TOUCH_PIN 32                        // the pin of the touch sensor
#define SCREEN_ROTATION 0                   // if for some reasons you want to rotate the display

#define MODE_FRAME 0                        // mode 0 is normal quotes mode
#define MODE_CONFIG 1                       // mode 1 is config mode
uint8_t MODE = MODE_FRAME;                  // MODE does contain the current active mode

IPAddress local_IP(192,168,4,1);            // the default IP adress you find the gateway at 
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);