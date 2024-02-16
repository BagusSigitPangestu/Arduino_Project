#include <SoftwareSerial.h>
SoftwareSerial SerialBT(3, 4);  // 2 rx, 3 tx Bluetooth
// SoftwareSerial SerialHT(11, 10);  // 2 rx, 3 tx HT
String dataHT;
String dataBT;

#define maxCharactersHT 700
char charArrayHT[maxCharactersHT];  // Array karakter untuk menampung data
int charIndexHT = 0;                // Indeks saat memasukkan karakter

#define maxCharactersBT 700
char charArrayBT[maxCharactersBT];  // Array karakter untuk menampung data
int charIndexBT = 0;                // Indeks saat memasukkan karakter

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
  // berikut code untuk membaca data dari perangkat lain
  // if (millis() - prevMillis2 >= 2000) {
  //   prevMillis2 = millis();

    // while (SerialBT.available() > 0) {
    //   delay(10);
    //   char inputChar = SerialBT.read();
    //   digitalWrite(RL_kirim, HIGH);
    //   if (inputChar == '\n' || charIndexBT == maxCharactersBT - 1) {
    //     // Jika karakter '\n' (newline) diterima atau mencapai batas karakter maksimal
    //     delay(500);
    //     charArrayBT[charIndexBT] = '\0';  // Menambahkan null-terminator pada akhir array
    //     Serial.print(charArrayBT);  // Mencetak charArray ke serial monitor
    //     charIndexBT = 0;              // Mengatur ulang indeks karakter
    //     delay(1500);
    //     digitalWrite(RL_kirim, LOW);
    //   } else {
    //     charArrayBT[charIndexBT] = inputChar;  // Menyimpan karakter dalam charArray
    //     charIndexBT++;
    //   }
    // }
  // }

  // berikut data yang dibaca dari serial monitor sendiri, kemudian dikirim ke perangkat lain
  // if (millis() - prevMillis1 >= 1000) {
  //   prevMillis1 = millis();

    while (Serial.available() > 0) {
      delay(10);
      char inputChar2 = Serial.read();
      if (inputChar2 == '\n' || charIndexHT == maxCharactersHT - 1) {
        // Jika karakter '\n' (newline) diterima atau mencapai batas karakter maksimal
        charArrayHT[charIndexHT] = '\0';  // Menambahkan null-terminator pada akhir array
        Serial.println(charArrayHT);  // Mencetak charArray ke serial monitor
        charIndexHT = 0;              // Mengatur ulang indeks karakter
      } else {
        charArrayHT[charIndexHT] = inputChar2;  // Menyimpan karakter dalam charArray
        charIndexHT++;
      }
    }
  // }
}