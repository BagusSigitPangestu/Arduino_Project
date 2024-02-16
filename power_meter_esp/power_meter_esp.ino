#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
char auth[] = "AjPEAEMhyMwEmxkL4X6NfGlVtULCQuWJ";
char ssid[] = "Pop";
char pass[] = "agung12345";

#include <SoftwareSerial.h>

#define pwrArduino 5

String data;

char dataHari[7][12] = { "Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu" };
String hari;
int tanggal, bulan, tahun, jam, menit, detik;

float voltage1 = 0.0;
float current1 = 0.0;
float power1 = 0.0;

float voltage2 = 0.0;
float current2 = 0.0;
float power2 = 0.0;

float voltage3 = 0.0;
float current3 = 0.0;
float power3 = 0.0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(pwrArduino, OUTPUT);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  digitalWrite(pwrArduino, 1);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();

  while (Serial.available() > 0) {
    char d = Serial.read();
    data += d;
    // delay(1/0);
  }

  if (data.length() > 0) {
    Serial.print(data);
    data = "";
  }

  while (mySerial.available() > 0) {
    char d = mySerial.read();
    data += d;
    delay(10);
  }

  if (data.length() > 0) {
    // if (data.startsWith("DATE + TIME")) {
    //   Serial.println(String() + hari + ", " + tanggal + "-" + bulan + "-" + tahun);
    //   Serial.println(String() + jam + ":" + menit + ":" + detik);
    //   Serial.println();
    //   Blynk.virtualWrite(V10, String() + hari + ", " + tanggal + "-" + bulan + "-" + tahun + "//" + jam + ":" + menit + ":" + detik);
    // }
    if (data.startsWith("V1: ")) {
      voltage1 = data.substring(4).toFloat();
      Serial.print("V1: ");
      Serial.print(voltage1);
      Serial.print("V  ");
      Blynk.virtualWrite(V1, voltage1);
    }
    if (data.startsWith("A1:")) {
      current1 = data.substring(4).toFloat();
      Serial.print("A1: ");
      Serial.print(current1);
      Serial.print("A  ");
      Blynk.virtualWrite(V2, current1);
    }
    if (data.startsWith("P1:")) {
      power1 = data.substring(4).toFloat();
      Serial.print("P1: ");
      Serial.print(power1);
      Serial.println("W  ");
      Blynk.virtualWrite(V3, power1);
    }

    if (data.startsWith("V2: ")) {
      voltage2 = data.substring(4).toFloat();
      Serial.print("V2: ");
      Serial.print(voltage2);
      Serial.print("V  ");
      Blynk.virtualWrite(V4, voltage2);
    }
    if (data.startsWith("A2:")) {
      current2 = data.substring(4).toFloat();
      Serial.print("A2: ");
      Serial.print(current2);
      Serial.print("A  ");
      Blynk.virtualWrite(V5, current2);
    }
    if (data.startsWith("P2:")) {
      power2 = data.substring(4).toFloat();
      Serial.print("P2: ");
      Serial.print(power2);
      Serial.println("W  ");
      Blynk.virtualWrite(V6, power2);
    }

    if (data.startsWith("V3: ")) {
      voltage3 = data.substring(4).toFloat();
      Serial.print("V3: ");
      Serial.print(voltage3);
      Serial.print("V  ");
      Blynk.virtualWrite(V7, voltage3);
    }
    if (data.startsWith("A3:")) {
      current3 = data.substring(4).toFloat();
      Serial.print("A3: ");
      Serial.print(current3);
      Serial.print("A  ");
      Blynk.virtualWrite(V8, current3);
    }
    if (data.startsWith("P3:")) {
      power3 = data.substring(4).toFloat();
      Serial.print("P3: ");
      Serial.print(power3);
      Serial.println("W  ");
      Blynk.virtualWrite(V9, power3);
    }
    data = "";
  }
}
