#include "Arduino.h"
#include "MicromationDevboardV3.h"
#include <ezButton.h>
#include <Preferences.h>
#include <nvs_flash.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <U8g2lib.h>
#include "pitches.h"

U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

WebServer server(80);
String authFailResponse = "Authentication Failed";
Preferences preferences;
const char *prekey = "APcredentials";

ezButton cswitch(SW, INPUT_PULLDOWN); // INPUT_PULLDOWN

MicromationDevboardV3::MicromationDevboardV3(struct iMi3Config config)
{
    this->html = "Content not found.";
    this->device_name = config.device_name;
    this->device_password = config.device_password;
    this->wifi_ssid = config.wifi_ssid;
    this->wifi_password = config.wifi_password;
    this->mode = config.mode;
    this->oledPages = config.oledPages;
    if (config.oledPages > OLED_PAGES)
    {
        this->oledPages = OLED_PAGES;
    }
    for (int i = 0; i < 5; i++)
    {
        this->custom_field_label[i] = config.custom_field_label[i];
    }

    this->debug = config.debug;
    // this->state = 0; // 0 = run, 1 = setup
    this->state = 0;

    this->btnState = 0; // 0 = no press, 1 = press, 2 = long press
    this->switchPressTime = 0;
}

void MicromationDevboardV3::doSetup()
{
    this->oledSetup();
    this->serialSetup();
    this->relaySetup();
    this->switchSetup();
    this->storageSetup();
    this->beep(NOTE_E6, 100);
    if (this->getState() == 1) // SET
    {

        if (this->debug)
        {
            this->serialdebug("getState() SET running config Form");
        }
        this->wifiapSetup(true);       // AP for setup
        this->draw_state = OLED_PAGES; // จะแสดงหน้าสุดท้ายของ OLED สำหรับการตั้งค่า
    }
    else if (this->getState() == 2)
    { // RESET FACTORY
        if (this->debug)
        {
            this->serialdebug("state RESET");
        }
        // storageClear();
        ESP.restart();
    }
    else
    {
        // RUN
        if (this->debug)
        {
            this->serialdebug("state RUN");
        }

        if (this->getDataInt("dev_run_mode") == 2)
        {
            this->webserverSetup(); // ST mode
        }
        else if (this->getDataInt("dev_run_mode") == 1 && this->getState() == 0)
        {
            this->wifiapSetup(false); // AP for Run
        }
        else
        {
            this->wifiapSetup(true); // SETUP
        }
    }
}
void MicromationDevboardV3::doLoop(void)
{

    this->switchLoop();
    this->oledLoop();

    if (this->getDataInt("dev_run_mode") == 2)
    {
        this->webserverLoop();
        this->clientLoop();
    }
    else
    {
        clientLoop();
    }
}
void MicromationDevboardV3::setState(int state)
{
    this->state = state;
    this->setStorageInt("device_state", state);
}
int MicromationDevboardV3::getState(void)
{

    return this->getDataInt("device_state", this->state);
}

String MicromationDevboardV3::getName(void)
{
    return this->getData("device_name", this->device_name);
}

// รับค่าจากปุ่ม และกำหนดค่า mode ให้กับอุปกรณ์
void MicromationDevboardV3::switchSetup()
{
    cswitch.setDebounceTime(50);
    cswitch.setCountMode(COUNT_FALLING);
}

void MicromationDevboardV3::switchLoop()
{

    cswitch.loop();
    this->switchState = 0;

    if (cswitch.getState() == 1)
    {
        this->switchState = 1;

        this->switchPressTime++;

        if (this->switchPressTime > 100 && this->switchPressTime < 200)
        {
            this->switchState = 2; // LONG PRESS
        }
        else if (this->switchPressTime > 500)
        {
            // Toggle state when press more than 5 seconds
            if (this->getState() == 1) // SET
            {
                this->setState(0); // RUN
            }
            else
            {
                this->setState(1); // SET
            }

            this->setStorageInt("device_state", this->state);
            this->beep(NOTE_E6, 100);
            ESP.restart();
        }
    }
    else
    {
        this->switchState = 0;
        this->switchPressTime = 0;
    }

    this->switchState = cswitch.getState();
    this->switchIsPressed = cswitch.isPressed();
    this->switchIsReleased = cswitch.isReleased();

    if (cswitch.isPressed())
    {
        this->log_string = "The button is pressed";
        if (this->debug)
            this->serialdebug("The button is pressed");
    }

    if (cswitch.isReleased())
    {
        if (this->debug)
            this->serialdebug("The button is released");
    }
}

