

void relayLoop(void) {

  if (WINSPD > maxSpd1 && maxSpd1 > 0) {
    iMi3.relayHi(R1);
   
  } else {
    iMi3.relayLo(R1);
    
  }

  if (WINSPD > maxSpd2 && maxSpd2 > 0) {
    iMi3.relayHi(R2);
    
  } else {
    iMi3.relayLo(R2);
    
  }
  if (WINSPD > maxSpd3 && maxSpd3 > 0) {
    iMi3.relayHi(R3);
    
  } else {
    iMi3.relayLo(R3);
    
  }
  if (WINSPD > maxSpd4 && maxSpd4 > 0) {

    iMi3.relayHi(R4);
    
  } else {
    iMi3.relayLo(R4);
    
  }
}
