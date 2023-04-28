const int numBytes = 8;
byte req[8] = {0x04, 0x03, 0x01, 0x00, 0x00, 0x01, 0x85, 0xA3}; // Ultrasonic DYP A01A
byte buffQuery[numBytes];

void serailSetup()
{
  iMi3.updateBaudRate(9600);
  memcpy(buffQuery, req, numBytes);
}

unsigned long previousMillis = 0;
unsigned long interval = 5000;
uint8_t i = 1;
void serailLoop()
{
  unsigned long currentMillis = millis();
  while (currentMillis - previousMillis >= interval) // None blocking delay
  {
    previousMillis = currentMillis;
    iMi3.SerialProcessData(buffQuery);

    RANGE = (iMi3.serialData.data1 * 0.1)-setVar0;

    Serial.println(iMi3.serialData.data1);
    Serial.println(iMi3.serialData.data2);
    Serial.println(setVar0);
    Serial.println(RANGE);

  }
}
