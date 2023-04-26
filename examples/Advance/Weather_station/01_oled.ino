

void oledLoop(void) {
  float y = random(1, 500) / 100.0;
  String msg1, msg2;
  msg1.concat(y);

  MicromationDevboardV3::iMi3OledMessage myMsg[4];

  myMsg[0].tp = 2;
  myMsg[0].msg1 = "TEMP";
  myMsg[0].msg2 = "C";
  myMsg[0].msg3 = "";
  myMsg[0].msg4 = "";
  myMsg[0].msg5 = round(TEMP);
  iMi3.printScreen(PAGE_1, myMsg[0]);

  myMsg[1].tp = 2;
  myMsg[1].msg1 = "HUMI";
  myMsg[1].msg2 = "%";
  myMsg[1].msg3 = "";
  myMsg[1].msg4 = "";
  myMsg[1].msg5 = round(HUMI);
  iMi3.printScreen(PAGE_2, myMsg[1]);

  myMsg[2].tp = 2;
  myMsg[2].msg1 = "SPD";
  myMsg[2].msg2 = "km/h";
  myMsg[2].msg3 = "";
  myMsg[2].msg4 = "";
  myMsg[2].msg5 = round(WINSPD);
  iMi3.printScreen(PAGE_3, myMsg[2]);

  myMsg[3].tp = 2;
  myMsg[3].msg1 = "DIR";
  myMsg[3].msg2 = "deg";
  myMsg[3].msg3 = "";
  myMsg[3].msg4 = "";
  myMsg[3].msg5 = round(WINDIR);
  iMi3.printScreen(PAGE_4, myMsg[3]);

}