void MicromationDevboardV3::beep(int note, int duration)
{
    if (this->debug)
    {
        this->serialdebug("Beep");
    }
    tone(BUZZZER_PIN, note, duration);
    delay(duration);
}
void MicromationDevboardV3::noBeep()
{
    if (this->debug)
    {
        this->serialdebug("Quiet");
    }
    noTone(BUZZZER_PIN);
}
// Storage
void MicromationDevboardV3::storageSetup()
{

    String _wifi_ssid, _wifi_password, _webpage_title, _device_name, _device_password;
    int _mode, custom1, custom2, custom3, custom4, custom5, _device_state;

    preferences.begin(prekey, false);

    _wifi_ssid = preferences.getString("wifi_ssid", "");
    _wifi_password = preferences.getString("wifi_password", "");

    if (_wifi_ssid == "" || _wifi_password == "")
    {
        preferences.putString("wifi_ssid", this->wifi_ssid);
        preferences.putString("wifi_password", this->wifi_password);
        if (this->debug)
        {
            this->serialdebug("No values saved for WIFssid or WiFipassword");
            this->serialdebug("WIFI Credentials Saved using default value.");
        }
    }

    _device_name = preferences.getString("device_name", "");
    _device_password = preferences.getString("device_password", "");

    if (_device_name == "" || _device_password == "")
    {
        preferences.putString("device_name", this->device_name);
        preferences.putString("device_password", this->device_password);
        if (this->debug)
        {
            this->serialdebug("No values saved for device_name or device_password");
            this->serialdebug("Device Credentials Saved using default value.");
        }
    }

    _device_state = preferences.getInt("device_state", 0);
    if (_device_state == 0)
    {
        preferences.putInt("device_state", 0);
        if (this->debug)
        {
            this->serialdebug("No values saved for device_state");
            this->serialdebug("Device State Saved using default value.");
        }
    }

    _mode = preferences.getInt("dev_run_mode", 0);
    if (_mode == 0)
    {
        preferences.putInt("dev_run_mode", this->mode);
        if (this->debug)
        {
            this->serialdebug("No values saved for mode");
            this->serialdebug("Mode Saved using default value.");
        }
    }

    custom1 = preferences.getInt("custom1", 0);
    if (custom1 == 0)
    {
        preferences.putInt("custom1", 0);
    }

    custom2 = preferences.getInt("custom2", 0);
    if (custom2 == 0)
    {
        preferences.putInt("custom2", 0);
    }

    custom3 = preferences.getInt("custom3", 0);
    if (custom3 == 0)
    {
        preferences.putInt("custom3", 0);
    }

    custom4 = preferences.getInt("custom4", 0);
    if (custom4 == 0)
    {
        preferences.putInt("custom4", 0);
    }

    custom5 = preferences.getInt("custom5", 0);
    if (custom5 == 0)
    {
        preferences.putInt("custom5", 0);
    }

    preferences.end();
}
void MicromationDevboardV3::setStorageInt(const char *key, int value)
{
    preferences.begin(prekey, false);
    preferences.putInt(key, value);
    preferences.end();
}
void MicromationDevboardV3::setStorage(const char *key, String value)
{
    preferences.begin(prekey, false);
    preferences.putString(key, value);
    preferences.end();
}
String MicromationDevboardV3::getStorage(const char *key)
{
    String value;
    preferences.begin(prekey, false);
    value = preferences.getString(key, "");
    preferences.end();
    return value;
}
int MicromationDevboardV3::getStorageInt(const char *key)
{
    int value;
    preferences.begin(prekey, false);
    value = preferences.getInt(key, 0);
    preferences.end();
    return value;
}
String MicromationDevboardV3::getData(const char *key)
{
    if (this->getStorage(key) != "")
    {
        return this->getStorage(key);
    }
    else
    {
        return "";
    }
}
int MicromationDevboardV3::getDataInt(const char *key)
{
    if (this->getStorageInt(key) != 0)
    {
        return this->getStorageInt(key);
    }
    else
    {
        return 0;
    }
}
int MicromationDevboardV3::getDataInt(const char *key, int old)
{
    if (this->getStorageInt(key) != 0)
    {
        return this->getStorageInt(key);
    }
    else
    {
        return old;
    }
}
String MicromationDevboardV3::getData(const char *key, String old)
{
    if (this->getStorage(key) != "")
    {
        return this->getStorage(key);
    }
    else
    {
        return old;
    }
}
// WIFI AP
void MicromationDevboardV3::wifiapSetup(bool isAPSET)
{
    if (this->debug)
    {
        this->serialdebug("Configuring access point...");
        this->serialdebug("AP Name: ");
        this->serialdebug(this->getData("device_name", this->device_name));
        this->serialdebug("AP Password: ");
        this->serialdebug(String(this->device_password));
    }

    WiFi.softAP(this->getData("device_name", this->device_name).c_str(), this->getData("device_password", this->device_password).c_str());
    IPAddress myIP = WiFi.softAPIP();
    this->device_ip = myIP.toString();
    if (this->debug)
    {
        this->serialdebug("AP IP address: ");
        this->serialdebug(String(myIP));
    }
    if (isAPSET)
    {
        this->serialdebug("RUN config server");
        // server.on("/", this->handleConfig);
        String dev_name = this->getData("device_name", this->device_name);
        char html[4000];
        snprintf(html, 4000, "<html>\
<head>\
<meta charset=\"UTF-8\" />\
<title>System configuration.</title>\
<style>\
   html{width:100%%;height:100%%}body{margin:0 30px;background:#005157;font-family:Arial,Helvetica,sans-serif;font-size:1.8rem;color:#eee}input:not([type=checkbox]),select,option{padding:20px;font-size:20px;width:100%%;margin:10px 0;font-weight:700;font-size:2rem;border:0;border-radius:5px;}legend h1{font-size:2.5rem;;color:#7fffd4}input[type=submit]{background-color:#32ab4e;font-size:4rem;}\
</style>\
</head>\
<body>\
<div style=\"width:95vw;margin:auto;\">\
<h1>System configuration.</h1>\
<form action=\"/saveConfig\" method=\"POST\">\
<fieldset>\
<legend>\
<h1>WiFi Router credential:</h1>\
</legend>\
<h3>SSID:</h3>\
<div><input type=\"text\" name=\"WiFissid\" value=\"%s\" maxlength=\"10\"></div>\
<h3>PASSWORD:</h3>\
<div><input type=\"text\" name=\"WiFipassword\" value=\"%s\" maxlength=\"10\"></div>\
</fieldset>\
<fieldset>\
<legend>\
<h1>Set max values</h1>\
</legend>\
<h1>%s</h1>\
<div><input type=\"number\"  name=\"custom1\" value=\"%ld\" size=\"5\"></div>\
<h1>%s</h1>\
<div><input type=\"number\"  name=\"custom2\" value=\"%ld\" size=\"5\"></div>\
<h1>%s</h1>\
<div><input type=\"number\"  name=\"custom3\" value=\"%ld\" size=\"5\"></div>\
<h1>%s</h1>\
<div><input type=\"number\"  name=\"custom4\" value=\"%ld\" size=\"5\"></div>\
<h1>%s</h1>\
<div><input type=\"number\"  name=\"custom5\" value=\"%ld\" size=\"5\"></div>\
</fieldset>\
<fieldset>\
<legend>\
<h1>Device setting:</h1>\
</legend>\
<h1>Device runing mode:</h1>\
<div><select name=\"devRunmode\"><option value=\"1\">Accesspoint mode</option><option value=\"2\">Station mode</option></select></div>\
<h1>WiFi AP mode credential:</h1>\
<h3>Device name (SSID):</h3>\
<div><input type=\"text\" name=\"APssid\" value=\"%s\" maxlength=\"10\"></div>\
<h3>PASSWORD:</h3>\
<div><input type=\"text\" name=\"APpassword\" value=\"%s\" maxlength=\"10\"></div>\
</fieldset>\
<div><input type=\"submit\" value=\"SAVE\"></div>\
</form>\
<div style=\"text-align: center;\">\
<hr> iMiconSystem.com</div>\
</div>\
</body>\
</html>",

                 this->getData("wifi_ssid").c_str(), this->getData("wifi_password").c_str(), this->custom_field_label[0].c_str(), this->getData("custom1").toInt(), this->custom_field_label[1].c_str(), this->getData("custom2").toInt(), this->custom_field_label[2].c_str(), this->getData("custom3").toInt(), this->custom_field_label[3].c_str(), this->getData("custom4").toInt(), this->custom_field_label[4].c_str(), this->getData("custom5").toInt(), this->getData("device_name").c_str(), this->getData("device_password").c_str());

        this->systemhtml = html;

        server.on("/", [&]()
                  { server.send(200, "text/html", this->systemhtml); });
        server.on("/saveConfig", this->saveConfig);
    }
    else
    {
        this->serialdebug("RUN normal server");
        server.on("/", [&]()

                  { server.send(200, "text/html", this->html); });
        // server.on("/", this->handleRoot);
    }

    server.onNotFound(this->handleNotFound);

    server.begin();
    if (this->debug)
        this->serialdebug("AP mode started");
}
// WIFI STA

