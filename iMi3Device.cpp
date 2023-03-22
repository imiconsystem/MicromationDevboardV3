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

WebServer server(80);
String authFailResponse = "Authentication Failed";

Preferences preferences;
const char *prekey = "APcredentials";

iMi3Device::iMi3Device(String device_name, String device_password, int mode, bool debug)
{
    this->device_name = device_name;
    this->device_password = device_password;
    if (mode)
    {
        this->mode = mode;
    }
    else
    {
        this->mode = 1;
    }
    this->debug = debug;
    this->state = 0; // 0 = run, 1 = setup, 2 = reset

    this->btnState = 0;
    this->CountPress = 0;
}
void iMi3Device::doSetup(String ssid, String password)
{
    if (ssid != "" && password != "")
    {
        this->host_ssid = ssid;
        this->host_password = password;
    }
    else
    {
        this->host_ssid = this->getStorage("WiFissid");
        this->host_password = this->getStorage("WiFipassword");
    }

    this->resetbuttonSetup();

    if (this->state == 1) // SET
    {
        Serial.println("SET");
        if (this->debug)
        {
            Serial.println("state SET running config Form");
        }
        this->wifiapSetup(true); // AP for setup
    }
    else if (this->state == 2)
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

        if (this->mode == 2)
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
    if (this->mode == 2)
    {

        this->webserverLoop();
        this->clientLoop();
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
int iMi3Device::getState(void)
{
    return this->state;
}
int iMi3Device::getMode(void)
{
    return this->mode;
}
String iMi3Device::getName(void)
{
    return this->device_name;
}

// รับค่าจากปุ่ม และกำหนดค่า mode ให้กับอุปกรณ์
void iMi3Device::resetbuttonSetup()
{
    ezButton button(SW);

    button.setDebounceTime(50);

    for (int i = 1; i <= 6; i++)
    {
        button.loop();
        if (button.getState() == 1)
            this->CountPress++;
        delay(1000);
    }

    if (this->CountPress == 6)
    {
        this->state = 2; // RESET
    }
    else if (this->CountPress > 0)
    {
        this->state = 1; // SET
    }
}

// Storage
void iMi3Device::storageSetup()
{

    String WIssid, WIpassword;

    preferences.begin(prekey, false);

    WIssid = preferences.getString("WiFissid", "");
    WIpassword = preferences.getString("WiFipassword", "");

    if (WIssid == "" || WIpassword == "")
    {
        preferences.putString("WiFissid", this->host_ssid);
        preferences.putString("WiFipassword", this->host_password);
        if (this->debug)
        {
            Serial.println("No values saved for WIFssid or WiFipassword");
            Serial.println("WIFI Credentials Saved using default value.");
        }
    }

    preferences.end();
}
String iMi3Device::getStorage(const char *key)
{
    String value;
    preferences.begin(prekey, false);
    value = preferences.getString(key);
    preferences.end();
    return value;
}
// WIFI AP
void iMi3Device::wifiapSetup(bool isAPSET)
{
    if (this->debug)
    {
        Serial.println("Configuring access point...");
        Serial.print("AP Name: ");
        Serial.println(this->device_name);
        Serial.print("AP Password: ");
        Serial.println(this->device_password);
    }

    WiFi.softAP(this->device_name, this->device_password);
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
        server.on("/", this->handleRoot);
    }

    server.onNotFound(this->handleNotFound);

    server.begin();
    if (this->debug)
        Serial.println("AP mode started");
}
// WIFI STA
unsigned long previousMillis = 0;
unsigned long interval = 30000;

void iMi3Device::webserverSetup()
{
    unsigned long currentMillis = millis();
    if (this->debug)
    {
        Serial.println("WiFissid");
        Serial.println(this->host_ssid);
        Serial.println("WiFipassword");
        Serial.println(this->host_password);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(this->host_ssid.c_str(), this->host_password.c_str());

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED && (currentMillis - previousMillis >= interval))
    {
        // Serial.println(WiFi.status());
        delay(1000);
        previousMillis = currentMillis;
    }

    if (MDNS.begin(this->device_name.c_str()))
    {
        if (this->debug)
        {
            Serial.println("MDNS responder started");
            Serial.println(this->device_name);
        }
    }

    server.on("/", this->handleRoot);

    server.onNotFound(this->handleNotFound);

    server.begin();
    if (this->debug)
        Serial.println("HTTP server started");
}
void iMi3Device::webserverLoop()
{
    unsigned long currentMillis = millis();
    // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
    while ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval))
    {
        // Serial.println(WiFi.status());
        if (this->debug)
            Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        WiFi.reconnect();
        delay(1000);
        previousMillis = currentMillis;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        if (this->debug)
        {
            Serial.print("Connected to ");
            Serial.println(this->host_ssid);
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
        }
    }
}
void iMi3Device::clientLoop()
{
    server.handleClient();
    delay(10); // allow the cpu to switch to other tasks
}
void iMi3Device::handleRoot()
{
    String message = "Hello World";
    server.send(200, "text/plain", this->html);
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

void iMi3Device::setHTML(char html[])
{
    this->html = html;
}

String iMi3Device::getIP(void)
{
    return this->device_ip;
}