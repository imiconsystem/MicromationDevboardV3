

void relayLoop(void) {

  if (WINDIR >= (360 - setVar1) && WINDIR >= (0 + setVar1)) iMi3.relayHi(R1);  // North wind
  else iMi3.relayLo(R1);

  if (WINDIR >= (90 - setVar2) && WINDIR <= (90 + setVar2)) iMi3.relayHi(R2);  // East wind
  else iMi3.relayLo(R2);

  if (WINDIR >= (180 - setVar3) && WINDIR <= (180 + setVar3)) iMi3.relayHi(R3);  // South wind
  else iMi3.relayLo(R3);

  if (WINDIR >= (270 - setVar4) && WINDIR <= (270 + setVar4)) iMi3.relayHi(R4);  // West wind
  else iMi3.relayLo(R4);
}