void MicromationDevboardV3::webserverSetup()
{
    unsigned long webcurrentMillis = millis();
    if (this->debug)
    {
        this->serialdebug("WiFissid");
        this->serialdebug(this->getData("wifi_ssid", this->wifi_ssid));
        this->serialdebug("WiFipassword");
        this->serialdebug(this->getData("wifi_password", this->wifi_password));
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(this->getData("wifi_ssid", this->wifi_ssid).c_str(), this->getData("wifi_password", this->wifi_password).c_str());

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED && (webcurrentMillis - this->webpreviousMillis >= this->devinterval))
    {
        // this->serialdebug(WiFi.status());
        delay(1000);
        this->webpreviousMillis = webcurrentMillis;
    }

    if (MDNS.begin(this->getData("device_name", this->device_name).c_str()))
    {
        if (this->debug)
        {
            this->serialdebug("MDNS responder started");
            this->serialdebug(this->getData("device_name", this->device_name));
        }
    }

    server.on("/", [&]()
              { server.send(200, "text/html", this->html); });
    // server.on("/", this->handleRoot);

    server.onNotFound(this->handleNotFound);

    server.begin();
    if (this->debug)
        this->serialdebug("HTTP server started");
}

void MicromationDevboardV3::webserverLoop()
{
    unsigned long sercurrentMillis = millis();

    // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
    while ((WiFi.status() != WL_CONNECTED) && (sercurrentMillis - this->serpreviousMillis >= this->devinterval))
    {
        // this->serialdebug(WiFi.status());
        if (this->debug)
            this->serialdebug("Reconnecting to WiFi...");

        WiFi.disconnect();
        WiFi.reconnect();
        delay(1000);
        this->serpreviousMillis = sercurrentMillis;
    }

    // this->log_string = WiFi.localIP().toString();

    if (WiFi.status() == WL_CONNECTED)
    {
        this->device_ip = WiFi.localIP().toString();
        if (this->debug)
        {
            // this->serialdebug("Connected to ");
            // this->serialdebug(this->wifi_ssid);
            // this->serialdebug("IP address: ");
            // this->serialdebug(WiFi.localIP());
        }
    }

    // String wf = "WiFi: ";
    // String ip = "IP: ";

    // wf += this->wifi_ssid;
    // ip += WiFi.localIP().toString();

    // const char *messages[] = {
    //     "System info.",
    //     wf.c_str(),
    //     ip.c_str(),
    //     ""};

    // this->printScreen(messages, 3000, FONT_SMALL);
}
void MicromationDevboardV3::clientLoop()
{
    server.handleClient();
    delay(10); // allow the cpu to switch to other tasks
}
void MicromationDevboardV3::handleRoot()
{
    String message = "Hello World";
    server.send(200, "text/plain", message);
}
void MicromationDevboardV3::handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}
void MicromationDevboardV3::handleConfig()
{
    String form = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"icon\" href=\"data:,\">"
                  "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
                  ".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;"
                  "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"
                  ".button2 {background-color: #77878A;}</style></head><body>"
                  "<h1>Access Point</h1>"
                  "<p>Connect to WiFi network and open this page to set up the AP</p>"
                  "<form method='POST' action='/saveConfig'>"
                  "<label for='ssid'>SSID:</label><br>"
                  "<input type='text' id='ssid' name='ssid' value=''><br>"
                  "<label for='password'>Password:</label><br>"
                  "<input type='text' id='password' name='password' value=''><br><br>"
                  "<input type='submit' class='button' value='Submit'>"
                  "</form></body></html>";
    server.send(200, "text/html", form);
}
void MicromationDevboardV3::saveConfig()
{

    int custom1, custom2, custom3, custom4, custom5, _devRunmode;

    custom1 = custom2 = custom3 = custom4 = custom5 = _devRunmode = 0;

    String message = "<h3>Data saved.<br></h3><hr>";
    String WIFIssid, WIFIpassword, APssid, APpassword, _webTitle, _deviceName;
    // message += (server.method() == HTTP_GET) ? "GET" : "POST";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " <b>" + server.argName(i) + ":</b> ______" + server.arg(i) + "______<br>\n";
        if (server.argName(i) == "devRunmode")
        {
            _devRunmode = server.arg(i).toInt();
        }
        else if (server.argName(i) == "APssid")
        {
            APssid = server.arg(i);
        }
        else if (server.argName(i) == "APpassword")
        {
            APpassword = server.arg(i);
        }
        else if (server.argName(i) == "WiFissid")
        {
            WIFIssid = server.arg(i);
        }
        else if (server.argName(i) == "WiFipassword")
        {
            WIFIpassword = server.arg(i);
        }
        else if (server.argName(i) == "custom1")
        {
            custom1 = server.arg(i).toInt();
        }
        else if (server.argName(i) == "custom2")
        {
            custom2 = server.arg(i).toInt();
        }
        else if (server.argName(i) == "custom3")
        {
            custom3 = server.arg(i).toInt();
        }
        else if (server.argName(i) == "custom4")
        {
            custom4 = server.arg(i).toInt();
        }
        else if (server.argName(i) == "custom5")
        {
            custom5 = server.arg(i).toInt();
        }
        else if (server.argName(i) == "web-title")
        {
            _webTitle = server.arg(i);
        }
        else if (server.argName(i) == "device-name")
        {
            _deviceName = server.arg(i);
        }
    }
    message += "<html><head><meta charset=\"UTF-8\">\
  <style>\
   html{width:100%%;height:100%%}body{margin:30px;background:#005157;font-family:Arial,Helvetica,sans-serif;color:#eee}\
</style></head><body><script>alert('Saving data please wait until device restarted.');</script><br>Configuration saved. Please restart the device.<br></body></html>";
    server.send(200, "text/html", message);

    MicromationDevboardV3 device(
        {
            "", // device name
            "", // device password
            "", // WiFi SSID
            "", // WiFi password
            {}, // Max value label
            1,
            2,    // mode 1 = AP, 2 = STA
            false // debug mode
        });

    device.setStorageInt("dev_run_mode", _devRunmode);
    device.setStorage("device_name", APssid);
    device.setStorage("device_password", APpassword);
    device.setStorage("wifi_ssid", WIFIssid);
    device.setStorage("wifi_password", WIFIpassword);
    device.setStorageInt("custom1", custom1);
    device.setStorageInt("custom2", custom2);
    device.setStorageInt("custom3", custom3);
    device.setStorageInt("custom4", custom4);
    device.setStorageInt("custom5", custom5);
    device.setStorageInt("device_state", 0); // Back to run mode
    ESP.restart();
}

