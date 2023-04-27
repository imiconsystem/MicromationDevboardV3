const int numBytes = 8;
byte req[8] = {0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC5, 0xE9}; // RS FX wind direction
byte buffQuery[numBytes];

void serailSetup()
{
  iMi3.updateBaudRate(4800);
  memcpy(buffQuery, req, numBytes);
}

float Data;

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

    Data = iMi3.serialData.data2;

    if (Data >= 0)
    {
      WINDIR = Data;

      Serial.print("DIR =  ");
      Serial.println(WINDIR);

      if (WINDIR >= 0 && WINDIR < 45)
        alarmlev = {"N", ""};
      else if (WINDIR >= 45 && WINDIR < 90)
        alarmlev = {"NE", ""};
      else if (WINDIR >= 90 && WINDIR < 135)
        alarmlev = {"E", ""};
      else if (WINDIR >= 135 && WINDIR < 180)
        alarmlev = {"SE", ""};
      else if (WINDIR >= 180 && WINDIR < 225)
        alarmlev = {"S", ""};
      else if (WINDIR >= 225 && WINDIR < 270)
        alarmlev = {"SW", ""};
      else if (WINDIR >= 270 && WINDIR < 315)
        alarmlev = {"W", ""};
      else if (WINDIR >= 315 && WINDIR < 360)
        alarmlev = {"NW", ""};
    }
  }
}
