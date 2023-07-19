
//  ระบบ ระบบวัดทิศทางลม เพื่อการแจ้งเตือน หรือควบคุมสั่งการอัตโนมัติ พร้อม Web Monitoring & WiFi management ใช้ในการแสดงผลค่าต่างที่วัดได้ และการตั้งค่าต่างๆ ผ่านเว็บบราวเซอร์
//
// ไฟล์ Sketch ทั้งหมดประกอบด้วย
// 01_oled.ino
// 02_serial.ino
// 03_webpage.ino
// 04_relay.ino
// Wind_direction.ino

// Learn more at https://github.com/imiconsystem/MicromationDevboardV3
#include <MicromationDevboardV3.h>

MicromationDevboardV3::iMi3Config config = {
    "Mi3Dev",                                           // device name
    "dddddddd",                                         // device password
    "yourWiFiSSID",                                        // WiFi SSID
    "yourWiFiPass",                                        // WiFi password
    {"Max1", "Max2", "Max3", "Max4", "Max5"}, // Custom field labels
    2,                                                  // mode 1 = AP, 2 = STA
    4,                                                       // relayChanels 4 OR 9 
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

int32_t WINDIR, setVar1, setVar2, setVar3, setVar4;

void setup()
{
  setVar1 = setVar2 = setVar3 = setVar4 = WINDIR = 0;

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