const int numBytes = 8;
byte modbusQuery1[numBytes] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B}; // BGT-WSD2 humi & temp
byte modbusQuery2[numBytes] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x38}; // RS FS wind speed
byte modbusQuery3[numBytes] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xE9}; // RS FX wind direction
byte buffQuery[numBytes];
void serailSetup()
{
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

    switch (i)
    {
    case 1:
      iMi3.updateBaudRate(9600);
      memcpy(buffQuery, modbusQuery1, numBytes);
      break;
    case 2:
      iMi3.updateBaudRate(9600);
      memcpy(buffQuery, modbusQuery2, numBytes);
      break;
    case 3:
      iMi3.updateBaudRate(4800);
      memcpy(buffQuery, modbusQuery3, numBytes);
      break;
    default:
      break;
    }
    iMi3.SerialProcessData(buffQuery);

    if (iMi3.serialData.id > 0)
    {
      if (iMi3.serialData.id == 1)
      {
        TEMP = round(iMi3.serialData.data2 * 0.1);
        HUMI = round(iMi3.serialData.data1 * 0.1);
      }
      else if (iMi3.serialData.id == 2)
      {
        WINSPD = round(iMi3.serialData.data1 * 0.1) * 3.6; // metre per second to kilometre per hour multiply the speed value by 3.6
      }
      else if (iMi3.serialData.id == 3)
      {
        WINDIR = iMi3.serialData.data2;
      }
    }

    i++;
    if (i > 3)
      i = 1;
  }
}
