const int numBytes = 8;
byte req[8] = { 0x02, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x38 };  // RS FS wind speed
byte buffQuery[numBytes];

void serailSetup() {
  iMi3.updateBaudRate(9600);
  memcpy(buffQuery, req, numBytes);
}

float Data1, knot;

unsigned long previousMillis = 0;
unsigned long interval = 5000;
uint8_t i = 1;
void serailLoop() {
  unsigned long currentMillis = millis();
  while (currentMillis - previousMillis >= interval)  // None blocking delay
  {
    previousMillis = currentMillis;
    iMi3.SerialProcessData(buffQuery);

    maxSpd1 = iMi3.getDataInt("custom1");
    maxSpd2 = iMi3.getDataInt("custom2");
    maxSpd3 = iMi3.getDataInt("custom3");
    maxSpd4 = iMi3.getDataInt("custom4");
    maxSpd5 = iMi3.getDataInt("custom5");


    Data1 = iMi3.serialData.data1 * 0.1;

    if (iMi3.serialData.data1 > 0) {
      WINSPD = Data1 * 3.6;   //metre per second to kilometre per hour multiply the speed value by 3.6
      knot = Data1 / 30.867;  //for an approximate result, divide the speed value by 30.867

      if (knot <= 1) alarmlev = { "Calm", "&#128512;" };
      else if (knot > 1 && knot <= 3) alarmlev = { "Light Air", "&#128512;" };
      else if (knot >= 4 && knot <= 6) alarmlev = { "Light Breeze", "&#128512;" };
      else if (knot >= 7 && knot <= 10) alarmlev = { "Gentle Breeze", "&#128512;" };
      else if (knot >= 11 && knot <= 16) alarmlev = { "Moderate Breeze", "&#128524;" };
      else if (knot >= 17 && knot <= 21) alarmlev = { "Fresh Breeze", "&#128524;" };
      else if (knot >= 22 && knot <= 27) alarmlev = { "Strong Breeze", "&#128528;" };
      else if (knot >= 28 && knot <= 33) alarmlev = { "Near Gale", "&#128528;" };
      else if (knot >= 34 && knot <= 40) alarmlev = { "Gale", "&#128531;" };
      else if (knot >= 41 && knot <= 47) alarmlev = { "Severe Gale", "&#128534;" };
      else if (knot >= 48 && knot <= 55) alarmlev = { "Storm", "&#128565;" };
      else if (knot >= 56 && knot <= 63) alarmlev = { "Violent Storm", "&#128561;" };
      else if (knot >= 64 && knot <= 71) alarmlev = { "Hurricane", "&#127756;" };
    } else {
      WINSPD = 0;
    }
  }

}
