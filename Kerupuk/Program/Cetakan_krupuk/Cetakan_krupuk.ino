#include <AccelStepper.h>
#include <MultiStepper.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

//==================== Per Stepperan ======================//
#define STEPS_PER_REV 200
float radius = 10;   // Adjust this as needed
int numSteps = 360;  // Number of steps for a complete circle (360 degrees)
float stepAngle = 360.0 / numSteps;
long positions[2];

AccelStepper stepperX(AccelStepper::DRIVER, 24, 22);  // Pin 24 = Pul, Pin 22 = Direction for X motor
AccelStepper stepperY(AccelStepper::DRIVER, 30, 28);
AccelStepper stepperZ(AccelStepper::DRIVER, 36, 34);
MultiStepper multiStepper;

//===================== LS & tombol =====================================//
#define PB1 15
#define PB2 14
#define PB3 2
#define PB4 3
#define limit_sX 4
#define limit_sY 5
#define limit_sZ 6

bool stateX = 1;
bool stateY = 1;
bool stateZ = 1;
bool PB1_State;
bool PB2_State;
bool PB3_State;
bool PB4_State;
bool PB_Tekan = 0;
bool cetak_State = false;
bool circle_State = false;
bool potong_State = false;

String tekan = "tekan";
String angkat = "angkat";
String stop = "stop";

//===================== Driver Motor =====================================//
#define ML1 9
#define ML2 8
#define pin1 41
#define pin2 43
#define pin3 45
#define pin4 47

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  // pinMode(limit_sX, INPUT);
  // pinMode(limit_sY, INPUT);
  // pinMode(limit_sZ, INPUT);
  pinMode(PB1, INPUT);
  pinMode(PB2, INPUT);
  pinMode(PB3, INPUT);
  pinMode(PB4, INPUT);
  pinMode(ML1, OUTPUT);
  pinMode(ML2, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  Serial.println(stateX);
  Serial.println(stateY);
  Serial.println(stateZ);
  Serial.println("Sebelum\n");

  // Homing();  // Homing ke posisi awal

  // delay(3000);
  stepperX.setMaxSpeed(4000);
  stepperX.setAcceleration(4500);
  // stepperY.setCurrentPosition(0);
  // stepperY.runToNewPosition(3200 * -2);
  // stepperX.setMaxSpeed(2000);
  // stepperX.setAcceleration(1000);
  stepperY.setMaxSpeed(2000);
  stepperY.setAcceleration(1000);
  // stepperZ.setMaxSpeed(2000);
  // stepperZ.setAcceleration(1000);
  // multiStepper.addStepper(stepperX);
  // multiStepper.addStepper(stepperY);
  stepperY.setCurrentPosition(0);

  // attachInterrupt(digitalPinToInterrupt(limit_sX), HomingX, FALLING);
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("MESIN KRUPUK");
}

void loop() {
  // BacaPB();
  stepperY.runToNewPosition(3200*7);
  stepperY.runToNewPosition(-3200*7);
  stepperY.run();

  // if (PB2_State == PB_Tekan) {
  //   // Serial.println("1");
  //   stepperZ.runToNewPosition(3200*7);
  //   // Homing();
  // }
  // if (PB3_State == PB_Tekan) {
  //   // Serial.println("1");
  //   stepperZ.runToNewPosition(3200);
  //   // Homing();
  // }
  // stepperX.move(1600);
  // stepperX.run();
  // stepperY.move(-1600);
  // stepperY.run();
  // stepperZ.move(-1600);
  // stepperZ.run();
  // PB1_State = digitalRead(limit_sX);
  // Serial.println(PB1_State);
  // delay(500);
  // jalan();
}
void Homing() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("HOMING");
  bool x = false;
  bool y = false;
  bool z = false;

  stepperX.setMaxSpeed(4000);
  stepperX.setAcceleration(4500);
  stepperY.setMaxSpeed(4000);
  stepperY.setAcceleration(4500);
  stepperZ.setMaxSpeed(2000);
  stepperZ.setAcceleration(2000);
  // setSpeed(3200);
  // stepperY.r
  while (!x) {
    stateX = digitalRead(limit_sX);
    if (stateX) {
      stepperX.move(-1600);
      stepperX.run();
    } else {
      HomingX();
      x = true;
    }
  }
  while (!y) {
    stateY = digitalRead(limit_sY);
    if (stateY) {
      stepperY.move(-1600);
      stepperY.run();
    } else {
      HomingY();
      y = true;
    }
  }
  while (!z) {
    stateZ = digitalRead(limit_sZ);
    if (stateZ) {
      stepperZ.move(-1600);
      stepperZ.run();
    } else {
      HomingZ();
      z = true;
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("HOMING FINISH");
      delay(1000);
    }
  }
}

