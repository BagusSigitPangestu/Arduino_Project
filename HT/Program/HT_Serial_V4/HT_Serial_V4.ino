#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define RL_kirim 26

BluetoothSerial SerialBT;
String DataBT;
String DataHT;

unsigned long prevMillis1;
unsigned long prevMillis2;

bool sendDataState1 = false;
bool sendDataState2 = false;

#define maxCharactersHT 1000         // Maks karakter 700
char charArrayHT[maxCharactersHT];  // Array karakter untuk menampung data
int charIndexHT = 0;                // Indeks saat memasukkan karakter

#define maxCharactersBT 1000
char charArrayBT[maxCharactersBT];  // Array karakter untuk menampung data
int charIndexBT = 0;                // Indeks saat memasukkan karakter

void setup() {

  Serial.begin(9600);
  Serial2.begin(1200);                 //16rx 17tx
  SerialBT.begin("HT_Chat Device 2");  //Bluetooth device name ubahh angka 1 sesuai device HTnya
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(RL_kirim, OUTPUT);
  delay(100);
  digitalWrite(RL_kirim, LOW);
}


void loop() {


  if (millis() - prevMillis2 >= 2000) {
    prevMillis2 = millis();

    while (SerialBT.available() > 0) {
      digitalWrite(RL_kirim, HIGH);
      char inputChar1 = SerialBT.read();      // Membaca karakter dari serial monitor
      charArrayBT[charIndexBT] = inputChar1;  // Menyimpan karakter dalam charArray
      charIndexBT++;
      if (inputChar1 == '$' || charIndexBT == maxCharactersBT - 1) {
        sendDataState1 = true;
      }
      if (sendDataState1) {
        delay(2000);
        charArrayBT[charIndexBT] = '\0';  // Menambahkan null-terminator pada akhir array
        Serial2.print(charArrayBT);       // Mencetak charArray ke serial monitor
        Serial.print(charArrayBT);
        charIndexBT = 0;
        delay(5000);
        digitalWrite(RL_kirim, LOW);
        sendDataState1 = false;
      }
    }
  }

  if (millis() - prevMillis1 >= 1000) {
    prevMillis1 = millis();

    while (Serial2.available() > 0) {
      char inputChar2 = Serial2.read();        // Membaca karakter dari serial monitor
      charArrayHT[charIndexHT] = inputChar2;  // Menyimpan karakter dalam charArray
      charIndexHT++;
      if (inputChar2 == '$' || charIndexHT == maxCharactersHT - 1) {
        sendDataState2 = true;
      }
      if (sendDataState2) {
        charIndexHT -= 1;
        charArrayHT[charIndexHT] = ' ';
        charArrayHT[charIndexHT] = '\0';  // Menambahkan null-terminator pada akhir array
        SerialBT.print(charArrayHT);      // Mencetak charArray ke serial monitor
        charIndexHT = 0;
        delay(200);
        sendDataState2 = false;
      }
    }
  }
}