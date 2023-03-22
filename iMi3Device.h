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

#define BUZZZER_PIN = 33; // piezo buzzer

// OUTPUTS RELAYS
#define R1 23
#define R2 19
#define R3 18
#define R4 32

#ifndef iMi3Device_h
#define iMi3Device_h

#define MODE_AP 1
#define MODE_STA 2

#include <Arduino.h>

class iMi3Device
{
public:
    iMi3Device(String device_name, String device_password, int mode, bool debug);
    void doSetup(String ssid, String password);
    void doLoop(void);
    int getState(void);
    int getMode(void);
    String getName(void);
    String getIP(void);

private:
    void setState(int state);
    void storageSetup(void);
    void resetbuttonSetup(void);
    void wifiapSetup(bool isAPSET);
    void webserverSetup(void);
    void webserverLoop(void);
    void clientLoop(void);
    static void handleRoot(void);
    static void handleNotFound(void);
    static void handleConfig(void);
    static void saveConfig(void);
    String device_name;
    String device_password; // รหัสผ่านของอุปกรณ์สำหรับเข้าสู่โหมด AP
    String device_ip;
    String host_ssid = "";
    String host_password = "";
    bool debug = false;
    int state;
    int mode;
    int btnState;
    int CountPress;
};
#endif