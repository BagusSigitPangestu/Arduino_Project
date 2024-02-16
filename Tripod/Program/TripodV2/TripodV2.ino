/*  Author  : Takahada
    Date    : 03 Agustus 2023

*/

//================== Library yang digunakan =========================//
#include <AccelStepper.h>
#include <Ps3Controller.h>
#include <ESP32Servo.h>

//================= Per Servoan ===========================//
Servo servoZoom;  
Servo servoAuto;

int posZoom = 0;
int posAuto = 0;

//===================== Per Stepperan =====================//
AccelStepper stepperX(1, 25, 33);   // direction Digital 9 (CCW), pulses Digital 8 (CLK)
AccelStepper stepperY(1, 27, 26);  // direction Digital 11 (CCW), pulses Digital 10 (CLK)

int Analog_LX = 0;  //x-axis value
int Analog_LY = 0;  //y-axis value
int Analog_RX = 0;  //x-axis value
int Analog_RY = 0;  //y-axis value
int Analog_LX_AVG = 0;  //x-axis value average
int Analog_LY_AVG = 0;  //y-axis value average
int Analog_RX_AVG = 0;  //x-axis value average
int Analog_RY_AVG = 0;  //y-axis value average

int step1;
int step2;

//================= Limit Switch ======================//
#define LS1 16
#define LS2 17
#define LS3 21
#define LS4 22

void setup() {

  //SERIAL
  Serial.begin(9600);
  Ps3.begin();

  servoZoom.attach(18);
  servoAuto.attach(19);

  //----------------------------------------------------------------------------
  InitialValues();  //averaging the values of the 3 analog pins (values from potmeters)
  //----------------------------------------------------------------------------
  //Stepper parameters
  //setting up some default values for maximum speed and maximum acceleration
  stepperX.setMaxSpeed(10000);      //SPEED = Steps / second
  stepperX.setAcceleration(10000);  //ACCELERATION = Steps /(second)^2
  stepperX.setSpeed(1000);
  delay(500);
  //----------------------------------------------------------------------------
  stepperY.setMaxSpeed(10000);      //SPEED = Steps / second
  stepperY.setAcceleration(10000);  //ACCELERATION = Steps /(second)^2
  stepperY.setSpeed(1000);
  delay(500);
}

void loop() {
  ReadAnalog();
  stepperX.run();  //step the motor (this will step the motor by 1 step at each loop indefinitely)
  stepperY.run();
  // Serial.print("AVG_X: ");
  // Serial.println(Analog_X);
  // Serial.print("AVG_Y: ");
  // Serial.println(Analog_Y);
}

void ReadAnalog() {
  //Reading the 3 potentiometers in the joystick: x, y and r.
  // Analog_X = analogRead(Analog_X_pin);
  Analog_LY = Ps3.data.analog.stick.ly;
  Analog_LX = Ps3.data.analog.stick.lx;
  Analog_RY = Ps3.data.analog.stick.ry;
  Analog_RX = Ps3.data.analog.stick.rx;

  //if the value is 25 "value away" from the average (midpoint), we allow the update of the speed
  //This is a sort of a filter for the inaccuracy of the reading
  if(abs(Analog_LX-Analog_LX_AVG)>60) 
  {
  stepperX.setSpeed(-20*(Analog_LX-Analog_LX_AVG));    
  }
  else
  {
    stepperX.setSpeed(0);
  }
  //----------------------------------------------------------------------------  
  if(abs(Analog_LY-Analog_LY_AVG)>60) 
  {
  stepperY.setSpeed(20*(Analog_LY-Analog_LY_AVG));  
  }
  else
  {
    stepperY.setSpeed(0);
  }

  if (Analog_RY < -60){
    posZoom == 180;

  servoZoom.write(posZoom); 
  }
  else if (Analog_RY > 60){
    posZoom == 0;

  servoZoom.write(posZoom); 
  }

  if (Analog_RX < -60){
    posAuto = 180;

  servoAuto.write(posAuto); 
  }
  else if (Analog_RX > 60){
    posAuto = 0;

  servoAuto.write(posAuto); 
  }
  //----------------------------------------------------------------------------
  // if(abs(Analog_R-Analog_R_AVG)>25)
  // {
  //   Analog_R_Value = map(Analog_R, 0, 1023, 0, 255); //10 bit ADC (0-1023) and 8 bit PWM (0-255)
  //   analogWrite(LED_pin, Analog_R_Value); //modify the PWM value
  // }
}

void InitialValues() {
  //Set the values to zero before averaging
  float tempLX = 0;
  float tempLY = 0;
  float tempRX = 0;
  float tempRY = 0;
  // float tempR = 0;
  //----------------------------------------------------------------------------
  //read the analog 50x, then calculate an average.
  //they will be the reference values
  for (int i = 0; i < 50; i++) {
    tempLX += Ps3.data.analog.stick.ly;
    delay(10);  //allowing a little time between two readings
    tempLY += Ps3.data.analog.stick.lx;
    delay(10);
    tempRX += Ps3.data.analog.stick.ry;
    delay(10);  //allowing a little time between two readings
    tempRY += Ps3.data.analog.stick.rx;
    delay(10);
    //  tempR += analogRead(Analog_R_pin);
    //  delay(10);
  }
  //----------------------------------------------------------------------------
  Analog_LX_AVG = tempLX / 50;
  Analog_LY_AVG = tempLY / 50;
  Analog_RX_AVG = tempRX / 50;
  Analog_RY_AVG = tempRY / 50;
  // Analog_R_AVG = tempR/50;
  //----------------------------------------------------------------------------
  Serial.print("AVG_LX: ");
  Serial.println(Analog_LX_AVG);
  Serial.print("AVG_LY: ");
  Serial.println(Analog_LY_AVG);
  Serial.print("AVG_RX: ");
  Serial.println(Analog_RX_AVG);
  Serial.print("AVG_RY: ");
  Serial.println(Analog_RY_AVG);
  // Serial.print("AVG_R: ");
  // Serial.println(Analog_R_AVG);
  Serial.println("Calibration finished");
}