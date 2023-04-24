// ██╗███╗   ███╗██╗ ██████╗ ██████╗ ███╗   ██╗    ███████╗██╗   ██╗███████╗████████╗███████╗███╗   ███╗
// ██║████╗ ████║██║██╔════╝██╔═══██╗████╗  ██║    ██╔════╝╚██╗ ██╔╝██╔════╝╚══██╔══╝██╔════╝████╗ ████║
// ██║██╔████╔██║██║██║     ██║   ██║██╔██╗ ██║    ███████╗ ╚████╔╝ ███████╗   ██║   █████╗  ██╔████╔██║
// ██║██║╚██╔╝██║██║██║     ██║   ██║██║╚██╗██║    ╚════██║  ╚██╔╝  ╚════██║   ██║   ██╔══╝  ██║╚██╔╝██║
// ██║██║ ╚═╝ ██║██║╚██████╗╚██████╔╝██║ ╚████║    ███████║   ██║   ███████║   ██║   ███████╗██║ ╚═╝ ██║
// ╚═╝╚═╝     ╚═╝╚═╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝    ╚══════╝   ╚═╝   ╚══════╝   ╚═╝   ╚══════╝╚═╝     ╚═╝

// กำหนดหมายเลขพินที่ตรงกับฮาร์ดแวร์ของบอร์ด
#define RXD2 16 // กำหนดหมายเลข GPIO 16 สำหรับขา Rx
#define TXD2 17 // กำหนดหมายเลข GPIO 17 สำหรับขา Tx
#define SW 4    // Custom swithch

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

#ifndef MicromationDevboardV3_h
#define MicromationDevboardV3_h

#define OLED_PAGES 3 // จำนวนหน้าที่จะแสดงผลบนจอ OLED สงวนหน้าสุดท้ายสำหรับการแสดงผลของระบบ
#define PAGE_1 1
#define PAGE_2 2
#define PAGE_3 3

#define SERIAL_INPUTS 3 // จำนวน Serial Input ที่ใช้งานได้
#define SERIAL_INPUT1 1
#define SERIAL_INPUT2 2
#define SERIAL_INPUT3 3

#define CUSTOM_FIELDS 5 // จำนวน Custom Field ที่ใช้งานได้

#include <Arduino.h>
#include <U8g2lib.h>

class MicromationDevboardV3
{
public:
    // MicromationDevboardV3(String device_name, String device_password, int mode, int oledPage, bool debug);
    struct iMi3Config
    {
        String device_name;
        String device_password;
        String wifi_ssid;
        String wifi_password;
        String custom_field_label[CUSTOM_FIELDS];
        int mode;
        int oledPages;
        bool debug;
    };

    MicromationDevboardV3(struct iMi3Config config);
    void doSetup(void);
    void doLoop(void);
    int getState(void);
    // int getMode(void);
    String getName(void);
    String getIP(void);
    String getData(const char *key);
    String getData(const char *key, String old);
    int getDataInt(const char *key);
    int getDataInt(const char *key, int old);

    static void handleRoot(void);
    String html = "";
    struct iMi3OledMessage
    {
        int tp;
        String msg1;
        String msg2;
        String msg3;
        String msg4;
        int msg5;
    };

    struct iMi3SerialData
    {
        String name;
        String id;
        int data1;
        int data2;
    };
    struct iMi3SerialData serialData;
    void printScreen(const uint8_t page, struct MicromationDevboardV3::iMi3OledMessage message);
    int switchState = 0;
    int switchPressTime;
    bool switchIsPressed = false;
    bool switchIsReleased = false;
    void beep(int note, int duration);
    void noBeep();
    void relayHi(int relay);
    void relayLo(int relay);
    bool relayIsOn(int relay);
    void SerialProcessData(byte buffQuery[8]);
    void updateBaudRate(int baudrate);

private:
    struct MicromationDevboardV3::iMi3OledMessage message[3];
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
    
    
    void relaySetup(void);
    String getStorage(const char *key);
    int getStorageInt(const char *key);
    void setStorage(const char *key, String value);
    void setStorageInt(const char *key, int value);
    static void handleNotFound(void);
    static void handleConfig(void);
    static void saveConfig(void);
    uint8_t draw_state = 1;
    String device_name;
    String device_password; // รหัสผ่านของอุปกรณ์สำหรับเข้าสู่โหมด AP
    String device_ip;
    String wifi_ssid = "";
    String wifi_password = "";
    String custom_field_label[CUSTOM_FIELDS] = {"Max val1", "Max val2", "Max val3", "Max val4", "Max val5"};
    bool debug = false;
    String log_string = "";
    int state;
    int mode = 1;
    int oledPages = 1;
    int btnState;
    int CountPress;
    String systemhtml = "";

    unsigned long webpreviousMillis = 0;
    unsigned long devinterval = 30000;
    unsigned long serpreviousMillis = 0;
    unsigned long oledpreviousMillis = 0;   // will store last time LED was updated
    unsigned long serialpreviousMillis = 0; // will store last time LED was updated
    unsigned long debugpreviousMillis = 0;  // will store last time LED was updated

    void serialdebug(String msg);
    String serialdebugmsg = "";
};
#endif