
int pressCount = 0;
int relays[4] = {R1,R2,R3,R4};

void switchLoop(void) {

  int press_time = iMi3.switchPressTime;

  if (press_time > 0 && press_time < 20) {
    Serial.println("Short press"); 
  } else if (press_time > 50 && press_time < 100) {
    Serial.println("Long press");
  }

  if (iMi3.switchIsPressed) {

    
    if(pressCount > 3) pressCount = 0;
    

    if(!iMi3.relayIsOn(relays[pressCount])) iMi3.relayHi(relays[pressCount]);
    else iMi3.relayLo(relays[pressCount]);

    pressCount ++;

    Serial.print("Switch is pressed: ");
    Serial.println(pressCount);
    beep();
  }
}
