/*
 * Dr.TRonik / YouTube / NİSAN 2023 / İzmir / Türkiye 
 * DS3231 RTC modül ve P10 Saat
 * 3 Adet buton ile saati manuel ayarlayabilme
 * Derleme ve karta yükleme öncesi, tüm kütüphaneler arduino ide'sine yüklenmiş olmalıdır...
 * YouTube: https://youtu.be/rSyQEbuOK-g
 * Bağlantı: https://github.com/ronaer/DS3231/commit/3c3257e1d5e774ff3d6b2eef5550fd8f6a08d10a
 * 
*/

/********************************************************************
  GLOBALS___GLOBALS___GLOBALS___GLOBALS___GLOBALS___GLOBALS___
 ********************************************************************/
//RealTimeClock
#include "RTClib.h"
RTC_DS3231 rtc;

//SPI for comminication
#include "SPI.h"

//DMD2 setup for P10
#include "DMD2.h"
#include <fonts/SystemFont5x7.h>
#include <fonts/angka6x13.h>
SoftDMD dmd(1, 1);

//Variables
int saat, dakika, saniye, p10_Brightness, newHour, newMinute;

//Definitions
#define menuPin 12   // menuPin BUTTON PIN NUMBER
#define minutePin 3  // minutePin BUTTON PIN NUMBER
#define hourPin 0    // hourPin BUTTON PIN NUMBER

bool buttonState_m = HIGH;       // buttonState_menu menuPin
bool buttonState_minute = HIGH;  // buttonState_minute
bool buttonState_hour = HIGH;    // buttonState_hour

char hour_[3];    //2 characters + null
char minute_[3];  //2 characters + null


/********************************************************************
  SETUP___SETUP___SETUP___SETUP___SETUP___SETUP___SETUP___SETUP___
 ********************************************************************/
void setup() {
  Serial.begin(9600);
  dmd.begin();
  dmd.clearScreen();

  pinMode(menuPin, INPUT_PULLUP);
  pinMode(minutePin, INPUT_PULLUP);
  pinMode(hourPin, INPUT_PULLUP);

  digitalWrite(menuPin, HIGH);
  digitalWrite(minutePin, HIGH);
  digitalWrite(hourPin, HIGH);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Bilgisayar saatine eşitleme
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0)); // Ocak 21 2014 saat 3 0 0 a ayarlama
}

/********************************************************************
  LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__LOOP__
 ********************************************************************/
void loop() {
  static unsigned long timer = millis();
  set_bright();
  dmd.setBrightness(p10_Brightness);  // set_bright() fonksiyonundan dönen değere göre panel parlaklığı...
  DateTime now = rtc.now();           // loop başlangıcında...
  saat = now.hour();                  // saat değişkenimiz saat modülünün saatine,
  dakika = now.minute();              // dakika değişkenimiz saat modülünün dakikasına,
  saniye = now.second();              // saniye değişkenimiz saat modülünün saniyesine,
  //Serial.println(p10_Brightness);

  //-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- SET CLOCK
  buttonState_m = digitalRead(menuPin);
  if (buttonState_m == LOW) {
    setHour();
    setMinute();
  } else {
    if (millis() - timer > 1000) {
      timer = millis();
      print_clock();
      printSecond();
      clearScreen();
    }
  }
  //-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -
}

/********************************************************************
  VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs__VOIDs
********************************************************************/
//=========================================== Saati panele yazdırma
void print_clock() {
  //________saniye efekti_________
  dmd.selectFont(SystemFont5x7);
  if (millis() / 1000 % 2 == 0)  // her 1 saniye için
  {
    dmd.drawChar(14, 5, ':');  //iki noktayı göster
  } else {
    dmd.drawChar(13, 5, ' ');  // gösterme
  }

  dmd.selectFont(angka6x13);  //Font seçimi

  sprintf(hour_, "%2d", saat);       // 2 basamak ve değer <10 ise başına 0 koymadan (01:09 ---> 1:09 olarak biçimlendirme)
  sprintf(minute_, "%02d", dakika);  // 2 basamak ve değer <10 ise başına 0 koyarak (1:9 ---> 1:09) biçimlendirme
  dmd.drawString(0, 1, hour_);
  dmd.drawString(19, 1, minute_);
}

//=============================================set hour
void setHour() {
  DateTime now = rtc.now();
  newHour = now.hour();

  buttonState_m = digitalRead(menuPin);
  buttonState_hour = digitalRead(hourPin);

  if (buttonState_m == LOW) {
    if (buttonState_hour == LOW) {
      newHour++;
      if (newHour > 23) newHour = 0;
      char newHour_[3];
      sprintf(newHour_, "%2d", newHour);
      dmd.drawString(0, 1, newHour_);
      dmd.drawString(19, 1, minute_);
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), newHour, now.minute(), 0));
      delay(200);
    }
  }
  for (int i = 0; i < 30; i++) { dmd.setPixel(i, 0, 0); }
  for (int i = 31; i < 60; i++) { dmd.setPixel(i, 1, 0); }
}

//=============================================set minute
void setMinute() {
  DateTime now = rtc.now();
  newMinute = now.minute();

  buttonState_m = digitalRead(menuPin);
  buttonState_minute = digitalRead(minutePin);
  if (buttonState_m == LOW) {
    if (buttonState_minute == LOW) {
      newMinute++;
      if (newMinute > 59) newMinute = 0;
      char newMinute_[3];
      sprintf(newMinute_, "%02d", newMinute);
      dmd.drawString(0, 1, hour_);
      dmd.drawString(19, 1, newMinute_);
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), newMinute, 0));
      delay(120);
    }
  }
  for (int i = 0; i < 30; i++) { dmd.setPixel(i, 0, 0); }
  for (int i = 31; i < 60; i++) { dmd.setPixel(i, 1, 0); }
}


//===========================================Saniye efekti
void printSecond() {
  DateTime now = rtc.now();
  saniye = now.second();

  if (saniye <= 29) {
    dmd.setPixel(1 + saniye, 0);
    dmd.setPixel(saniye, 0, 0);
  } else {
    dmd.setPixel(saniye - 29, 1);
    dmd.setPixel(saniye - 30, 1, 0);
  }
  if (saniye == 30) dmd.setPixel(30, 0, 0);
  if (saniye == 0) dmd.setPixel(30, 1, 0);
}

//===========================================Ekranı silme
void clearScreen() {
  buttonState_minute = digitalRead(minutePin);
  buttonState_hour = digitalRead(hourPin);

  if ((buttonState_minute == LOW) && (buttonState_hour == LOW)) {
    dmd.clearScreen();
  }
}
//============================Saate göre parlaklık  ayarlama
int set_bright() {
  if (saat >= 8 && saat < 12) {
    p10_Brightness = 30;
  } else if (saat >= 12 && saat < 19) {
    p10_Brightness = 50;
  } else if (saat >= 19 && saat < 22) {
    p10_Brightness = 10;
  } else if (saat >= 22 && saat < 8) {
    p10_Brightness = 1;
  } else p10_Brightness = 1;
  return p10_Brightness;
}

/*___İletişim:
e-posta: bilgi@ronaer.com
https://www.instagram.com/dr.tronik2023/   
YouTube: Dr.TRonik: www.youtube.com/c/DrTRonik
PCBWay: https://www.pcbway.com/project/member/shareproject/?bmbno=A0E12018-0BBC-4C
*/