String MicromationDevboardV3::getIP(void)
{
    return this->device_ip;
}

// OLED
void MicromationDevboardV3::oledSetup(void)
{
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFontMode(0); // enable transparent mode, which is faster
}

void MicromationDevboardV3::oledLoop(void)
{
    // this->serialdebug("PAGES: " + String(this->draw_state));
    // this->serialdebug("OLED PAGES: " + String(this->oledPages));
    unsigned long oledcurrentMillis = millis();
    // this->serialdebug(oledcurrentMillis - this->oledpreviousMillis);

    if (oledcurrentMillis - this->oledpreviousMillis >= 5000)
    {
        this->oledpreviousMillis = oledcurrentMillis;

        u8g2.firstPage();
        do
        {
            oledDrawState();
        } while (u8g2.nextPage());

        if (this->getState() == 1)
        {
            this->draw_state = 0; // if RESET state, draw last page only
        }
        else
        {
            this->draw_state++;
            if (this->draw_state > this->oledPages)
                this->draw_state = 0;
        }

        // delay(3000);
    }
}

void MicromationDevboardV3::oledDrawState()
{
    switch (this->draw_state)
    {
    case 1:
        oledDrawPage1();
        break;
    case 2:
        oledDrawPage2();
        break;
    case 3:
        oledDrawPage3();
        break;
    case 0:
        oledDrawSysPage();
        break;
    }
    //  case 1:
    //     y[0] = 6;
    //     y[1] = 14;
    //     y[2] = 22;
    //     y[3] = 30;
    //     u8g2.setFont(u8g2_font_5x8_tf);
    //     break;
    // case 2:
    //     y[0] = 7;
    //     y[1] = 15;
    //     y[2] = 23;
    //     y[3] = 31;
    //     u8g2.setFont(u8g2_font_6x10_tf);
    //     break;
    // case 3:
    //     y[0] = 10;
    //     y[1] = 22;
    //     y[2] = 32;
    //     y[3] = 100; // hide
    //     u8g2.setFont(u8g2_font_7x14_tf);
    //     break;
    // case 4:
    //     y[0] = 12;
    //     y[1] = 25;
    //     y[2] = 100; // hide
    //     y[3] = 100; // hide
    //     u8g2.setFont(u8g2_font_ncenB10_tr);
}

