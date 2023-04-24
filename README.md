## Micromation Dev board V3 Library for Arduino IDE >= 2.0.0
This library is designed to make it easy to use micromation dev board V3 with ESP32 relays module, Builtin wifi management system, custom HTML webpage, supported 3 pages OLED screen, custom switch, relays, RS485, buzzer. It is easy to use for not only beginners but also experienced users.

# Getting start guide

## How to install
- Open Arduino IDE app
- Click Library manager on sidebar menu
- Search for MicromationDevboardV3
- Select any version then click install

## How to use

Example code to start using the library.
```
#include <iMi3Device.h>

iMi3Device::iMi3DeviceConfig config = {
    "iMi3Device",                                            // device name
    "dddddddd",                                              // device password
    "yourWiFiSSID",                                                    // WiFi SSID
    "yourWiFiPass",                                            // WiFi password
    {"Custom1", "Custom2", "Custom3", "Custom4", "Custom5"}, // Custom field labels
    2,                                                       // mode 1 = AP, 2 = STA
    1,                                                       // OLED pages
    true                                                     // debug mode
};

iMi3Device iMi3(config);

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
    Serial.print("Devvice pass: ");
    Serial.println(iMi3.getData("device_password"));
  }
}

```
For more example please check the example menu in arduino IDE.

[Learn more about Micromation Dev board V3](https://www.imiconsystem.com/product/new-micromation-dev-board-v3-lite-with-esp32-and-enclosure/)
