

void relayLoop(void) {

if (RANGE > 0 && RANGE > setVar1 && RANGE <= setVar2  && setVar1 > 0) {
    iMi3.relayHi(R1);
    R1info.stateText = "ON";
    R1info.cssClass = "alert";
  } else {
    iMi3.relayLo(R1);
    R1info.stateText = "OFF";
    R1info.cssClass = "normal";
  }

  if (RANGE > setVar2 && RANGE <= setVar3  && setVar2 > 0) {
    iMi3.relayHi(R2);
    R2info.stateText = "ON";
    R2info.cssClass = "alert";
  } else {
    iMi3.relayLo(R2);
    R2info.stateText = "OFF";
    R2info.cssClass = "normal";
  }
  if (RANGE > setVar3 && RANGE <= setVar4  && setVar3 > 0) {
    iMi3.relayHi(R3);
    R3info.stateText = "ON";
    R3info.cssClass = "alert";
  } else {
    iMi3.relayLo(R3);
    R3info.stateText = "OFF";
    R3info.cssClass = "normal";
  }
  if (RANGE > setVar4 && setVar4 > 0) {
    iMi3.relayHi(R4);
    R4info.stateText = "ON";
    R4info.cssClass = "alert";
  } else {
    iMi3.relayLo(R4);
    R4info.stateText = "OFF";
    R4info.cssClass = "normal";
  }
}
