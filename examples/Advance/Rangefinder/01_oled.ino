

void oledLoop(void) {
  float y = random(1, 500) / 100.0;
  String msg1, msg2;
  msg1.concat(y);

  MicromationDevboardV3::iMi3OledMessage myMsg[3];

  myMsg[0].tp = 2;
  myMsg[0].msg1 = "DIS";
  myMsg[0].msg2 = "cm";
  myMsg[0].msg3 = "";
  myMsg[0].msg4 = "";
  myMsg[0].msg5 = RANGE;
  iMi3.printScreen(PAGE_1, myMsg[0]);

}