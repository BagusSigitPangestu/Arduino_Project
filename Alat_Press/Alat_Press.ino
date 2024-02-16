/*
  Author  : Takahada
  Date    : 28-01-2024
*/

//Library yang digunakan
#include <AccelStepper.h>       // Library motor Stepper
#include <LiquidCrystal_I2C.h>  // Library LCD I2C
#include <Keypad.h>             //Library Keypad
#include <EEPROM.h>             //Library EEPROM

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Keypad //
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '.', 'D' }
};

byte colPins[COLS] = { 34, 36, 38, 40 };  //posisi pin baris
byte rowPins[ROWS] = { 26, 28, 30, 32 };  //posisi pin kolom tambahkan jika menggunakan keypad 4x4
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char customKey, butt;

// Per Stepperan //
AccelStepper stepperZ(AccelStepper::DRIVER, 22, 24);  // pin 22 -> Pul, pin 24 -> Dir

// Variabel LS, Tombol, Buzzer //
#define PB_Up 7
#define PB_Down 5
#define Limit_sZ 2
#define Pot A0
#define Buzzer 46

bool stateZ;
bool PB_Up_state;
bool PB_Down_state;
bool klik = 0;

String dataIn;

float valDataIn = 0;
int valJarak = 0;
int dataSpeed;
int speed = 0;

int valPot;
int pers = 0;
const int numReadings = 100;
int readings[numReadings];
int readIndex = 0;
long total = 0;
int counter = 1;

// Karakter panah atas
uint8_t panahAtas[8] = {
  0b00100,
  0b01110,
  0b11111,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b00000
};

// Karakter panah bawah
uint8_t panahBawah[8] = {
  0b00000,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b11111,
  0b01110,
  0b00100
};

// Fungsi untuk pembacaan tombol
void bacaTombol() {
  PB_Up_state = digitalRead(PB_Up);
  PB_Down_state = digitalRead(PB_Down);
}

// Fungsi untuk homing
void homing() {
  bool Z_state = false;
  lcd.setCursor(5, 0);
  lcd.print("HOMING");

  while (!Z_state) {
    stateZ = digitalRead(Limit_sZ);
    if (stateZ) {
      stepperZ.setSpeed(-15000);
      stepperZ.runSpeed();
    } else {
      stepperZ.setCurrentPosition(0);
      stepperZ.runToNewPosition(400 * 10);
      stepperZ.setCurrentPosition(0);
      Z_state = true;
      lcd.clear();
    }
  }
}

// Fungsi untuk mencari nilai rata rata dari pembacaan potensio
int avg() {
  int average;
  unsigned long prevWaktu2;
  if (millis() - prevWaktu2 >= 20) {
    prevWaktu2 = millis();
    valPot = analogRead(Pot);
    total = total - readings[readIndex];
    readings[readIndex] = map(valPot, 0, 1023, 0, 100);
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
    if (readIndex >= numReadings) {
      readIndex = 0;
    }
    average = total / numReadings;
  }
  return average;
}

// Fungsi untuk pertama kali alat di nyalakan
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, panahAtas);
  lcd.createChar(1, panahBawah);
  lcd.clear();

  pinMode(PB_Up, INPUT);
  pinMode(PB_Down, INPUT);
  pinMode(Limit_sZ, INPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Pot, INPUT);

  dataSpeed = EEPROM.read(1);
  pers = dataSpeed;

  stepperZ.setMaxSpeed(10000);
  stepperZ.setAcceleration(10000);
  stepperZ.setCurrentPosition(0);

  homing();

  digitalWrite(Buzzer, HIGH);
  delay(200);
  digitalWrite(Buzzer, LOW);
  delay(200);
}

