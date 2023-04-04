#include "Arduino.h"
#include "iMi3Device.h"
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

iMi3Device::iMi3Device(struct iMi3DeviceConfig config)
{
    this->html = "This is blank html.";
    this->device_name = config.device_name;
    this->device_password = config.device_password;
    this->wifi_ssid = config.wifi_ssid;
    this->wifi_password = config.wifi_password;
    this->mode = config.mode;

    this->debug = config.debug;
    // this->state = 0; // 0 = run, 1 = setup
    this->state = "0";

    this->btnState = 0; // 0 = no press, 1 = press, 2 = long press
    this->switchPressTime = 0;

    Serial.println("iMi3Device constructor");
}

void iMi3Device::doSetup()
{
    this->oledSetup();
    this->serialSetup();
    this->relaySetup();
    this->switchSetup();
    this->storageSetup();
    this->beep(NOTE_E6, 100);
    if (this->getState() == "1") // SET
    {
        Serial.println("SET");
        if (this->debug)
        {
            Serial.println("getState() SET running config Form");
        }
        this->wifiapSetup(true); // AP for setup
    }
    else if (this->getState() == "2")
    { // RESET FACTORY
        if (this->debug)
        {
            Serial.println("state RESET");
        }
        // storageClear();
        ESP.restart();
    }
    else
    {
        // RUN
        if (this->debug)
        {
            Serial.println("state RUN");
        }

        if (this->mode == "2")
        {
            this->webserverSetup(); // ST mode
        }
        else
        {
            this->wifiapSetup(false); // AP for Run
        }
    }
}
void iMi3Device::doLoop(void)
{

    this->switchLoop();

    if (this->mode == "2")
    {
        this->serialLoop();
        this->webserverLoop();
        this->clientLoop();
        this->oledLoop();
    }
    else
    {
        clientLoop();
    }
}
void iMi3Device::setState(int state)
{
    this->state = state;
}
String iMi3Device::getState(void)
{
    return this->getData("device_state", this->state);
}
String iMi3Device::getMode(void)
{
    return this->mode;
}
String iMi3Device::getName(void)
{
    return this->getData("device_name", this->device_name);
}

// รับค่าจากปุ่ม และกำหนดค่า mode ให้กับอุปกรณ์
void iMi3Device::switchSetup()
{
    cswitch.setDebounceTime(50);
    cswitch.setCountMode(COUNT_FALLING);
}

void iMi3Device::switchLoop()
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
            this->state = 1; // SET
            this->setStorage("device_state", "1");
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
            Serial.println("The button is pressed");
    }

    if (cswitch.isReleased())
    {
        if (this->debug)
            Serial.println("The button is released");
    }
}

void iMi3Device::beep(int note, int duration)
{
    tone(BUZZZER_PIN, note, duration);
    delay(duration);
}
void iMi3Device::noBeep()
{
    noTone(BUZZZER_PIN);
}
// Storage
void iMi3Device::storageSetup()
{

    String _wifi_ssid, _wifi_password, _webpage_title, _device_name, _device_password, _device_state, _mode;

    preferences.begin(prekey, false);

    _wifi_ssid = preferences.getString("wifi_ssid", "");
    _wifi_password = preferences.getString("_wifi_password", "");

    if (_wifi_ssid == "" || _wifi_password == "")
    {
        preferences.putString("wifi_ssid", this->wifi_ssid);
        preferences.putString("wifi_password", this->wifi_password);
        if (this->debug)
        {
            Serial.println("No values saved for WIFssid or WiFipassword");
            Serial.println("WIFI Credentials Saved using default value.");
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
            Serial.println("No values saved for device_name or device_password");
            Serial.println("Device Credentials Saved using default value.");
        }
    }

    _webpage_title = preferences.getString("webpage_title", "");

    if (_webpage_title == "")
    {
        preferences.putString("webpage_title", "iMi3Device");
        if (this->debug)
        {
            Serial.println("No values saved for webpage_title");
            Serial.println("Webpage Title Saved using default value.");
        }
    }

    _device_state = preferences.getString("device_state", "");
    if (_device_state == "")
    {
        preferences.putString("device_state", "0");
        if (this->debug)
        {
            Serial.println("No values saved for device_state");
            Serial.println("Device State Saved using default value.");
        }
    }

    _mode = preferences.getString("mode", "");
    if (_mode == "")
    {
        preferences.putString("mode", "1");
        if (this->debug)
        {
            Serial.println("No values saved for mode");
            Serial.println("Mode Saved using default value.");
        }
    }

    preferences.end();
}
void iMi3Device::setStorage(const char *key, String value)
{
    preferences.begin(prekey, false);
    preferences.putString(key, value);
    preferences.end();
}
String iMi3Device::getStorage(const char *key)
{
    String value;
    preferences.begin(prekey, false);
    value = preferences.getString(key, "");
    preferences.end();
    return value;
}
String iMi3Device::getData(const char *key)
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
String iMi3Device::getData(const char *key, String old)
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
void iMi3Device::wifiapSetup(bool isAPSET)
{
    if (this->debug)
    {
        Serial.println("Configuring access point...");
        Serial.print("AP Name: ");
        Serial.println(this->getData("device_name", this->device_name));
        Serial.print("AP Password: ");
        Serial.println(this->device_password);
    }

    WiFi.softAP(this->getData("device_name", this->device_name).c_str(), this->getData("device_password", this->device_password).c_str());
    IPAddress myIP = WiFi.softAPIP();
    this->device_ip = myIP.toString();
    if (this->debug)
    {
        Serial.print("AP IP address: ");
        Serial.println(myIP);
    }
    if (isAPSET)
    {
        server.on("/", this->handleConfig);
        server.on("/saveConfig", this->saveConfig);
    }
    else
    {
        server.on("/", [&]()
                  { server.send(200, "text/html", this->html); });
        // server.on("/", this->handleRoot);
    }

    server.onNotFound(this->handleNotFound);

    server.begin();
    if (this->debug)
        Serial.println("AP mode started");
}
// WIFI STA
unsigned long webpreviousMillis = 0;
unsigned long interval = 30000;

