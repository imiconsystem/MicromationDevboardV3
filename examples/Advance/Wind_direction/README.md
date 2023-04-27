# Ready to use wind direction indicator with relays controller

ระบบวัดทิศทางลม เพื่อการแจ้งเตือน หรือควบคุมสั่งการอัตโนมัติ พร้อม Web Monitoring & WiFi management สำหรับ New Micromation Dev Board V.2 Lite with ESP32 ใช้ในการแสดงผลค่าต่างที่วัดได้ และการตั้งค่าต่างๆ ผ่านเว็บบราวเซอร์

![wind direction indicator](https://github.com/imiconsystem/ready2u-wind-direction/blob/main/images/wind-dir-indicator-web.gif)


## ฟีเจอร์การแสดงผลและสั่งงาน
- แสดงผลค่าทิศทางลม
- แสดงสถานะการควบคุมสั่งงานผ่าน RELAY
- แสดงสถานะเงื่อนไขสั่งงานที่ได้ตั้งค่าไว้ล่วงหน้า

## ฟีเจอร์การตั้งค่า
- ตั้งค่าการเชื่อมต่อ WiFi
- ตั้งค่าเงื่อนไขสั่งงาน RELAY
- การตั้งค่าโหมดการทำงานแบบ Accesspoint หรือ Station


## Compatible hardware
1. บอร์ดพัฒนา New Micromation Dev Board V3 with ESP32 [>> เรียนรู้เพิ่มเติมและสั่งซื้อ](https://www.imiconsystem.com/product/new-micromation-dev-board-v3-lite-with-esp32-and-enclosure/)

2. RS-FX-N01 RS485 Modbus เซ็นเซอร์วัดทิศทางลม Wind Direction Sensor [>> เรียนรู้เพิ่มเติมและสั่งซื้อ](https://www.imiconsystem.com/product/rs-fx-n01-rs485-%e0%b9%80%e0%b8%8b%e0%b9%87%e0%b8%99%e0%b9%80%e0%b8%8b%e0%b8%ad%e0%b8%a3%e0%b9%8c%e0%b8%a7%e0%b8%b1%e0%b8%94%e0%b8%97%e0%b8%b4%e0%b8%a8%e0%b8%97%e0%b8%b2%e0%b8%87%e0%b8%a5%e0%b8%a1/)

3. RS-485 HUB [>> เรียนรู้เพิ่มเติมและสั่งซื้อ](https://www.imiconsystem.com/product/rs-485-hub/)

## Hardware setup

ใช้ Micromation with RS-485 Module ต่อใช้งานร่วมกับ Wind Speed Sensor RS-FS-N01 ผ่าน RTU / RS-485 ด้วยโปรโตคอล Modbus Protocol โดยใช้คอนโทรลเลอร์ ESP32 Dev Kit V1 [>> เรียนรู้เพิ่มเติม](https://www.imiconsystem.com/product/rs-fx-n01-rs485-%e0%b9%80%e0%b8%8b%e0%b9%87%e0%b8%99%e0%b9%80%e0%b8%8b%e0%b8%ad%e0%b8%a3%e0%b9%8c%e0%b8%a7%e0%b8%b1%e0%b8%94%e0%b8%97%e0%b8%b4%e0%b8%a8%e0%b8%97%e0%b8%b2%e0%b8%87%e0%b8%a5%e0%b8%a1/)

## Software setup
1. ดาวน์โหลดและติดตั้ง Arduino IDE https://www.arduino.cc/en/software
2. เพิ่มบอร์ด ESP32 board package สำหรับ Arduino IDE
- เข้าเมนู File > Preferences, ใส่ค่าใน "Additional Boards Manager URLs" ด้วย: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_dev_index.json
- เข้าเมนู Tools > Board > Boards Manager..., พิมพ์ "esp32" ในช่องค้นหา, เลือกติดตั้งบอร์ด ****esp32**** รุ่นล่าสุด หากไม่เจอให้รีสาตาร์ทโปรแกรม Arduino IDE
- เข้าเมนู Tools -> Manage Libraries ติดตั้ง Library ต่อไปนี้
1. MicromationDevboardV3

## Flash โปรแกรมลง ESP32
1. เปิดโปรแกรม Arduino IDE
2. เข้าเมนู Tools เลือกบอร์ด DOIT ESP32 DEVKIT V1
3. เลือกพอร์ต
5. เปิดไฟล์ ready2u-anemometer-alarm.ino
6. อัพโหลดไฟล์

## การใช้งานตั้งค่า 'SET MODE' 
- กด Switch ค้างไว้ 6 วินาที รอจนได้ยินเสียงบี๊บ 1 ครั้งจึงปล่อยปุ่ม
- จากนั้นเชื่อมต่อระบบผ่าน WiFi ด้วย SSID และ Password ที่แสดงบนจอ OLED;
- สามารถเข้าสู่หน้าจอตั้งค่าผ่านเว็บบราวเซอร์ด้วย URL http://192.168.4.1

SET MODE ประกอบไปด้วย

1. ส่วนของการตั้งค่า WiFi Router เชื่อมต่อ WiFi เข้ากับ Router ที่บ้านหรือสำนักงาน
2. ส่วนของการตั้งค่า Set Max Values เงื่อนไขในการตั้งค่าจุดควบคุมสั่งงานผ่าน Relay
3. ส่วนของการตั้งค่า System Setting ใช้สำหรับ ตั้งค่าชื่อและไอดีอุปกรณ์
4. การตั้งค่าโหมดการทำงานแบบ Accesspoint หรือ Station

## เริ่มต้นใช้งาน Web monitoring
- กดปุ่ม En บน Esp32 เพื่อเริ่มต้นใช้งาน รอสักครู่เพื่อให้ระบบเชื่อมต่อเข้ากับ WiFi จากนั้นสามารถเข้าสู่หน้า Web monitoring ผ่านเว็บบราวเซอร์ด้วย local domain หรือ IP ที่แสดงบนจอ OLED

## **หมายเหตุ

ระบบปฏิบัติการของท่านต้องติดตั้ง mDNS service ถึงจะสามารถเข้าดูหน้าเว็บด้วยโดเมนเนมได้ ซึ่งระบบปฏิบัติการส่วนมากจะติดตั้งให้โดยอัตโนมัติแล้ว  [>> เรียนรู้เพิ่มเติม](https://espressif.github.io/esp-protocols/mdns/en/index.html)
