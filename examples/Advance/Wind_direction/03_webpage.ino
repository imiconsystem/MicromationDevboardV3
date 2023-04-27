
void webpageLoop()
{
  char html[4150];
  int dir;
  if (WINDIR > 0)
    dir = WINDIR;
  else
    dir = 0;
  String relay1Text, relay2Text, relay3Text, relay4Text, setVar1Class, setVar2Class, Setvar3Class, setVar4Class, icon, webtitle;
  relay1Text = "OFF";
  relay2Text = "OFF";
  relay3Text = "OFF";
  relay4Text = "OFF";
  webtitle = "Wind direction monitoring";

  if (iMi3.relayIsOn(R1))
  {
    setVar1Class = "alert";
    relay1Text = "ON";
  }
  else
    setVar1Class = "normal";

  if (iMi3.relayIsOn(R2))
  {
    setVar2Class = "alert";
    relay2Text = "ON";
  }
  else
    setVar2Class = "normal";

  if (iMi3.relayIsOn(R3))
  {
    Setvar3Class = "alert";
    relay3Text = "ON";
  }
  else
    Setvar3Class = "normal";

  if (iMi3.relayIsOn(R4))
  {
    setVar4Class = "alert";
    relay4Text = "ON";
  }
  else
    setVar4Class = "normal";

  icon = alarmlev.icon;

  snprintf(html, 4150,
           "<!DOCTYPE html><html><head> <meta charset=\"UTF-8\"/> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">  <meta http-equiv='refresh' content='3'/><style>.max-val span, h3{text-align: center}#pm25-num sup, body{font-family: Arial, Helvetica, sans-serif}html{width: 100vw; height: 100vh}body{margin: 0 30px; background: #001213; background: linear-gradient(126deg, #001213 0, #005157 100%%); color: #eee;}.val{font-size: 30px; display: block; margin-bottom: 10px}.pm-other .val{text-align: right}.max-val span{display: inline-block; background-color: #ffffff21; border-radius: 5px; padding: 5px; font-size: 10px; margin: 3px;}.alert{background-color: #00be8e !important}h1{font-size: 16px; margin: 0 0 3px; padding: 0; color: #89ffae}h3{font-size: 22px; margin-top: 0}*{box-sizing: border-box}.flex-container{display: flex; flex-direction: row; text-align: left}.flex-item-left{padding: 10px 20px 10px 0; flex: 50%%; border-right: 1px solid #ffffff52; position: relative; display: flex; align-items: center; justify-content: center; column-gap: 30px; row-gap: 30px;}.flex-item-right{padding: 10px 0 10px 20px; flex: 50%%;}.flex-item-right sup{font-size: 15px}#pm25-num{font-size: 100px; line-height: 100px}#pm25-num sup{font-size: 20px}.emoticon{font-size: 7rem; line-height: 9rem; filter: grayscale(0); right: 0; border: solid; border-radius: 100px; width: 180px; height: 180px; display: flex; justify-content: center; transform: rotate(%ddeg); margin-top: 0px; background-color: rgba(0, 0, 0, 0.5); transition-duration: 0.5s; transition-delay: 1s;}@media (max-width:660px){.emoticon{top: 0}.flex-container{flex-direction: column}.flex-item-left{padding: 10px 0; border-right: 0; border-bottom: 1px solid #ffffff52; justify-content: center; flex-wrap: wrap;}.flex-item-right{padding: 10px 0}}.container{max-width: 830px; margin: auto;}</style></head><body> <div class=\"container\"> <h3 style=\"display: flex;align-items: center;column-gap: 10px;justify-content: center;\"><span style='font-size:50px;filter: grayscale(1);'>&#10228;</span> %s</h3> <div class=\"flex-container\"> <div class=\"flex-item-left\"> <div style=\"text-align: center;\"> <p style=\"font-size:2rem; margin:0px 0;padding:0;\">N</p><div style=\"display:flex;flex-direction: row;flex-wrap: nowrap;align-items: center;column-gap: 5px;\"> <p style=\"font-size:2rem; margin:0px 0;padding:0;\">W</p><div class=\"emoticon\">↑</div><p style=\"font-size:2rem; margin:0px 0;padding:0;\">E</p></div><p style=\"font-size:2rem; margin:0px 0;padding:0;\">S</p></div><div class=\"pm25\" style=\"display: flex;flex-direction: row;justify-content: space-between;flex-wrap: nowrap;\"> <div class=\"\"> <h1>Acquisition value</h1> <div id=\"pm25-num\" class=\"val\">%d°</div><h1>Direction</h1> <span class=\"val\">%s</span> </div></div></div><div class=\"flex-item-right\"> <div class=\"\"> <div class=\"\" style=\"display: flex;flex-direction: column;\"> <div style=\"display: flex;flex-wrap: nowrap;justify-content: space-between;\"> <div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY1</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">North ± %d°</span> </div></div><div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY2</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">East ± %d°</span> </div></div></div></div></div><div class=\"\"> <div class=\"\" style=\"display: flex;flex-direction: column;\"> <div style=\"display: flex;flex-wrap: nowrap;justify-content: space-between;\"> <div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY3</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">South ± %d°</span> </div></div><div class=\"relay-box\" style=\"text-align:center; padding-top:30px;\"> <div> <h1>RELAY4</h1> <span class=\"val\">%s</span> </div><div class=\"max-val\"> <span class=\"%s\">West ± %d°</span> </div></div></div></div></div></div></div><p style=\"text-align: center;\"><b>Domain:</b> %s.local</p></div></body></html>",

           dir, webtitle.c_str(), WINDIR, alarmlev.word.c_str(), relay1Text.c_str(), setVar1Class.c_str(), setVar1, relay2Text.c_str(), setVar2Class.c_str(), setVar2, relay3Text.c_str(), Setvar3Class.c_str(), setVar3, relay4Text.c_str(), setVar4Class.c_str(), setVar4, iMi3.getData("device_name").c_str());

  iMi3.html = html;
}