void loop() {
Display1:
  customKey = customKeypad.getKey();
  bacaTombol();
  if (customKey) {
    butt = customKey;

    if (butt == 'A') {
      lcd.clear();
      goto Display2;
    }
    if (butt == 'B') {
      lcd.clear();
      goto Display3;
    }
    if (butt == 'C') {
      counter++;
      if (counter > 2) {
        counter = 1;
      }
    }
    if (butt == 'D') {
      stepperZ.setMaxSpeed(speed);
      stepperZ.setAcceleration(speed);
      stepperZ.runToNewPosition(0);
      digitalWrite(Buzzer, HIGH);
      delay(200);
      digitalWrite(Buzzer, LOW);
      delay(200);
    }
    // if (butt == '.') {
    //   stepperZ.setCurrentPosition(0);
    //   for (int i = 1; i <= 3; i++) {
    //     digitalWrite(Buzzer, HIGH);
    //     delay(100);
    //     digitalWrite(Buzzer, LOW);
    //     delay(100);
    //   }
    // }
    if (butt == '*') {
      switch (counter) {
        case 1:  // Saat kondisi stepper berputar
          valJarak = ((50 * 8) * valDataIn);
          stepperZ.setMaxSpeed(speed);
          stepperZ.setAcceleration(speed);
          stepperZ.setCurrentPosition(0);
          stepperZ.runToNewPosition(valJarak);
          for (int i = 1; i <= 2; i++) {
            digitalWrite(Buzzer, HIGH);
            delay(200);
            digitalWrite(Buzzer, LOW);
            delay(200);
          }
          delay(2000);
          stepperZ.runToNewPosition(-4000);
          break;
        case 2:
          valJarak = ((50 * 8) * valDataIn);
          stepperZ.setMaxSpeed(speed);
          stepperZ.setAcceleration(speed);
          stepperZ.setCurrentPosition(0);
          stepperZ.runToNewPosition(valJarak * (-1));
          for (int i = 1; i <= 2; i++) {
            digitalWrite(Buzzer, HIGH);
            delay(200);
            digitalWrite(Buzzer, LOW);
            delay(200);
          }
          break;
      }
    }
  }
  bool up = false;
  bool down = false;
  unsigned long prevWaktu1;

  speed = map(pers, 0, 100, 0, 25000);
  if (PB_Down_state == klik) {
    while (!PB_Down_state) {
      bacaTombol();
      stepperZ.setSpeed(speed * -1);
      stepperZ.runSpeed();
    }
  } else if (PB_Up_state == klik) {
    lcd.setCursor(15, 1);
    lcd.write(1);
    while (!PB_Up_state) {
      bacaTombol();
      stepperZ.setSpeed(speed * 1);
      stepperZ.runSpeed();
    }

  } else {
    stepperZ.stop();
  }

  if (millis() - prevWaktu1 >= 500) {
    prevWaktu1 = millis();
    lcd.clear();
    if (counter == 1) {
      lcd.setCursor(15, 1);
      lcd.write(1);
    }
    if (counter == 2) {
      lcd.setCursor(15, 0);
      lcd.write(0);
    }
    lcd.setCursor(0, 0);
    lcd.print("Press: ");
    lcd.print(valDataIn);
    lcd.print("mm");
    lcd.setCursor(0, 1);
    lcd.print("Speed: ");
    lcd.print(pers);
    lcd.print("%");
    Serial.println(counter);
  }

  goto Display1;

Display2:
  customKey = customKeypad.getKey();
  if (customKey) {
    butt = customKey;
    dataIn += butt;

    if (butt == '*') {
      lcd.clear();
      valDataIn = dataIn.toFloat();
      dataIn = "";
      goto Display1;
    }
    if (butt == 'C') {
      lcd.clear();
      dataIn = "";
    }
  }
  lcd.setCursor(0, 0);
  lcd.print("Set Press (mm)");
  lcd.setCursor(0, 1);
  lcd.print(dataIn);
  lcd.print(" mm");

  goto Display2;

Display3:
  customKey = customKeypad.getKey();
  int dataAVG = avg();
  unsigned long prevWaktu3;
  if (customKey) {
    butt = customKey;

    if (butt == '*') {
      lcd.clear();
      pers = dataAVG;
      EEPROM.write(1, pers);
      goto Display1;
    }
  }
  if (millis() - prevWaktu3 >= 100) {
    prevWaktu3 = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Set Speed (%)");
    lcd.setCursor(0, 1);
    lcd.print(dataAVG);
    lcd.print(" %");
  }
  goto Display3;
}