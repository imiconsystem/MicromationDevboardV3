#include <MicromationDevboardV3.h>

MicromationDevboardV3::iMi3Config config = {
    "Mi3Dev",                                           // device name
    "dddddddd",                                         // device password
    "yourWiFiSSID",                                               // WiFi SSID
    "yourWiFiPass",                                       // WiFi password
    {"MaxSP1", "MaxSP2", "MaxSP3", "MaxSP4", "MaxSP5"}, // Custom field labels
    2,                                                  // mode 1 = AP, 2 = STA
    1,                                                  // OLED pages
    false                                               // debug mode
};

struct ALARMLEV
{
  String word;
  String icon;
};
struct ALARMLEV alarmlev = {};

MicromationDevboardV3 iMi3(config);

int32_t maxSpd1, maxSpd2, maxSpd3, maxSpd4, maxSpd5;
float WINSPD;

void setup()
{
  maxSpd1 = maxSpd2 = maxSpd3 = maxSpd4 = maxSpd5 = WINSPD = 0;

  maxSpd1 = iMi3.getDataInt("custom1");
  maxSpd2 = iMi3.getDataInt("custom2");
  maxSpd3 = iMi3.getDataInt("custom3");
  maxSpd4 = iMi3.getDataInt("custom4");
  maxSpd5 = iMi3.getDataInt("custom5");

  Serial.begin(115200);

  iMi3.doSetup();
  serailSetup();
}

void loop()
{
  iMi3.doLoop();
  oledLoop();
  serailLoop();
  webpageLoop();
  relayLoop();
}