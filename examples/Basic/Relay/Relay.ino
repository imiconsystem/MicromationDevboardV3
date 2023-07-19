#include <MicromationDevboardV3.h>

MicromationDevboardV3::iMi3Config config = {
    "Mi3Dev",                                            // device name
    "dddddddd",                                              // device password
    "i2",                                                    // WiFi SSID
    "im25092499",                                            // WiFi password
    {"Custom1", "Custom2", "Custom3", "Custom4", "Custom5","Custom6", "Custom7", "Custom8", "Custom9", "Custom10"}, // Custom field labels
    2,                                                       // mode 1 = AP, 2 = STA
    4,                                                       // relayChanels 4 OR 9 
    2,                                                       // OLED pages
    true                                                     // debug mode
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
        relayLoop();
    }
}

void relayLoop()
{
    float WINSPD = random(1, 300);

    int maxSpd1, maxSpd2, maxSpd3, maxSpd4;
    maxSpd1 = 10;
    maxSpd2 = 20;
    maxSpd3 = 100;
    maxSpd4 = 200;

    if (WINSPD > maxSpd1)
    {
        iMi3.relayHi(R1);
    }
    else
        iMi3.relayLo(R1);

    if (WINSPD > maxSpd2)
    {
        iMi3.relayHi(R2);
    }
    else
        iMi3.relayLo(R2);

    if (WINSPD > maxSpd3)
    {
        iMi3.relayHi(R3);
    }
    else
        iMi3.relayLo(R3);

    if (WINSPD > maxSpd4)
    {
        iMi3.relayHi(R4);
    }
    else
        iMi3.relayLo(R4);
}