//================== Library yang digunakan =========================//
#include <AccelStepper.h>
#include <Ps3Controller.h>
#include <ESP32Servo.h>
#include "EEPROM.h"

//================= Per Servoan ===========================//
#define addressRX 2
#define addressRY 3
Servo servoZoom;
Servo servoAuto;

int posZoom = 0;
int posAuto = 0;

//===================== Per Stepperan =====================//
#define addressLX 0
#define addressLY 1

AccelStepper stepperX(1, 25, 33);
AccelStepper stepperY(1, 27, 26);

int Analog_LX = 0;  //Lx-axis value
int Analog_LY = 0;  //Ly-axis value
int Analog_RX = 0;  //Rx-axis value
int Analog_RY = 0;  //Ry-axis value

int stepPosisi_X = 0;
int stepPosisi_Y = 0;

float posisi_X;
float posisi_Y;
// unsigned long prevmillis1;
// unsigned long prevmillis2;
void setup() {

  //SERIAL
  Serial.begin(9600);
  Ps3.begin();

  if (!EEPROM.begin(1000)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

  servoZoom.attach(18);
  servoAuto.attach(19);

  servoZoom.write(posZoom);
  servoAuto.write(posAuto);

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  posisi_X = EEPROM.read(addressLX);
  posisi_X = map(posisi_X, 1, 255, -3200, 3200);
  stepperX.setCurrentPosition(posisi_X);
  Serial.println("Setelah conv :" + String(posisi_X));
  Serial.println(".........................");
  stepperX.setMaxSpeed(10000);      //SPEED = Steps / second
  stepperX.setAcceleration(10000);  //ACCELERATION = Steps /(second)^2
  stepperX.runToNewPosition(0);
  delay(500);

  stepperX.setMaxSpeed(60000);  //SPEED = Steps / second
  stepperX.setAcceleration(60000);
  delay(500);
  //----------------------------------------------------------------------------
  posisi_Y = EEPROM.read(addressLY);
  posisi_Y = map(posisi_Y, 1, 255, -5500, 5000);
  stepperY.setCurrentPosition(posisi_Y);
  Serial.println("Setelah conv :" + String(posisi_Y));
  Serial.println(".........................");
  stepperY.setMaxSpeed(10000);      //SPEED = Steps / second
  stepperY.setAcceleration(10000);  //ACCELERATION = Steps /(second)^2
  stepperY.runToNewPosition(0);
  delay(500);

  stepperY.setMaxSpeed(90000);  //SPEED = Steps / second 
  stepperY.setAcceleration(90000);
  delay(500);
}

void loop() {

  //------------------- Pembacaan Nilai analog Stick PS3-----------------------------------------
  Analog_LY = Ps3.data.analog.stick.ly;
  Analog_LX = Ps3.data.analog.stick.lx;
  Analog_RY = Ps3.data.analog.stick.ry;
  Analog_RX = Ps3.data.analog.stick.rx;

  //----------------------------- Putaran Steper X-----------------------------------------------
  if (Analog_LX > 60 && stepPosisi_X >= -3200) { // ke kiri
    stepPosisi_X -= 50;
    stepperX.runToNewPosition(stepPosisi_X);
  }
  if (Analog_LX < -60 && stepPosisi_X <= 3200) { // ke kanan
    stepPosisi_X += 50;
    stepperX.runToNewPosition(stepPosisi_X);
  }
  //----------------------------- Putaran Steper Y-----------------------------------------------
  if (Analog_LY > 60 && stepPosisi_Y >= -5500) {  // Turun ke bawah
    stepPosisi_Y -= 50;
    stepperY.runToNewPosition(stepPosisi_Y);
  }
  if (Analog_LY < -60 && stepPosisi_Y <= 5000) {  // Naik Ke atas 
    stepPosisi_Y += 50;
    stepperY.runToNewPosition(stepPosisi_Y);
  }
  //----------------------------- Putaran Servo Zoom -----------------------------------------------
  if (Analog_RY < -60 && posZoom <= 80) { // 80 bisa di gantii
    posZoom += 1;
    servoZoom.write(posZoom);
    delay(30);
  } else if (Analog_RY > 60 && posZoom >= 0) {
    posZoom -= 1;
    servoZoom.write(posZoom);
    delay(30);
  }

  //---------------------------- Putaran Servo Auto Fokus ------------------------------------------------
  if (Analog_RX < -60 && posAuto <= 70) { // 70 bisa digantii
    posAuto += 1;
    servoAuto.write(posAuto);
    delay(30);
  } else if (Analog_RX > 60 && posAuto >= 0) {
    posAuto -= 1;
    servoAuto.write(posAuto);
    delay(30);
  }

  if (Analog_LX < 50 && Analog_LX > -50) {
    posisi_X = map(stepPosisi_X, -3200, 3200, 1, 255);
    EEPROM.write(addressLX, posisi_X);
    EEPROM.commit();
  }

  if (Analog_LY < 50 && Analog_LY > -50) {
    posisi_Y = map(stepPosisi_Y, -5500, 5000, 1, 255);
    EEPROM.write(addressLY, posisi_Y);
    EEPROM.commit();
  }
}