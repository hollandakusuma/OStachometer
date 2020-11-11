#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <SD.h>

#define pin 2
#define mark 8

//LCD 16X2 I2C
#define I2C_ADDR    0x3f//0x3F // <<----- Add your address here.  Find it from I2C Scanner
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7
LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

RTC_DS1307 rtc;
DateTime nows;

File file;
char namaFile[20];
unsigned int nomorFile;

String kalimat;
int x, y;
unsigned int count;
float rpm;
boolean tekan;

void setup() {
  Serial.begin(9600);
  // Switch on the backlight
  pinMode(pin, INPUT);
  Serial.println("Open Source Tachometer");
  
  lcd.begin(16, 2);              // initialize the lcd
  lcd.setBacklightPin(BACKLIGHT_PIN, POSITIVE);
  lcd.setBacklight(HIGH);
  
  kalimat = "OPEN  SOURCE";
  lcd.setCursor((16 - kalimat.length()) / 2, 0);
  lcd.print(kalimat);
  kalimat = "TACHOMETER";
  lcd.setCursor((16 - kalimat.length()) / 2, 1);
  lcd.print(kalimat);
  delay(3000);

  lcd.clear();
  kalimat = "INIT RTC";
  lcd.setCursor((16 - kalimat.length()) / 2, 0);
  lcd.print(kalimat);
  Serial.println(kalimat);

  if (! rtc.begin()) {
    Serial.println("RTC ERROR!!!");
    kalimat = "RTC ERROR!!!";
    lcd.setCursor((16 - kalimat.length()) / 2, 1);
    lcd.print(kalimat);
    while (1);
  }
  Serial.println("RTC OK");
  kalimat = "-  RTC OK  -";
  lcd.setCursor((16 - kalimat.length()) / 2, 1);
  lcd.print(kalimat);
  delay(2000);

  lcd.clear();
  kalimat = "INIT SD CARD";
  Serial.println(kalimat);
  lcd.setCursor((16 - kalimat.length()) / 2, 0);
  lcd.print(kalimat);

  if (!SD.begin(SS)) {
    Serial.println("SD CARD ERROR!!!");
    kalimat = "SD CARD ERROR!";
    lcd.setCursor((16 - kalimat.length()) / 2, 1);
    lcd.print(kalimat);
    while (1);
  }
  kalimat = "-  SD CARD OK  -";
  lcd.setCursor((16 - kalimat.length()) / 2, 1);
  lcd.print(kalimat);
  Serial.println(kalimat);
  delay(2000);

  cekFile();
  buatFile();

  lcd.clear();
  kalimat = "Data save to";
  lcd.setCursor((16 - kalimat.length()) / 2, 0);
  lcd.print(kalimat);
  kalimat = namaFile;
  lcd.setCursor((16 - kalimat.length()) / 2, 1);
  lcd.print(kalimat);
  delay(4000);

  lcd.clear();
  kalimat = "TACHOMETER";
  lcd.setCursor((16 - kalimat.length()) / 2, 0);
  lcd.print(kalimat);
  kalimat = "READY TO MEASURE";
  lcd.setCursor((16 - kalimat.length()) / 2, 1);
  lcd.print(kalimat);
  Serial.println("TACHOMETER READY TO MEASURE");
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("RPM = ");

}

void loop() {
  nows = rtc.now();
  unsigned long waktu = millis();
  tekan = 0, rpm = 0; count = 0;
  attachInterrupt(digitalPinToInterrupt(pin), isr, RISING);
  while (millis() - waktu < 1000) { //waktu pengambilan data selama 1 detik
  }

  detachInterrupt(digitalPinToInterrupt(pin));
  rpm = float(count) / float(mark) * 60.00 ;
  Serial.print("count=");
  Serial.print(count);
  Serial.print("\tRPM=");
  Serial.println(rpm);
  simpanData();

  lcd.setCursor(6, 0);
  lcd.print("        ");
  lcd.setCursor(6, 0);
  lcd.print(rpm);
}

void isr() {
  count++;
}

void cekFile() {
  int  nilai = 1;
  boolean a, j;
  while (1) {
    sprintf(namaFile, "tacho%02d.csv", nilai);
    j = SD.exists(namaFile);
#ifdef debug
    Serial.print(namaFile);
#endif
    if (j == 0) {
#ifdef debug
      Serial.println(F(" doesn't exist"));
#endif
      a = 1;
      break;
    }
    else {
#ifdef debug
      Serial.println(F(" exist"));
#endif
      nilai++;
    }
    //    delay(100);
    if (a == 1) {
      break;
    }
  }
}

void buatFile() {
  file = SD.open(namaFile, FILE_WRITE);
  file.println(F("Date, Time, Marking Point, Count, RPM"));
  file.flush();
  file.close();
}

void simpanData() {
  file = SD.open(namaFile, FILE_WRITE);
  file.print(nows.year());
  file.print("/");
  sd2digits(nows.month());
  file.print("/");
  sd2digits(nows.day());
  file.print(", ");
  sd2digits(nows.hour());
  file.print(":");
  sd2digits(nows.minute());
  file.print(":");
  sd2digits(nows.second());
  file.print(", ");
  file.print(mark);
  file.print(", ");
  file.print(count);
  file.print(", ");
  file.println(rpm);
  file.close();

//  Serial.print("Data have been saved to ");
//  Serial.println(namaFile);

}

void sd2digits(int number) {
  if (number >= 0 && number < 10) {
    file.print('0');
  }
  file.print(number);
}
