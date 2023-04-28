const int numBytes = 8;
byte req[8] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; // BGT-WSD2 humi & temp

void serailSetup()
{
  iMi3.updateBaudRate(9600);
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
    iMi3.SerialProcessData(req);

    if (iMi3.serialData.data1 > 0 && iMi3.serialData.data2 > 0)
    {
      HUMI = iMi3.serialData.data1 * 0.1;
      TEMP = iMi3.serialData.data2 * 0.1;

      
    }
    else
    {
      TEMP = 0;
      HUMI = 0;
    }
  }
}
