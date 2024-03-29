#include <MicromationDevboardV3.h>

MicromationDevboardV3::iMi3Config config = {
    "Mi3Dev",                                            // device name
    "dddddddd",                                              // device password
    "yourWiFiSSID",                                          // WiFi SSID
    "yourWiFiPass",                                          // WiFi password
    {"Custom1", "Custom2", "Custom3", "Custom4", "Custom5","Custom6", "Custom7", "Custom8", "Custom9", "Custom10"}, // Custom field labels
    2,                                                       // mode 1 = AP, 2 = STA
    4,                                                       // relayChanels 4 OR 9 
    2,                                                       // OLED pages
    true                                                    // debug mode
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
    webpageLoop();
  }
}

void webpageLoop()
{
  float WINSPD = random(1, 300);
  char html[3500];
  String relay1Text, relay2Text, relay3Text, relay4Text, maxSpd1Class, maxSpd2Class, maxSpd3Class, maxSpd4Class, webtitle, alarmlev;

  int maxSpd1, maxSpd2, maxSpd3, maxSpd4;
  maxSpd1 = 10;
  maxSpd2 = 20;
  maxSpd3 = 100;
  maxSpd4 = 200;

  relay1Text = "OFF";
  relay2Text = "OFF";
  relay3Text = "OFF";
  relay4Text = "OFF";

  webtitle = "Custom Web Page";
  alarmlev = "Calm";

  if (WINSPD > maxSpd1)
  {
    maxSpd1Class = "alert";
    relay1Text = "ON";
  }
  else
    maxSpd1Class = "normal";

  if (WINSPD > maxSpd2)
  {
    maxSpd2Class = "alert";
    relay2Text = "ON";
  }
  else
    maxSpd2Class = "normal";

  if (WINSPD > maxSpd3)
  {
    maxSpd3Class = "alert";
    relay3Text = "ON";
  }
  else
    maxSpd3Class = "normal";

  if (WINSPD > maxSpd4)
  {
    maxSpd4Class = "alert";
    relay4Text = "ON";
  }
  else
    maxSpd4Class = "normal";

  snprintf(html, 3500,
           "<!DOCTYPE html><html><head> <meta charset=\"UTF-8\"/> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> <meta http-equiv='refresh' content='10'/> <style>.max-val span, h3{text-align: center}#pm25-num sup, body{font-family: Arial, Helvetica, sans-serif}html{width: 100vw; height: 100vh}body{margin: 0 30px; background: #001213; background: linear-gradient(126deg, #001213 0, #005157 100%%); color: #eee;}.val{font-size: 30px; display: block; margin-bottom: 10px}.pm-other .val{text-align: right}.max-val span{display: inline-block; background-color: #ffffff21; border-radius: 5px; padding: 5px; font-size: 10px; margin: 3px;}.alert{background-color: #00be8e!important}h1{font-size: 16px; margin: 0 0 3px; padding: 0; color: #89ffae}h3{font-size: 22px; margin-top: 0}*{box-sizing: border-box}.flex-container{display: flex; flex-direction: row; text-align: left}.flex-item-left{padding: 10px 20px 10px 0; flex: 50%%; border-right: 1px solid #ffffff52; position: relative;}.flex-item-right{padding: 10px 0 10px 20px; flex: 50%%;}.flex-item-right sup{font-size: 15px}#pm25-num{font-size: 100px; line-height: 100px}#pm25-num sup{font-size: 20px}.emoticon{font-size: 10rem; filter: grayscale(1); opacity: .2; position: absolute; right: 0}@media (max-width:660px){.emoticon{top: 0}.flex-container{flex-direction: column}.flex-item-left{padding: 10px 0; border-right: 0; border-bottom: 1px solid #ffffff52}.flex-item-right{padding: 10px 0}}.container{max-width: 800px; margin: auto;}</style></head><body> <div class=\"container\"> <h3 style=\"display: flex;align-items: center;column-gap: 10px;justify-content: center;\"><span style='font-size:50px;filter: grayscale(1);'>&#9728;</span> %s</h3> <div class=\"flex-container\"> <div class=\"flex-item-left\"> <div class=\"pm25\" style=\"display: flex;flex-direction: row;justify-content: space-between;flex-wrap: nowrap;\"> <div class=\"\"> <h1>Wind Speed km/h</h1> <div id=\"pm25-num\" class=\"val\">%.2f </div><h1>Beaufort Wind Scale</h1> <span class=\"val\">%s</span> </div></div></div><div class=\"flex-item-right\"> <div class=\"\"> <div class=\"\" style=\"display: flex;flex-direction: column;\"> <div style=\"display: flex;flex-wrap: nowrap;justify-content: space-between;\"> <div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY1</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">SPEED > %d</span> </div></div><div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY2</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">SPEED > %d</span> </div></div></div></div></div><div class=\"\"> <div class=\"\" style=\"display: flex;flex-direction: column;\"> <div style=\"display: flex;flex-wrap: nowrap;justify-content: space-between;\"> <div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY3</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">SPEED > %d</span> </div></div><div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY4</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">SPEED > %d</span> </div></div></div></div></div></div></div><p style=\"text-align: center;\"><b>Domain:</b> %s.local</p></div></body></html>",

           webtitle.c_str(), WINSPD, alarmlev.c_str(), relay1Text.c_str(), maxSpd1Class.c_str(), maxSpd1, relay2Text.c_str(), maxSpd2Class.c_str(), maxSpd2, relay3Text.c_str(), maxSpd3Class.c_str(), maxSpd3, relay4Text.c_str(), maxSpd4Class.c_str(), maxSpd4, iMi3.getData("device_name").c_str());

  iMi3.html = html;

  // to see the web site open your web browser and connect to your device local domain or IP address.
}