void MicromationDevboardV3::oledDrawPage1(void)
{
    // Page breadcum
    // u8g2.drawBox(93, 29, 6, 3);
    // u8g2.drawFrame(93, 29, 6, 3);
    // u8g2.drawFrame(102, 29, 6, 3);
    // u8g2.drawFrame(111, 29, 6, 3);
    // u8g2.drawFrame(120, 29, 6, 3);

    if (this->message[0].tp == 1)
    {
        u8g2.setFont(u8g2_font_5x8_tf);
        u8g2.drawStr(0, 10, this->message[0].msg1.c_str());
        u8g2.drawStr(0, 16, this->message[0].msg2.c_str());
        u8g2.drawStr(0, 24, this->message[0].msg3.c_str());
        u8g2.drawStr(0, 32, this->message[0].msg4.c_str());
    }
    else if (this->message[0].tp == 2)
    {
        u8g2.setFont(u8g2_font_squeezed_b7_tr);
        u8g2.drawStr(0, 8, this->message[0].msg1.c_str());
        u8g2.drawStr(91, 30, this->message[0].msg2.c_str());
        u8g2.setFont(u8g2_font_6x12_t_symbols);
        u8g2.setFont(u8g2_font_7_Seg_33x19_mn);
        u8g2.drawStr(30, 0, u8x8_u8toa(this->message[0].msg5 > 999 ? 999 : this->message[0].msg5, 3));
    }
}

