//--------------------------- Library yang di gunakan --------------//
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_TCS34725.h"
#include <Wire.h>
#include "DHT.h"

//--------------------------- Segala per pin nan -------------------//
#define RELAY_PIN_LAMPU 2   // Pin D4 atau GPIO 2
#define RELAY_PIN_KIPAS 14  // Pin D5 atau GPIO 14
#define BUZZER_PIN 12       // Pin D6 atau GPIO 12
#define DHTPIN 13           // Pin D7 atau GPIO 13

//--------------------------- Sensor Warna -------------------------//
byte gammatable[256];
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
#define commonAnode false
float red, green, blue;

//--------------------------- LCD I2C 20x4 --------------------------//
LiquidCrystal_I2C LCD (0x27, 20, 4);

//---------------------------- Segala per DHTan ---------------------//
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float varTemp;      // variabel suhu
float varHumadity;  // variabel kelembaban

//--------------------------- Variabel Millis -----------------------//
unsigned long prevmillis1;
#define jeda1 1000
unsigned long prevmillis2;
#define jeda2 1000
unsigned long WaktuAkhirBUzzer;
#define WaktuJedaBuzzer 50

//------------------------ Variabel Status --------------------------//
bool bunyiState = 1;
String kipasAutoState = "1";
String lampuAutoState = "1";
String lampuState = "1";
String kipasState = "1";

//------------------------ Konfigurasi Firebase ---------------------//
#define FIREBASE_HOST "tem-onn-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "Ho6ccFEGcoXafZ12n5Ole1KBXsgvkD9p1dTjriJm"

// #define FIREBASE_HOST "datatester-52fef-default-rtdb.firebaseio.com"
// #define FIREBASE_AUTH "Juomb6GlemUd2S7JtjacfI0AmNzQW6coh3WFb9Gr"
FirebaseData firebaseData;
FirebaseJson json;

//------------------------ Konfigurasi koneksi WiFi -----------------//
const char* ssid = "Al -Firdaus";
const char* password = "Adipapableki*#7";

int x = 0;

uint8_t derajat[8] = {
  0b01110,
  0b10001,
  0b10001,
  0b01110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void setup() {
  Serial.begin(9600);
  dht.begin();
  LCD.init();
  LCD.backlight();
  LCD.clear();
  LCD.createChar(1, derajat);

  // Inisialisasi koneksi WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    LCD.setCursor(0, 0);
    LCD.print("Connecting To WiFi");
    LCD.setCursor(0, 1);
    LCD.print(ssid);
    LCD.setCursor(0, 2);
    LCD.setCursor(x, 3);
    LCD.print(".");
    Serial.print(".");
    if (x == 20) {
      LCD.clear();
      x = 0;
    }
    x++;
  }
  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Connected To WiFi");
  LCD.setCursor(0, 1);
  LCD.print(ssid);
  LCD.setCursor(0, 3);
  LCD.print(WiFi.localIP());
  Serial.println("");
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
  delay(2000);
  LCD.clear();

//  LCD.setCursor(1,0);
  if (tcs.begin()) {
    LCD.print("Sensor Warna Ready");
  } else {
    LCD.print("Sensor Warna Error");
    while (1);
  }

  pinMode(RELAY_PIN_LAMPU, OUTPUT);
  pinMode(RELAY_PIN_KIPAS, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN_LAMPU, HIGH);
  digitalWrite(RELAY_PIN_KIPAS, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);

  // Inisialisasi koneksi Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  
  if (millis() - prevmillis1 >= jeda1) {
    prevmillis1 = millis();

    varTemp = dht.readTemperature();
    varHumadity = dht.readHumidity();

    tcs.setInterrupt(false);
    delay(60);
    tcs.getRGB(&red, &green, &blue);
    tcs.setInterrupt(true);
    delay(60);
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Temperature :");
    LCD.setCursor(14, 0);
    LCD.print(varTemp);
    LCD.setCursor(18, 0);
    LCD.write(1);
    LCD.print("C");

    LCD.setCursor(0, 1);
    LCD.print("Humidity    :");
    LCD.setCursor(14, 1);
    LCD.print(varHumadity);
    LCD.setCursor(18, 1);
    LCD.print("%");

    LCD.setCursor(0, 2);
    LCD.print("R:");
    LCD.print(int(red));
    LCD.setCursor(7, 2);
    LCD.print("G:");
    LCD.print(int(green));
    LCD.setCursor(14, 2);
    LCD.print("B:");
    LCD.print(int(blue));

    if ((red > 100 && red < 130) && (green > 80 && green < 100)) {
      if (blue > 50 && blue < 70) {
        LCD.setCursor(0, 3);
        LCD.print("Warna : PUTIH");
      }
    }

    Serial.print(varTemp);
    Serial.println(varHumadity);
  }

  if (millis() - prevmillis2 >= jeda2) {
    prevmillis2 = millis();
    String path = "/";
    String jsonData = "{";
    jsonData += "\"suhu\": " + String(varTemp) + ",";
    jsonData += "\"kelembaban\": " + String(varHumadity);
    jsonData += "}";

    FirebaseJson json;
    json.setJsonData(jsonData);

    if (Firebase.updateNode(firebaseData, path, json)) {
      Serial.println("Update data to Firebase succeeded");
    } else {
      Serial.println("Update data to Firebase failed");
      Serial.println(firebaseData.errorReason());
    }

    if (Firebase.getString(firebaseData, "/kipas")) {
      kipasState = firebaseData.stringData();
    }
    if (Firebase.getString(firebaseData, "/lampu")) {
      lampuState = firebaseData.stringData();
    }
    if (Firebase.getString(firebaseData, "/kipasauto")) {
      kipasAutoState = firebaseData.stringData();
    }
    if (Firebase.getString(firebaseData, "/lampuauto")) {
      lampuAutoState = firebaseData.stringData();
    }
  }

  if (kipasState == "0" && kipasAutoState == "1") {
    digitalWrite(RELAY_PIN_KIPAS, LOW);  // Kipas Nyala
  } else {
    digitalWrite(RELAY_PIN_KIPAS, HIGH);  // Kipas Mati
  }

  if (lampuState == "0" && lampuAutoState == "1") {
    digitalWrite(RELAY_PIN_LAMPU, LOW);  // Lampu Nyala
  } else {
    digitalWrite(RELAY_PIN_LAMPU, HIGH);  // Lampu Mati
  }

  if (varTemp > 40) {
    alarm();
    if (kipasAutoState == "0") {
      digitalWrite(RELAY_PIN_KIPAS, LOW);  // Kipas Nyala
    }
    if (lampuAutoState == "0") {
      digitalWrite(RELAY_PIN_LAMPU, HIGH);  // Lampu Mati
    }
  } else {
    digitalWrite(BUZZER_PIN, HIGH);
    if (kipasAutoState == "0") {
      digitalWrite(RELAY_PIN_KIPAS, HIGH);  // Kipas Mati
    }
    if (lampuAutoState == "0") {
      digitalWrite(RELAY_PIN_LAMPU, LOW);  // Lampu Nyala
    }
  }
}

void alarm() {
  if (millis() - WaktuAkhirBUzzer >= WaktuJedaBuzzer) {
    WaktuAkhirBUzzer = millis();
    if (bunyiState == 1) {
      digitalWrite(BUZZER_PIN, HIGH);
      bunyiState = 0;
    } else {
      digitalWrite(BUZZER_PIN, LOW);
      bunyiState = 1;
    }
  }
}