void Cetak() {
  if (!circle_State) {
    stepperX.setMaxSpeed(1000);
    stepperX.setAcceleration(500);
    stepperY.setMaxSpeed(1000);
    stepperY.setAcceleration(500);
    stepperZ.setMaxSpeed(1000);
    stepperZ.setAcceleration(500);
    long positions[2];
    for (int i = 0; i < numSteps; i += 1.8) {
      float angle = i * stepAngle;
      float radians = radians(angle);
      float x = radius * cos(radians);
      float y = radius * sin(radians);
      positions[0] = x * STEPS_PER_REV;
      positions[1] = y * STEPS_PER_REV;

      multiStepper.moveTo(positions);

      while (multiStepper.run()) {
        // Wait for the motors to reach their positions
      }
      // delay(5);
    }
    circle_State = true;
    MotorPenekan(stop);
  }
  if (circle_State == true) {
    stepperZ.runToNewPosition(3200);
    delay(2000);
    // Potong(potong_State, false);
    // delay(4000);
    // Potong(potong_State, true);
    cetak_State = false;
  }
}

void HomingX() {  // Titik tengah 3200*6
  stepperX.setCurrentPosition(0);
  stepperX.runToNewPosition(3200 * 5);
  stepperX.setCurrentPosition(0);
}
void HomingY() {  // Titik tengah 3200*6
  stepperY.setCurrentPosition(0);
  stepperY.runToNewPosition(3200 * 6);
  stepperY.setCurrentPosition(0);
}
void HomingZ() {  // Titik tengah 3200*4
  stepperZ.setCurrentPosition(0);
  stepperZ.runToNewPosition(3200);
  stepperZ.setCurrentPosition(0);
}

void MotorPenekan(String x) {
  if (x == "tekan") {
    analogWrite(ML1, 255);
    analogWrite(ML2, 0);
  } else if (x == "angkat") {
    analogWrite(ML1, 0);
    analogWrite(ML2, 255);
  } else if (x == "stop") {
    analogWrite(ML1, 0);
    analogWrite(ML2, 0);
  }
}

void Potong(bool x, bool n) {
  if (n == true) {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);
    digitalWrite(pin4, LOW);
  } else {
    if (x == true) {
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, HIGH);
      digitalWrite(pin4, LOW);
    } else {
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, HIGH);
    }
  }
}
void BacaPB() {

  PB1_State = digitalRead(PB1);
  PB2_State = digitalRead(PB2);
  PB3_State = digitalRead(PB3);
  PB4_State = digitalRead(PB4);
  Serial.print("X ");
  Serial.println(PB1_State);
  // // Serial.print("Y ");
  // // Serial.println(PB2_State);
  // // Serial.print("Z ");
  // // Serial.println(PB3_State);
}

void jalan() {
  BacaPB();
  delay(200);
  if (PB2_State == PB_Tekan) {
    // Serial.println("1");
    MotorPenekan(angkat);
    // Homing();
  } else if (PB3_State == PB_Tekan) {
    MotorPenekan(tekan);
  } else {
    MotorPenekan(stop);
  }
  if (PB1_State == PB_Tekan) {
    Serial.println("1");
    cetak_State = true;
    circle_State = false;
    potong_State != potong_State;
  }
  if (cetak_State == true) {
    stepperZ.runToNewPosition(3200 * 7);
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Mencetak");
    Serial.println("cetak");
    MotorPenekan(tekan);
    delay(4000);
    Cetak();
    lcd.clear();
  }

  lcd.setCursor(2, 0);
  lcd.print("MESIN KRUPUK");
}
