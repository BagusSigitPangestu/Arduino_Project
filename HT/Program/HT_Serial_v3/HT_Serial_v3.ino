#include <SoftwareSerial.h>
SoftwareSerial SerialBT(3, 4);  // 3 rx, 4 tx Bluetooth

unsigned long prevMillis1;
unsigned long prevMillis2;

#define maxCharactersHT 701 // Maks karakter 700
char charArrayHT[maxCharactersHT];  // Array karakter untuk menampung data
int charIndexHT = 0;                // Indeks saat memasukkan karakter

#define maxCharactersBT 701
char charArrayBT[maxCharactersBT];  // Array karakter untuk menampung data
int charIndexBT = 0;                // Indeks saat memasukkan karakter

bool sendDataState1 = false;
bool sendDataState2 = false;
#define RL_kirim 12
void setup() {
  SerialBT.begin(9600);
  Serial.begin(1200);

  pinMode(RL_kirim, OUTPUT);
  delay(100);
  digitalWrite(RL_kirim, LOW);
}

void loop() {

  while (SerialBT.available() > 0) {
    char inputChar1 = SerialBT.read();      // Membaca karakter dari serial monitor
    charArrayBT[charIndexBT] = inputChar1;  // Menyimpan karakter dalam charArray
    charIndexBT++;
    digitalWrite(RL_kirim, HIGH);
    if (inputChar1 == '$' || charIndexBT == maxCharactersBT - 1) {
      sendDataState1 = true;
    }
    if (sendDataState1) {
      charArrayBT[charIndexBT] = '\0';  // Menambahkan null-terminator pada akhir array
      Serial.print(charArrayBT);        // Mencetak charArray ke serial monitor
      charIndexBT = 0;
      delay(2000);
      digitalWrite(RL_kirim, LOW);
      sendDataState1 = false;
    }
  }

  while (Serial.available() > 0) {
    char inputChar2 = Serial.read();        // Membaca karakter dari serial monitor
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
