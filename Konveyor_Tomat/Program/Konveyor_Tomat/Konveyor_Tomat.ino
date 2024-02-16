// Linrary yang digunakan
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

// Membuat class lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Segala perpin nan
#define pinServoA 9
#define pinServoB 10
#define pinIR_A 7
#define pinIR_B 8
#define pinRST 2

#define pinA 5
#define pinB 6
#define ENA 3

// Membuat class ServoA dan ServoB
Servo servoA;
Servo servoB;

// Variabel sudut servo
int sudutA = 180;
int sudutB = 180;

// Kondisi sensor
#define ada_Object 0
#define tekan 0

// Variabel Counter A
unsigned int CounterA = 0;
bool IR_StateA = 0;
bool IR_HoldA = false;

// Variabel Counter B
unsigned int CounterB = 0;
bool IR_StateB = 0;
bool IR_HoldB = false;

// Varialbel kondisi tombol reset
bool RST_State = 0;

// Varialbel millis
unsigned long prevMillis1;
unsigned long prevMillis2;

// Varialbel untuk menampung data dari serial
char data;

void setup() {
  Serial.begin(9600);
  pinMode(pinIR_A, INPUT);
  pinMode(pinIR_B, INPUT);
  pinMode(pinRST, INPUT);
  pinMode(pinA, OUTPUT);
  pinMode(pinA, OUTPUT);
  pinMode(ENA, OUTPUT);
  servoA.attach(pinServoA);
  servoB.attach(pinServoB);
  servoA.write(sudutA);
  servoB.write(sudutB);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Konveyor Tomat");
  delay(2000);
  lcd.clear();
  motor();
  lcd.setCursor(0, 0);
  lcd.print("Good");
  lcd.setCursor(8, 0);
  lcd.print("Not Good");
  lcd.setCursor(0, 1);
  lcd.print(CounterA);
  lcd.setCursor(8, 1);
  lcd.print(CounterB);
}

void loop() {
  // Pembacaan data dari serial monitor
  if (millis() - prevMillis1 >= 500) {
    prevMillis1 = millis();
    while (Serial.available() > 0) {
      data = Serial.read();

      if (data == '1') {  // Kondisi ketika data yang diterima adalah 1
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("Not Good");
        sudutB = 110;
        servoB.write(sudutB);
      } else if (data == '2') {  // Kondisi ketika data yang diterima adalah 2
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.print("Good");
        sudutA = 130;
        servoA.write(sudutA);
      } else if (data == '3') {  // Kondisi ketika data yang diterima adalah 3
        motor();
        lcd.setCursor(0, 0);
        lcd.print("Good");
        lcd.setCursor(0, 1);
        lcd.print(CounterA);
        lcd.setCursor(8, 0);
        lcd.print("Not Good");
        lcd.setCursor(8, 1);
        lcd.print(CounterB);
      }
    }
  }

  // Memanggil fungsi counter
  counterA(sudutA);
  counterB(sudutB);

  // Kondisi untuk meriset nilai counter
  if (millis() - prevMillis2 >= 500) {
    prevMillis2 = millis();
    RST_State = digitalRead(pinRST);
    if (RST_State == tekan) {
      CounterA = 0;
      CounterB = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Good");
      lcd.setCursor(8, 0);
      lcd.print("Not Good");
      lcd.setCursor(0, 1);
      lcd.print(CounterA);
      lcd.setCursor(8, 1);
      lcd.print(CounterB);
    }
  }
}

// Fungsi menjalankan konveyor
void motor() {
  analogWrite(ENA, 150);
  digitalWrite(pinA, HIGH);
  digitalWrite(pinB, LOW);
}

// Fungsi menghitung counter pada sensor A
void counterA(int x) {
  IR_StateA = digitalRead(pinIR_A);
  if (IR_StateA == ada_Object && x == 130) {
    if (IR_HoldA == false) {
      CounterA++;
      delay(100);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Good");
      lcd.setCursor(8, 0);
      lcd.print("Not Good");
      lcd.setCursor(0, 1);
      lcd.print(CounterA);
      lcd.setCursor(8, 1);
      lcd.print(CounterB);
      sudutA = 180;
      delay(1000);
      servoA.write(sudutA);
      IR_HoldA = true;
      data = '3';
    }
  } else {
    IR_HoldA = false;
  }
}

// Fungsi menghitung counter pada sensor B
void counterB(int x) {
  IR_StateB = digitalRead(pinIR_B);
  if (IR_StateB == ada_Object && x == 110) {
    if (IR_HoldB == false) {
      CounterB++;
      delay(100);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Good");
      lcd.setCursor(8, 0);
      lcd.print("Not Good");
      lcd.setCursor(0, 1);
      lcd.print(CounterA);
      lcd.setCursor(8, 1);
      lcd.print(CounterB);
      delay(1000);
      sudutB = 180;
      servoB.write(sudutB);
      IR_HoldB = true;
      data = '3';
    }
  } else {
    IR_HoldB = false;
  }
}