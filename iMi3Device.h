// ██╗███╗   ███╗██╗ ██████╗ ██████╗ ███╗   ██╗    ███████╗██╗   ██╗███████╗████████╗███████╗███╗   ███╗
// ██║████╗ ████║██║██╔════╝██╔═══██╗████╗  ██║    ██╔════╝╚██╗ ██╔╝██╔════╝╚══██╔══╝██╔════╝████╗ ████║
// ██║██╔████╔██║██║██║     ██║   ██║██╔██╗ ██║    ███████╗ ╚████╔╝ ███████╗   ██║   █████╗  ██╔████╔██║
// ██║██║╚██╔╝██║██║██║     ██║   ██║██║╚██╗██║    ╚════██║  ╚██╔╝  ╚════██║   ██║   ██╔══╝  ██║╚██╔╝██║
// ██║██║ ╚═╝ ██║██║╚██████╗╚██████╔╝██║ ╚████║    ███████║   ██║   ███████║   ██║   ███████╗██║ ╚═╝ ██║
// ╚═╝╚═╝     ╚═╝╚═╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝    ╚══════╝   ╚═╝   ╚══════╝   ╚═╝   ╚══════╝╚═╝     ╚═╝

// กำหนดหมายเลขพินที่ตรงกับฮาร์ดแวร์ของบอร์ด
#define RXD2 16 // กำหนดหมายเลข GPIO 16 สำหรับขา Rx
#define TXD2 17 // กำหนดหมายเลข GPIO 17 สำหรับขา Tx
#define SW 2    // Custom swithch

#define BUZZZER_PIN 5 // piezo buzzer

// OUTPUTS RELAYS
#define R1 23
#define R2 19
#define R3 18
#define R4 32
#ifdef IMIV3PRO
#define R5 25
#define R6 26
#define R7 14
#define R8 26
#define R8 27
#endif

#ifndef iMi3Device_h
#define iMi3Device_h

#define OLED_PAGES 4 // จำนวนหน้าที่จะแสดงผลบนจอ OLED สงวนหน้าสุดท้ายสำหรับการแสดงผลของระบบ
#define PAGE_1 1
#define PAGE_2 2
#define PAGE_3 3

#define SERIAL_INPUTS 3 // จำนวน Serial Input ที่ใช้งานได้
#define SERIAL_INPUT1 1
#define SERIAL_INPUT2 2
#define SERIAL_INPUT3 3

#include <Arduino.h>
#include <U8g2lib.h>

class iMi3Device
{
public:
    // iMi3Device(String device_name, String device_password, int mode, int oledPage, bool debug);
    struct iMi3DeviceConfig
    {
        String device_name;
        String device_password;
        String wifi_ssid;
        String wifi_password;
        String mode;
        bool debug;
    };

    iMi3Device(struct iMi3DeviceConfig config);
    void doSetup();
    void doLoop(void);
    String getState(void);
    String getMode(void);
    String getName(void);
    String getIP(void);
    String getData(const char *key);
    String getData(const char *key, String old);
    static void handleRoot(void);
    String html = "";
    struct iMi3OledMessage
    {
        int tp;
        String msg1;
        String msg2;
        String msg3;
        String msg4;
        String msg5;
    };
    struct iMi3SerialInput
    {
        String name;
        int buadrate;
        byte request_command[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    };

    struct iMi3SerialData
    {
        String name;
        String id;
        int data1;
        int data2;
    };
    struct iMi3SerialData serialData;
    void printScreen(const uint8_t page, struct iMi3Device::iMi3OledMessage message);
    void readSerial(const uint8_t index, struct iMi3Device::iMi3SerialInput serialInput);
    void printLog(const char *message);
    int switchState = 0;
    int switchPressTime;
    bool switchIsPressed = false;
    bool switchIsReleased = false;
    void beep(int note, int duration);
    void noBeep();
    void relayHi(int relay);
    void relayLo(int relay);
    bool relayIsOn(int relay);

private:
    struct iMi3Device::iMi3OledMessage message[3];
    struct iMi3Device::iMi3SerialInput serialInput[3];
    void setState(int state);
    void storageSetup(void);
    void switchSetup(void);
    void switchLoop(void);
    void wifiapSetup(bool isAPSET);
    void oledSetup(void);
    void webserverSetup(void);
    void webserverLoop(void);
    void clientLoop(void);
    void oledLoop(void);
    void oledDrawState();
    void oledDrawPage1(void);
    void oledDrawPage2(void);
    void oledDrawPage3(void);
    void oledDrawSysPage(void);
    void oledLogSetup(void);
    void serialSetup(void);
    void serialLoop(void);
    void SerialProcessData(byte buffQuery[8]);
    void relaySetup(void);
    String getStorage(const char *key);
    void setStorage(const char *key, String value);
    static void handleNotFound(void);
    static void handleConfig(void);
    static void saveConfig(void);
    uint8_t draw_state = 1;
    String device_name;
    String device_password; // รหัสผ่านของอุปกรณ์สำหรับเข้าสู่โหมด AP
    String device_ip;
    String wifi_ssid = "";
    String wifi_password = "";
    bool debug = false;
    String log_string = "";
    String state;
    String mode = "1";
    int oledPages = 1;
    int btnState;
    int CountPress;
    
    
};
#endif