void iMi3Device::webserverSetup()
{
    unsigned long webcurrentMillis = millis();
    if (this->debug)
    {
        Serial.println("WiFissid");
        Serial.println(this->wifi_ssid);
        Serial.println("WiFipassword");
        Serial.println(this->wifi_password);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(this->wifi_ssid.c_str(), this->wifi_password.c_str());

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED && (webcurrentMillis - webpreviousMillis >= interval))
    {
        // Serial.println(WiFi.status());
        delay(1000);
        webpreviousMillis = webcurrentMillis;
    }

    if (MDNS.begin(this->getData("device_name", this->device_name).c_str()))
    {
        if (this->debug)
        {
            Serial.println("MDNS responder started");
            Serial.println(this->getData("device_name", this->device_name));
        }
    }

    server.on("/", [&]()
              { server.send(200, "text/html", this->html); });
    // server.on("/", this->handleRoot);

    server.onNotFound(this->handleNotFound);

    server.begin();
    if (this->debug)
        Serial.println("HTTP server started");
}
unsigned long serpreviousMillis = 0;
void iMi3Device::webserverLoop()
{
    unsigned long sercurrentMillis = millis();

    // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
    while ((WiFi.status() != WL_CONNECTED) && (sercurrentMillis - serpreviousMillis >= interval))
    {
        // Serial.println(WiFi.status());
        if (this->debug)
            Serial.println("Reconnecting to WiFi...");

        this->log_string = "Reconnecting to WiFi...";

        WiFi.disconnect();
        WiFi.reconnect();
        delay(1000);
        serpreviousMillis = sercurrentMillis;
    }

    // this->log_string = WiFi.localIP().toString();

    if (WiFi.status() == WL_CONNECTED)
    {
        if (this->debug)
        {
            // Serial.print("Connected to ");
            // Serial.println(this->wifi_ssid);
            // Serial.print("IP address: ");
            // Serial.println(WiFi.localIP());
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
void iMi3Device::clientLoop()
{
    server.handleClient();
    delay(10); // allow the cpu to switch to other tasks
}
void iMi3Device::handleRoot()
{
    String message = "Hello World";
    server.send(200, "text/plain", message);
}
void iMi3Device::handleNotFound()
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
void iMi3Device::handleConfig()
{

    String form = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"icon\" href=\"data:,\">"
                  "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
                  ".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;"
                  "text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}"
                  ".button2 {background-color: #77878A;}</style></head><body>"
                  "<h1>ESP32 Access Point</h1>"
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
void iMi3Device::saveConfig()
{
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    if (ssid != "" && password != "")
    {
        preferences.begin(prekey, false);
        preferences.putString("WiFissid", ssid);
        preferences.putString("WiFipassword", password);
        preferences.end();
        server.send(200, "text/html", "Configuration saved. Please restart the device.");
    }
    else
    {
        server.send(200, "text/html", "Please fill in both fields.");
    }
}

String iMi3Device::getIP(void)
{
    return this->device_ip;
}

// OLED
void iMi3Device::oledSetup(void)
{
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFontMode(0); // enable transparent mode, which is faster
}

unsigned long oledpreviousMillis = 0; // will store last time LED was updated
void iMi3Device::oledLoop(void)
{
    // Serial.println("PAGES: " + String(this->draw_state));
    // Serial.println("OLED PAGES: " + String(this->oledPages));
    unsigned long oledcurrentMillis = millis();
    // Serial.println(oledcurrentMillis - oledpreviousMillis);

    if (oledcurrentMillis - oledpreviousMillis >= 5000)
    {
        oledpreviousMillis = oledcurrentMillis;

        u8g2.firstPage();
        do
        {
            oledDrawState();
        } while (u8g2.nextPage());

        this->draw_state++;
        if (this->draw_state > OLED_PAGES)
            this->draw_state = 1;

        // delay(3000);
    }
}

void iMi3Device::oledDrawState()
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
    case 4:
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

void iMi3Device::oledDrawPage1(void)
{
    Serial.print("MSG1: ");
    Serial.println(this->message[0].msg1);
    Serial.print("MSG2: ");
    Serial.println(this->message[0].msg2);
    Serial.print("MSG3: ");
    Serial.println(this->message[0].msg3);
    Serial.print("MSG4: ");
    Serial.println(this->message[0].msg4);
    Serial.print("MSG5: ");
    Serial.println(this->message[0].msg5);
    Serial.print("TP: ");
    Serial.println(this->message[0].tp);

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
        u8g2.setFont(u8g2_font_ncenB10_tr);
        u8g2.drawStr(0, 12, this->message[0].msg1.c_str());
        u8g2.setFont(u8g2_font_5x8_tf);
        u8g2.drawStr(0, 25, this->message[0].msg2.c_str());
        u8g2.drawStr(30, 25, this->message[0].msg3.c_str());
        u8g2.drawStr(40, 25, this->message[0].msg4.c_str());
    }
    else
    {
        u8g2.setFont(u8g2_font_5x8_tf);
        u8g2.drawStr(0, 6, this->message[0].msg1.c_str());
        u8g2.drawStr(0, 14, this->message[0].msg2.c_str());
        u8g2.drawStr(0, 22, this->message[0].msg3.c_str());
        u8g2.drawStr(0, 30, this->message[0].msg4.c_str());
    }
}

void iMi3Device::oledDrawPage2(void)
{
    u8g2.setFont(u8g2_font_5x8_tf);
    u8g2.drawStr(0, 6, this->message[1].msg1.c_str());
    u8g2.drawStr(0, 14, this->message[1].msg2.c_str());
    u8g2.drawStr(0, 22, this->message[1].msg3.c_str());
    u8g2.drawStr(0, 30, this->message[1].msg4.c_str());
}

void iMi3Device::oledDrawPage3(void)
{
    u8g2.setFont(u8g2_font_5x8_tf);
    u8g2.drawStr(0, 6, message[2].msg1.c_str());
    u8g2.drawStr(0, 14, message[2].msg2.c_str());
    u8g2.drawStr(0, 22, message[2].msg3.c_str());
    u8g2.drawStr(0, 30, message[2].msg4.c_str());
}

void iMi3Device::oledDrawSysPage(void)
{
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 20, "System Page");
}

void iMi3Device::printScreen(const uint8_t page, struct iMi3Device::iMi3OledMessage message)
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

void iMi3Device::readSerial(const uint8_t index, struct iMi3Device::iMi3SerialInput serialInput)
{

    if (index <= SERIAL_INPUTS)
    {
        this->serialData.name = serialInput.name;
        this->serialInput[index].name = serialInput.name;
        this->serialInput[index].buadrate = serialInput.buadrate;
        for (int i = 0; i < 8; i++)
        {
            this->serialInput[index].request_command[i] = serialInput.request_command[i];
        }
    }
}

void iMi3Device::serialSetup()
{
    Serial2.begin(9600); //
}
unsigned long serialpreviousMillis = 0; // will store last time LED was updated
void iMi3Device::serialLoop()
{
    int baudrate = 9600;
    int numBytes = 8;
    byte buffQuery[8];
    for (int i = 1; i <= SERIAL_INPUTS; i++)
    {

        unsigned long serialcurrentMillis = millis();
        // Serial.println(serialcurrentMillis - oledpreviousMillis);

        if (serialcurrentMillis - serialpreviousMillis >= 1000)
        {
            serialpreviousMillis = serialcurrentMillis;
            if (this->serialInput[i].buadrate > 0)
            {
                baudrate = this->serialInput[i].buadrate;
            }
            Serial2.updateBaudRate(baudrate);
            memcpy(buffQuery, this->serialInput[i].request_command, numBytes);

            this->SerialProcessData(buffQuery);
        }
    }
}

void iMi3Device::SerialProcessData(byte buffQuery[8])
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
        // Serial.print(ByteArray[a]);
        // Serial.print(" ");
        a++;
    }

    ByteData[0] = ByteArray[3] * 256 + ByteArray[4];
    ByteData[1] = ByteArray[5] * 256 + ByteArray[6];

    float Data1, Data2;
    Data1 = ByteData[0];
    Data2 = ByteData[1];

    // Serial.print(ByteArray[0]);
    // Serial.print(" ");
    // Serial.print(Data1);
    // Serial.print(" ");
    // Serial.print(Data2);
    // Serial.println(" ");

    this->serialData.id = ByteArray[0];
    this->serialData.data1 = Data1;
    this->serialData.data2 = Data2;
}

// RELAYS

void iMi3Device::relaySetup()
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

void iMi3Device::relayHi(int relay)
{

    digitalWrite(relay, HIGH);
}

void iMi3Device::relayLo(int relay)
{
    digitalWrite(relay, LOW);
}

bool iMi3Device::relayIsOn(int relay)
{
    return digitalRead(relay) == HIGH;
}