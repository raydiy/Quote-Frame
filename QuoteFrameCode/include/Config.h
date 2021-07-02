
// Replace with your network credentials
//const char* STA_SSID = "";
//const char* STA_PASSWORD = "";

const char *AP_SSID = "QuoteFrame";
const char *AP_PASSWORD = "12345678";

uint16_t QUOTEID = 0;
bool EXPORT_TO_JSON = false;


#define TIME_TO_DEEPSLEEP 60*2    // in seconds, how long after a new quote gets loaded. 60*60*24 would be once per day. 60 would be every minute
#define TOUCH_THRESHOLD 20      //Define touch sensitivity. Greater the value, more the sensitivity.
#define TOUCH_PIN 32
#define SCREEN_ROTATION 0

#define MODE_FRAME 0
#define MODE_CONFIG 1
uint8_t MODE = MODE_FRAME;

IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);