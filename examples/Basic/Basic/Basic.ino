#include <MicromationDevboardV3.h>

MicromationDevboardV3::iMi3Config config = {
    "Mi3Dev",                                            // device name
    "dddddddd",                                              // device password
    "yourWiFiSSID",                                                    // WiFi SSID
    "yourWiFiPass",                                            // WiFi password
    {"Custom1", "Custom2", "Custom3", "Custom4", "Custom5","Custom6", "Custom7", "Custom8", "Custom9", "Custom10"}, // Custom field labels
    2,                                                       // mode 1 = AP, 2 = STA
    4,                                                       // relayChanels 4 OR 9 
    1,                                                       // OLED pages
    true                                                     // debug mode
};

MicromationDevboardV3 iMi3(config);

void setup()
{
  Serial.begin(115200);
  iMi3.doSetup(); // Device setup (must be called in setup)
}

unsigned long previousMillis = 0;
unsigned long interval = 1000;

void loop()
{
  iMi3.doLoop(); // Device loop (must be called in loop)

  unsigned long currentMillis = millis();
  while (currentMillis - previousMillis >= interval) // None blocking delay
  {
    previousMillis = currentMillis;
    Serial.print("WiFi SSID: ");
    Serial.println(iMi3.getData("wifi_ssid"));
    Serial.print("WiFi pass: ");
    Serial.println(iMi3.getData("wifi_password"));
    Serial.print("Device state: "); // 0 = run, 1 = setup
    Serial.println(iMi3.getState());
    Serial.print("Device run mode: "); // mode 1 = AP, 2 = STA
    Serial.println(iMi3.getDataInt("dev_run_mode"));
    Serial.print("Device name (SSID): ");
    Serial.println(iMi3.getData("device_name"));
    Serial.print("Device pass: ");
    Serial.println(iMi3.getData("device_password"));
  }
}
