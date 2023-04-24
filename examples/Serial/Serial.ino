#include <MicromationDevboardV3.h>

MicromationDevboardV3::iMi3Config config = {
  "Mi3Dev",                                                   // device name
  "dddddddd",                                                 // device password
  "yourWiFiSSID",                                             // WiFi SSID
  "yourWiFiPass",                                             // WiFi password
  { "Custom1", "Custom2", "Custom3", "Custom4", "Custom5" },  // Custom field labels
  2,                                                          // mode 1 = AP, 2 = STA
  2,                                                          // OLED pages
  false                                                        // debug mode
};

MicromationDevboardV3 iMi3(config);

const int numBytes = 8;

byte req1[numBytes] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };  // BGT-WSD2 humi & temp
byte req2[numBytes] = { 0x02, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x38 };  // RS FS wind speed query command
byte req3[numBytes] = { 0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xE9 };  // RS FX wind direction
byte buffQuery[numBytes];

void setup() {
  Serial.begin(115200);
  iMi3.doSetup();  // Device setup (must be called in setup)

}

unsigned long previousMillis = 0;
unsigned long interval = 5000;


uint8_t i = 1;
void loop() {
  iMi3.doLoop();  // Device loop (must be called in loop)

  unsigned long currentMillis = millis();
  while (currentMillis - previousMillis >= interval)  // None blocking delay
  {
    previousMillis = currentMillis;
    Serial.println("Query: " + String(i));
    switch (i) {
      case 1:
        iMi3.updateBaudRate(9600);
        memcpy(buffQuery, req1, numBytes);
        break;
      case 2:
        iMi3.updateBaudRate(9600);
        memcpy(buffQuery, req2, numBytes);
        break;
      case 3:
        iMi3.updateBaudRate(4800);
        memcpy(buffQuery, req3, numBytes);
        break;
      default:
        break;
    }
    iMi3.SerialProcessData(buffQuery);
    Serial.print("ID: ");
    Serial.println(iMi3.serialData.id);
    Serial.print("Data1: ");
    Serial.println(iMi3.serialData.data1);
    Serial.print("Data2: ");
    Serial.println(iMi3.serialData.data2);
    Serial.println("");
    i++;
    if (i > 3) i = 1;
  }
}