void MicromationDevboardV3::oledDrawPage2(void)
{
    // Page breadcum
    // u8g2.drawBox(102, 29, 6, 3);
    // u8g2.drawFrame(93, 29, 6, 3);
    // u8g2.drawFrame(102, 29, 6, 3);
    // u8g2.drawFrame(111, 29, 6, 3);
    // u8g2.drawFrame(120, 29, 6, 3);

    if (this->message[1].tp == 1)
    {
        u8g2.setFont(u8g2_font_5x8_tf);
        u8g2.drawStr(0, 10, this->message[1].msg1.c_str());
        u8g2.drawStr(0, 16, this->message[1].msg2.c_str());
        u8g2.drawStr(0, 24, this->message[1].msg3.c_str());
        u8g2.drawStr(0, 32, this->message[1].msg4.c_str());
    }
    else if (this->message[1].tp == 2)
    {
        u8g2.setFont(u8g2_font_squeezed_b7_tr);
        u8g2.drawStr(0, 8, this->message[1].msg1.c_str());
        u8g2.drawStr(91, 30, this->message[1].msg2.c_str());
        u8g2.setFont(u8g2_font_6x12_t_symbols);
        u8g2.setFont(u8g2_font_7_Seg_33x19_mn);
        u8g2.drawStr(30, 0, u8x8_u8toa(this->message[1].msg5 > 999 ? 999 : this->message[1].msg5, 3));
    }
}

