
#include <iMi3Device.h>

iMi3Device::iMi3DeviceConfig config = {
  "iMi3Device",  // device name
  "dddddddd",    // device password
  "i2",          // WiFi SSID
  "",  // WiFi password
  "2",           // mode 1 = AP, 2 = STA
  true           // debug mode
};

iMi3Device iMi3(config);

void setup() {
  Serial.begin(115200);
  iMi3.doSetup();

}

void loop() {
  iMi3.doLoop();
  Serial.print("Mode: ");
  Serial.println(iMi3.getMode());
  Serial.print("State: ");
  Serial.println(iMi3.getState());
}
