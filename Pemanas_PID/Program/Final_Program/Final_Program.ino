#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <AccelStepper.h>
#include <EEPROM.h>
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {  //menggunakan keypad 4x3
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte colPins[COLS] = { 10, 9, 8, 7 };  //posisi pin baris
byte rowPins[ROWS] = { 6, 5, 4, 3 };   //posisi pin kolom tambahkan jika menggunakan keypad 4x4

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define ONE_WIRE_BUS 11
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

AccelStepper stepper(1, 12, 13);


// Variabel PID
float Kp = 50, Ki = 0, Kd = 0;
float PID_p;
float PID_i;
float PID_d;
int PID_total;
int PID_total_Step;
float Error;
float prev_Error;
int sp = 0;

bool StateStep = false;
bool epromState = true;


bool PID_State = true;
bool Off_Step = false;

unsigned long prevWaktu;
unsigned long prevWaktu2;
int interval = 1000;

int posisiAwal;
int posisiSaatIni;

// Variabel Suhu
float Temp = 0;

char customKey;
String Data;
char butt;
String DataTemp;

int Screen1_State = 0;
int Screen2_State = 0;

//======= Serial komunikasi======//
String data;
String ArrData[3];
bool parsing = false;
String webState = "0";
String webSP;
String submitState = "0";
String StateStop = "0";

// void calculatePID(int SP);
void bacaPID(int x);
void Screen();
void stepPosisi(int Posisi);
// void bacaKey();


void bacaSuhu() {
  sensors.requestTemperatures();
  Temp = sensors.getTempCByIndex(0);
}

void setup() {

  sensors.begin();
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  Timer1.initialize(500000);
  Timer1.attachInterrupt(bacaSuhu);
  stepper.setMaxSpeed(40000);  // Stepper motor speed and acceleration parameters (Units are steps/sec and steps/second^2, respectively)
  stepper.setAcceleration(40000);
  posisiAwal = EEPROM.read(0);
  posisiAwal = map(posisiAwal, 0, 100, 0, -580);
  stepPosisi(0);

  Serial.println(posisiAwal);
  delay(1000);
}

void loop() {
  Screen();
}

void bacaPID(int x) {
  Error = x - Temp;
  PID_p = Kp * Error;
  PID_i = Ki * (Error + prev_Error);
  PID_d = Kd * (Error - prev_Error);

  PID_total = (PID_p + PID_i + PID_d);

  if (x < Temp) {
    PID_State = false;
    stepPosisi(0);
    posisiSaatIni = 0;
    EEPROM.write(0, posisiSaatIni);
  } else {
    PID_State = true;
  }

  if (PID_State == true) {
    PID_total_Step = -1.2*PID_total;
    if (PID_total_Step < -580){
      PID_total_Step = -580;
    }
    stepPosisi(PID_total_Step);
    posisiSaatIni = map(PID_total_Step, 0, -580, 0, 100);
    EEPROM.write(0, posisiSaatIni);
  }

  prev_Error = Error;
}


void stepPosisi(int Posisi) {
  stepper.setMaxSpeed(40000);  //set speed
  stepper.runToNewPosition(Posisi);
}

void Screen() {
Tampil1:
  komSerial();
  customKey = customKeypad.getKey();
  if (customKey) {
    butt = customKey;
    Serial.println(butt);
    if (butt == 'A') {
      lcd.clear();
      Screen1_State = 1;
      goto Tampil2;
    }
  }
  if (submitState == "1") {
    lcd.clear();
    sp = webSP.toInt();
    Screen1_State = 0;
    goto Tampil3;
  }
  lcd.setCursor(0, 0);
  lcd.print("UNDIP");
  lcd.setCursor(10, 1);
  lcd.print("Klik A");
  goto Tampil1;

Tampil2:
  customKey = customKeypad.getKey();
  if (customKey) {
    butt = customKey;
    DataTemp += butt;
    if (butt == '#') {
      submitState = "1";
      Serial.println("#" + String(sp) + "#" + String(Temp) + "#" + submitState + "#" + StateStop + "#$");  // Komunikasi Serial
      lcd.clear();
      DataTemp = "";
      goto Tampil3;
    }
    if (butt == 'C') {
      if (Screen1_State == 1) {
        epromState = false;
        lcd.clear();
        DataTemp = "";
        sp = "";
      }
    }
  }

  if (epromState) {
    sp = EEPROM.read(1);
    lcd.setCursor(0, 0);
    lcd.print("Set Point :");
    lcd.print(sp);
  }

  if (Screen1_State == 1 && epromState == false) {
    sp = DataTemp.toInt();
    lcd.setCursor(0, 0);
    lcd.print("Set Point :");
    lcd.print(sp);
  }
  goto Tampil2;


Tampil3:
  komSerial();
  if (submitState == "0") {
    lcd.clear();
    goto Tampil2;
  }
  customKey = customKeypad.getKey();
  if (customKey) {
    butt = customKey;
    if (butt == '#') {
      lcd.clear();
      sp = 0;
      EEPROM.write(1, sp);
      Screen1_State = 1;
      submitState = "0";
      delay(100);
      Serial.println("#" + String(sp) + "#" + String(Temp) + "#" + submitState + "#" + StateStop + "#$");
      goto Tampil2;
    }
    if (butt == '*') {
      lcd.clear();
      StateStop = "1";
      stepPosisi(0);
      Serial.println("#" + String(sp) + "#" + String(Temp) + "#" + submitState + "#" + StateStop + "#$");
      goto Tampil4;
    }
  }
  EEPROM.write(1, sp);
  bacaPID(sp);
  unsigned long prevMillis1;
  unsigned long prevMillis2;

  if (millis() - prevMillis1 >= 500) {
    prevMillis1 = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SP:");
    lcd.print(sp);
    lcd.setCursor(7, 0);
    lcd.print("PID :");
    lcd.print(PID_total);
    lcd.setCursor(0, 1);
    lcd.print("Temp:");
    lcd.print(Temp);
  }

  if (millis() - prevMillis2 >= 1000) {
    prevMillis2 = millis();
    Serial.println("#" + String(sp) + "#" + String(Temp) + "#" + submitState + "#" + StateStop + "#$");
  }
  delay(500);


  goto Tampil3;

Tampil4:

  customKey = customKeypad.getKey();
  if (customKey) {
    butt = customKey;
    if (butt = 'A') {
      lcd.clear();
      submitState = "0";
      StateStop = "0";
      stepPosisi(0);
      posisiSaatIni = 0;
      EEPROM.write(0, posisiSaatIni);
      Serial.println("#" + String(sp) + "#" + String(Temp) + "#" + submitState + "#" + StateStop + "#$");
      goto Tampil2;
    }
    else{
      goto Tampil4;
    }
  }

  submitState = "0";
  StateStop = "0";
  stepPosisi(0);
  posisiSaatIni = 0;
  EEPROM.write(0, posisiSaatIni);

  lcd.setCursor(5, 0);
  lcd.print("Stop");
  Serial.println("#" + String(sp) + "#" + String(Temp) + "#" + submitState + "#" + StateStop + "#$");
  delay(1000);
  goto Tampil4;
}

void komSerial() {
  if (millis() - prevWaktu >= interval) {
    prevWaktu = millis();
    while (Serial.available()) {
      char d = Serial.read();
      data += d;
      if (d == '$') {
        parsing = true;
      }
      if (parsing) {
        int index = 0;
        for (int i = 0; i < data.length(); i++) {
          if (data[i] == '#') {
            index++;
            ArrData[index] = "";
          } else {
            ArrData[index] += data[i];
          }
        }

        submitState = ArrData[1];
        webSP = ArrData[2];
        parsing = false;
        data = "";
      }
    }
  }
}