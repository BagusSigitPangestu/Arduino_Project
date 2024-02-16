#include <SoftwareSerial.h>
SoftwareSerial SerialBT(3, 4);  // 2 rx, 3 tx Bluetooth
// SoftwareSerial SerialHT(11, 10);  // 2 rx, 3 tx HT
String dataHT;
String dataBT;

unsigned long prevMillis1;
unsigned long prevMillis2;

bool sendState = false;
bool sendState2 = false;

#define RL_kirim 12

void setup() {
  SerialBT.begin(9600);
  Serial.begin(1200);

  pinMode(RL_kirim, OUTPUT);
  delay(100);
  digitalWrite(RL_kirim, LOW);
}
void loop() {
  // baca data dari bluetooth kemudian kirim ke HT
  if (millis() - prevMillis2 >= 2000) {
    prevMillis2 = millis();

    while (SerialBT.available() > 0) {
      delay(10);
      char d = SerialBT.read();
      dataBT += d;

      if (d == '$') {
        digitalWrite(RL_kirim, HIGH);
        sendState = true;
      }
    }
    if (sendState) {
      dataBT.remove(0, 1);
      delay(1000);
      // SerialHT.println(data);
      Serial.print(dataBT);
      delay(1000);
      digitalWrite(RL_kirim, LOW);
      sendState = false;
      dataBT = "";
    }
  }

  // Baca data dari HT kemudian kirim ke bluetooth
  if (millis() - prevMillis1 >= 1000) {
    prevMillis1 = millis();

    while (Serial.available() > 0) {
      delay(10);
      char c = Serial.read();
      dataHT += c;
      if (dataHT.length() > 1) {
        sendState2 = true;
      }
    }
    if (sendState2) {
      delay(200);
      SerialBT.print(dataHT);
      dataHT = "";
      sendState2 = false;
      
    }
  }
}