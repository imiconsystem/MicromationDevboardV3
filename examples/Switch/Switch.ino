#include <MicromationDevboardV3.h>

MicromationDevboardV3::iMi3Config config = {
    "Mi3Dev",                                            // device name
    "dddddddd",                                              // device password
    "i2",                                                    // WiFi SSID
    "im25092499",                                            // WiFi password
    {"Custom1", "Custom2", "Custom3", "Custom4", "Custom5"}, // Custom field labels
    2,                                                       // mode 1 = AP, 2 = STA
    2,                                                       // OLED pages
    true                                                     // debug mode
};

MicromationDevboardV3 iMi3(config);

int pressCount = 0;

void setup()
{
    Serial.begin(115200);
    iMi3.doSetup(); // Device setup (must be called in setup)
}

void loop()
{
    iMi3.doLoop(); // Device loop (must be called in loop)
    switchLoop();
}

void switchLoop(void)
{

    int press_time = iMi3.switchPressTime;

    if (press_time > 0 && press_time < 20)
    {
        Serial.println("Short press");
    }
    else if (press_time > 50 && press_time < 100)
    {
        Serial.println("Long press");
    }

    if (iMi3.switchIsPressed)
    {

        pressCount++;

        Serial.print("Switch is pressed: ");
        Serial.println(pressCount);
    }
}