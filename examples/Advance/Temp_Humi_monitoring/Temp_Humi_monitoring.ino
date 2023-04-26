//  ระบบวัดอุณหภูมิและความชื้น เพื่อการแจ้งเตือน หรือควบคุมสั่งการอัตโนมัติ พร้อม Web Monitoring & WiFi management ใช้ในการแสดงผลค่าต่างที่วัดได้ และการตั้งค่าต่างๆ ผ่านเว็บบราวเซอร์
//
// ไฟล์ Sketch ทั้งหมดประกอบด้วย
// 01_oled.ino
// 02_serial.ino
// 03_webpage.ino
// 04_relay.ino
// Temp_Humi_monitoring.ino

// Learn more at https://github.com/imiconsystem/MicromationDevboardV3

#include <MicromationDevboardV3.h>

MicromationDevboardV3::iMi3Config config = {
    "Mi3Dev",                                              // device name
    "dddddddd",                                            // device password
    "yourWiFiSSID",                                        // WiFi SSID
    "yourWiFiPass",                                        // WiFi password
    {"MinTemp", "MaxTemp", "MinHumi", "MaxHumi", "Unuse"}, // Custom field labels
    2,                                                     // mode 1 = AP, 2 = STA
    2,                                                     // OLED pages
    false                                                  // debug mode
};

struct WEBINFO
{
  String stateText;
  String icon;
  String cssClass;
};
struct WEBINFO R1info = {"OFF", "", "normal"};
struct WEBINFO R2info = {"OFF", "", "normal"};
struct WEBINFO R3info = {"OFF", "", "normal"};
struct WEBINFO R4info = {"OFF", "", "normal"};

MicromationDevboardV3 iMi3(config);

int32_t setVar1, setVar2, setVar3, setVar4;
float_t TEMP, HUMI;

void setup()
{
  setVar1 = setVar2 = setVar3 = setVar4 = 0;

  setVar1 = iMi3.getDataInt("custom1");
  setVar2 = iMi3.getDataInt("custom2");
  setVar3 = iMi3.getDataInt("custom3");
  setVar4 = iMi3.getDataInt("custom4");

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