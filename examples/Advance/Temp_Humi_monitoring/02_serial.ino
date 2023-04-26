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

      if (TEMP <= setVar1 && setVar1 > 0)
      {
        iMi3.relayHi(R1);
        R1info.stateText = "ON";
        R1info.cssClass = "alert";
      }
      else
      {
        iMi3.relayLo(R1);
        R1info.stateText = "OFF";
        R1info.cssClass = "normal";
      }

      if (TEMP >= setVar2 && setVar2 > 0)
      {
        iMi3.relayHi(R2);
        R2info.stateText = "ON";
        R2info.cssClass = "alert";
      }
      else
      {
        iMi3.relayLo(R2);
        R2info.stateText = "OFF";
        R2info.cssClass = "normal";
      }
      if (HUMI <= setVar3 && setVar3 > 0)
      {
        iMi3.relayHi(R3);
        R3info.stateText = "ON";
        R3info.cssClass = "alert";
      }
      else
      {
        iMi3.relayLo(R3);
        R3info.stateText = "OFF";
        R3info.cssClass = "normal";
      }
      if (HUMI >= setVar4 && setVar4 > 0)
      {
        iMi3.relayHi(R4);
        R4info.stateText = "ON";
        R4info.cssClass = "alert";
      }
      else
      {
        iMi3.relayLo(R4);
        R4info.stateText = "OFF";
        R4info.cssClass = "normal";
      }
    }
    else
    {
      TEMP = 0;
      HUMI = 0;
    }
  }
}
