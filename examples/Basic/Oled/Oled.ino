#include <MicromationDevboardV3.h>

MicromationDevboardV3::iMi3Config config = {
    "Mi3Dev",                                            // device name
    "dddddddd",                                              // device password
    "yourWiFiSSID",                                          // WiFi SSID
    "yourWiFiPass",                                          // WiFi password
    {"Custom1", "Custom2", "Custom3", "Custom4", "Custom5","Custom6", "Custom7", "Custom8", "Custom9", "Custom10"}, // Custom field labels
    2,                                                       // mode 1 = AP, 2 = STA
    4,                                                       // relayChanels 4 OR 9 
    2,                                                       // OLED pages (max 2)
    true                                                     // debug mode
};

MicromationDevboardV3 iMi3(config);

void setup()
{
    Serial.begin(115200);
    iMi3.doSetup(); // Device setup (must be called in setup)
}

void loop()
{
    iMi3.doLoop(); // Device loop (must be called in loop)
    oledLoop();
}

void oledLoop(void) {
  int WINSPD = random(1, 500);
  String msg1;
  msg1 = "Wind speed =  ";
  msg1.concat(WINSPD);

  MicromationDevboardV3::iMi3OledMessage myMsg[3];

  myMsg[0].tp = 2;  // Template ID 2 
  myMsg[0].msg1 = "SPD"; // Top left text
  myMsg[0].msg2 = "km/h"; // Bottom right text
  myMsg[0].msg3 = "";
  myMsg[0].msg4 = "";
  myMsg[0].msg5 = round(WINSPD); // Interger value
  iMi3.printScreen(PAGE_1, myMsg[0]);

  myMsg[1].tp = 1; // Template ID 1
  myMsg[1].msg1 = msg1.c_str(); // line 1 display concat string
  myMsg[1].msg2 = "Second line"; // line 2
  myMsg[1].msg3 = "Third line"; // line 3
  myMsg[1].msg4 = "Fourth line"; // line 4
  myMsg[1].msg5 = 0; // not display
  iMi3.printScreen(PAGE_2,myMsg[1]);

}
