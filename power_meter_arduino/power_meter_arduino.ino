#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include <SoftwareSerial.h>
SoftwareSerial mySerial(12, 13);

#include <PZEM004Tv30.h>
PZEM004Tv30 pzem1(2, 3);
PZEM004Tv30 pzem2(4, 5);
PZEM004Tv30 pzem3(10, 11);

#include "RTClib.h"
RTC_DS3231 rtc;
char dataHari[7][12] = { "Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu" };
String hari;
int tanggal, bulan, tahun, jam, menit, detik;

#define R 7
#define S 8
#define T 9

unsigned long WsLCD = 0;
const long delayLCD = 3000;
int currentDisplay = 4;

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    Serial.println("RTC Tidak Ditemukan");
    Serial.flush();
    abort();
  }
  //Atur Waktu
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(2023, 1, 21, 17, 00, 00));

  pinMode(R, OUTPUT);
  pinMode(S, OUTPUT);
  pinMode(T, OUTPUT);

  digitalWrite(R, 0);
  digitalWrite(S, 0);
  digitalWrite(T, 0);

  lcd.setCursor(5, 0);
  lcd.print("MONITORING");
  lcd.setCursor(1, 1);
  lcd.print("TEGANGAN ARUS DAYA");
  lcd.setCursor(4, 2);
  lcd.print("TIGA  SENSOR");
  lcd.setCursor(0, 3);
  lcd.print("SENSOR PZEM-004t-V30");
  delay(delayLCD);
}

void loop() {
  float voltage1 = pzem1.voltage();
  float current1 = pzem1.current();
  float power1 = voltage1 * current1;

  float voltage2 = pzem2.voltage();
  float current2 = pzem2.current();
  float power2 = voltage2 * current2;

  float voltage3 = pzem3.voltage();
  float current3 = pzem3.current();
  float power3 = voltage3 * current3;

  DateTime now = rtc.now();
  hari = dataHari[now.dayOfTheWeek()];
  tanggal = now.day(), DEC;
  bulan = now.month(), DEC;
  tahun = now.year(), DEC;
  jam = now.hour(), DEC;
  menit = now.minute(), DEC;
  detik = now.second(), DEC;
  Serial.println(hari + ", " + tanggal + "-" + bulan + "-" + tahun);
  Serial.println(jam + ":" + menit + ":" + detik);
  Serial.println();

  if (millis() - WsLCD >= delayLCD) {
    WsLCD = millis();

    switch (currentDisplay) {
      case 1:
        lcd.setCursor(5, 0);
        lcd.print(" SENSOR 1 ");

        lcd.setCursor(0, 1);
        lcd.print("Tegangan: ");
        lcd.print(voltage1);
        lcd.print(" V    ");

        lcd.setCursor(0, 2);
        lcd.print("Arus    : ");
        lcd.print(current1);
        lcd.print(" A    ");

        lcd.setCursor(0, 3);
        lcd.print("Daya    : ");
        lcd.print(power1);
        lcd.print(" W     ");

        currentDisplay = 2;
        break;

      case 2:
        lcd.setCursor(5, 0);
        lcd.print(" SENSOR 2 ");

        lcd.setCursor(0, 1);
        lcd.print("Tegangan: ");
        lcd.print(voltage2);
        lcd.print(" V    ");

        lcd.setCursor(0, 2);
        lcd.print("Arus    : ");
        lcd.print(current2);
        lcd.print(" A    ");


        lcd.setCursor(0, 3);
        lcd.print("Daya    : ");
        lcd.print(power2);
        lcd.print(" W     ");

        currentDisplay = 3;
        break;

      case 3:
        lcd.setCursor(5, 0);
        lcd.print(" SENSOR 3 ");

        lcd.setCursor(0, 1);
        lcd.print("Tegangan: ");
        lcd.print(voltage3);
        lcd.print(" V    ");

        lcd.setCursor(0, 2);
        lcd.print("Arus    : ");
        lcd.print(current3);
        lcd.print(" A    ");

        lcd.setCursor(0, 3);
        lcd.print("Daya    : ");
        lcd.print(power3);
        lcd.print(" W     ");

        currentDisplay = 4;
        break;

      case 4:
        lcd.setCursor(0, 0);
        lcd.print("                    ");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        lcd.print(String() + hari + ", " + tanggal + "-" + bulan + "-" + tahun);
        lcd.print(" ");
        lcd.setCursor(0, 2);
        lcd.print("      ");
        lcd.print(String() + jam + ":" + menit + ":" + detik);
        lcd.print("      ");
        lcd.setCursor(0, 3);
        lcd.print("                    ");

        currentDisplay = 1;
        break;
    }
  }
  mySerial.println("DATE + TIME");
  String datakirim = "#" + hari +"#"+ String(tanggal) +"#"+String(bulan)+"#"+String(tahun)+"#"+String(jam)+"#"+String(menit)+"#"+String(voltage1)+"$";
  // mySerial.println(String() + hari + ", " + tanggal + "-" + bulan + "-" + tahun);
  // mySerial.println(String() + jam + ":" + menit + ":" + detik);
  mySerial.println();

  mySerial.print("V1: ");
  mySerial.print(voltage1);
  mySerial.print("V  ");

  mySerial.print("A1: ");
  mySerial.print(current1);
  mySerial.print("A  ");

  mySerial.print("P1: ");
  mySerial.print(power1);
  mySerial.println("W  ");


  mySerial.print("V2: ");
  mySerial.print(voltage2);
  mySerial.print("V  ");

  mySerial.print("A2: ");
  mySerial.print(current2);
  mySerial.print("A  ");

  mySerial.print("P2: ");
  mySerial.print(power2);
  mySerial.println("W  ");


  mySerial.print("V3: ");
  mySerial.print(voltage3);
  mySerial.print("V  ");

  mySerial.print("A3: ");
  mySerial.print(current3);
  mySerial.print("A  ");

  mySerial.print("P3: ");
  mySerial.print(power3);
  mySerial.println("W  ");

  if (current1 >= 2) {
    digitalWrite(R, 1);
  } else {
    digitalWrite(R, 0);
  }

  if (current2 >= 2) {
    digitalWrite(S, 1);
  } else {
    digitalWrite(S, 0);
  }

  if (current3 >= 2) {
    digitalWrite(T, 1);
  } else {
    digitalWrite(T, 0);
  }
}