void MicromationDevboardV3::oledDrawPage3(void)
{
    // Page breadcum
    // u8g2.drawBox(111, 29, 6, 3);
    // u8g2.drawFrame(93, 29, 6, 3);
    // u8g2.drawFrame(102, 29, 6, 3);
    // u8g2.drawFrame(111, 29, 6, 3);
    // u8g2.drawFrame(120, 29, 6, 3);

    if (this->message[2].tp == 1)
    {
        u8g2.setFont(u8g2_font_5x8_tf);
        u8g2.drawStr(0, 10, this->message[2].msg1.c_str());
        u8g2.drawStr(0, 16, this->message[2].msg2.c_str());
        u8g2.drawStr(0, 24, this->message[2].msg3.c_str());
        u8g2.drawStr(0, 32, this->message[2].msg4.c_str());
    }
    else if (this->message[2].tp == 2)
    {
        u8g2.setFont(u8g2_font_squeezed_b7_tr);
        u8g2.drawStr(0, 8, this->message[2].msg1.c_str());
        u8g2.drawStr(91, 30, this->message[2].msg2.c_str());
        u8g2.setFont(u8g2_font_6x12_t_symbols);
        u8g2.setFont(u8g2_font_7_Seg_33x19_mn);
        u8g2.drawStr(30, 0, u8x8_u8toa(this->message[2].msg5 > 999 ? 999 : this->message[2].msg5, 3));
    }
}

void MicromationDevboardV3::oledDrawSysPage(void)
{
    // Page breadcum
    // u8g2.drawBox(120, 29, 6, 3);
    // u8g2.drawFrame(93, 29, 6, 3);
    // u8g2.drawFrame(102, 29, 6, 3);
    // u8g2.drawFrame(111, 29, 6, 3);
    // u8g2.drawFrame(120, 29, 6, 3);

    String WiFiRSSI;

    String devMode[2] = {"AP", "ST"};

    IPAddress apIP = WiFi.softAPIP();

    String staIP = WiFi.localIP().toString();

    WiFiRSSI = WiFi.RSSI();

    u8g2.setFont(u8g2_font_siji_t_6x10);
    u8g2.drawGlyph(0, 8, 0xE219); // wifi
    if (this->getDataInt("dev_run_mode") == 2)
        u8g2.drawGlyph(0, 16, 0xE0F2); // rssi

    u8g2.setFont(u8g2_font_squeezed_b7_tr);
    if (this->getState() == 0 && this->getDataInt("dev_run_mode") == 2)
    {
        u8g2.drawStr(15, 8, this->getData("wifi_ssid", this->wifi_ssid).c_str()); // STA mode
        u8g2.drawStr(15, 17, WiFiRSSI.c_str());
        u8g2.drawFrame(10, 21, 20, 11);
        u8g2.drawStr(16, 30, devMode[1].c_str());
        u8g2.drawStr(40, 17, staIP.c_str());
        String domain = this->getData("device_name") + ".local";
        u8g2.drawStr(40, 25, domain.c_str());
    }
    else
    {
        u8g2.drawStr(15, 8, this->getData("device_name").c_str()); // AP mode
        u8g2.drawStr(15, 17, this->getData("device_password", this->device_password).c_str());
        // u8g2.drawStr(50, 17, apIP.toString().c_str());
        u8g2.drawStr(15, 26, apIP.toString().c_str()); // Reset state

        u8g2.drawFrame(90, 21, 20, 11);
        u8g2.drawStr(96, 30, devMode[0].c_str());
    }
}

