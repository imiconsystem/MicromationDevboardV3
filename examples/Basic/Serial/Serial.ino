#include <iMi3Device.h>

iMi3Device::iMi3DeviceConfig config = {
    "iMi3Device",                                            // device name
    "dddddddd",                                              // device password
    "yourWiFiSSID",                                                    // WiFi SSID
    "yourWiFiPass",                                                  // WiFi password
    {"Custom1", "Custom2", "Custom3", "Custom4", "Custom5"}, // Custom field labels
    2,                                                       // mode 1 = AP, 2 = STA
    2,                                                       // OLED pages
    false                                                     // debug mode
};

iMi3Device iMi3(config);

struct iMi3Device::iMi3SerialInput serailInputs;

byte req[8] = { 0x02, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x38 };  // RS FS wind speed query command

void setup()
{
  Serial.begin(115200);
  iMi3.doSetup(); // Device setup (must be called in setup)

  serailInputs.name = "SPD1"; // name your input device
  serailInputs.buadrate = 9600; // device buadrate
  memcpy(serailInputs.request_command, req, 8);

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
    serailLoop();
  }
}


void serailLoop() {

  iMi3.readSerial(SERIAL_INPUT1, serailInputs);
  Serial.print("Data1: ");
  Serial.println(iMi3.serialData.data1);
  Serial.print("Data2: ");
  Serial.println(iMi3.serialData.data2);
  
}