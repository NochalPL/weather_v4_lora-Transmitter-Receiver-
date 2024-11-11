//===========================================
//WiFi connection
//===========================================
char ssid[] = "ssid";      // WiFi Router ssid
char pass[] = "password";  // WiFi Router password

//===========================================
//MQTT broker connection
//===========================================
const char* mqttServer = "91.121.93.94";  //test.mosquitto.org
//const char* mqttServer = "192.168.5.66";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";
const char mainTopic[20] = "RoyalGorge/";
#define RETAIN false

//===========================================
//ADC calibration
//===========================================
#define ADCBattery 371 
#define ADCSolar 195

//===========================================
//Altitude offsets
//===========================================
#define OFFSET_IN 5.58 
#define OFFSET_MM 141.7