void MicromationDevboardV3::printScreen(const uint8_t page, struct MicromationDevboardV3::iMi3OledMessage message)
{
    if (page <= OLED_PAGES)
    {
        this->message[page - 1].tp = message.tp;
        this->message[page - 1].msg1 = message.msg1;
        this->message[page - 1].msg2 = message.msg2;
        this->message[page - 1].msg3 = message.msg3;
        this->message[page - 1].msg4 = message.msg4;
        this->message[page - 1].msg5 = message.msg5;
    }
}

// SESRIAL INPUT

void MicromationDevboardV3::serialSetup()
{
    Serial2.begin(4800); //
}

void MicromationDevboardV3::updateBaudRate(int baud)
{
    Serial2.updateBaudRate(baud);
}

void MicromationDevboardV3::SerialProcessData(byte buffQuery[8])
{
    int numBytes = 8;
    byte ByteArray[9];
    int ByteData[9];
    for (int j = 0; j < numBytes; j++)
    {
        Serial2.write(buffQuery[j]);
    }
    int a = 0;
    while (Serial2.available() > 0)
    {
        ByteArray[a] = Serial2.read();
        // this->serialdebug(ByteArray[a]);
        // this->serialdebug(" ");
        a++;
    }

    ByteData[0] = ByteArray[3] * 256 + ByteArray[4];
    ByteData[1] = ByteArray[5] * 256 + ByteArray[6];

    float Data1, Data2;
    Data1 = ByteData[0];
    Data2 = ByteData[1];

    if (this->debug)
    {
        Serial.print("ID: ");
        Serial.println(String(ByteArray[0]));
        Serial.print(" Data1: ");
        Serial.println(String(Data1));
        Serial.print(" Data2: ");
        Serial.println(String(Data2));
        Serial.println(" ");
    }
    if (ByteArray[0] > 0)
    {
        this->serialData.id = ByteArray[0];
        this->serialData.data1 = Data1;
        this->serialData.data2 = Data2;
    }
}

// RELAYS

void MicromationDevboardV3::relaySetup()
{
    pinMode(R1, OUTPUT);
    pinMode(R2, OUTPUT);
    pinMode(R3, OUTPUT);
    pinMode(R4, OUTPUT);
    relayLo(R1);
    relayLo(R2);
    relayLo(R3);
    relayLo(R4);

#ifdef IMIV3PRO
    pinMode(R5, OUTPUT);
    pinMode(R6, OUTPUT);
    pinMode(R7, OUTPUT);
    pinMode(R8, OUTPUT);
    pinMode(R9, OUTPUT);
    relayLo(R5);
    relayLo(R6);
    relayLo(R7);
    relayLo(R8);
    relayLo(R9);
#endif
}

void MicromationDevboardV3::relayHi(int relay)
{
    if (this->debug)
        this->serialdebug("PIN: " + String(relay) + " ON");

    digitalWrite(relay, HIGH);
}

void MicromationDevboardV3::relayLo(int relay)
{
    if (this->debug)
        this->serialdebug("PIN: " + String(relay) + " OFF");
    digitalWrite(relay, LOW);
}

bool MicromationDevboardV3::relayIsOn(int relay)
{
    return digitalRead(relay) == HIGH;
}

void MicromationDevboardV3::serialdebug(String msg)
{
    unsigned int strlen = this->serialdebugmsg.length();

    if (strlen > 1000)
    {
        this->serialdebugmsg = "";
    }
    this->serialdebugmsg.concat(msg);
    this->serialdebugmsg.concat("\n");

    unsigned long debugcurrentMillis = millis();

    if (debugcurrentMillis - this->debugpreviousMillis >= 3000)
    {
        this->debugpreviousMillis = debugcurrentMillis;
        Serial.println(this->serialdebugmsg);
    }
}