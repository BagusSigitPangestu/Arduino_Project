//============ Library yang digunakan=============//
#include <Adafruit_MLX90614.h>
#include <MQUnifiedsensor.h>
#include <WiFi.h>
#include <HTTPClient.h>

//=================== Variabel Server ===============//
const char* ssid = "Inkubator-bayi";
const char* password = "Inkubator-bayi123";

const String urlPOST = "https://smartbabyincubator.000webhostapp.com/inc/post.php";
bool postState = false;

//=================== Variabel mq135=================//
#define placa "ESP32"
#define Voltage_Resolution 3.3
#define pin 35                  //Analog input 0 of your arduino
#define type "MQ-135"           //MQ135
#define ADC_Bit_Resolution 12   // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6  //RS / R0 = 3.6 ppm
//Declare Sensor
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

float Oksigen, valOksigen;
String kipasState = "OFF";

//=================== Variabel mlx90614 ==============//
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
float Temp;
String lampuState = "OFF";

#define kipasPIN 26
#define lampuPIN 25

unsigned long prevMillis1;
unsigned long prevMillis2;

//======================================================//
void setup() {
  Serial.begin(9600);

  pinMode(kipasPIN, OUTPUT);
  pinMode(lampuPIN, OUTPUT);

  digitalWrite(kipasPIN, LOW);
  digitalWrite(lampuPIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1)
      ;
  };

  MQ135.setRegressionMethod(1);  //_PPM =  a*ratio^b
  MQ135.setA(110.47);
  MQ135.setB(-2.862);
  MQ135.init();
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++) {
    MQ135.update();  // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / 10);
}

//=========================================================================//
void loop() {

  if (millis() - prevMillis1 >= 1000) { // Pembacaan sensor setiap 1 detik
    prevMillis1 = millis();
    Temp = mlx.readObjectTempC();

    for(int i = 0; i < 10; i++){ // Mengambil nilai rata-rata ppm dengan pembacaan sebanyak 10x
    MQ135.update();                // Update data, the arduino will read the voltage from the analog pin
    valOksigen += MQ135.readSensor();  // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
    }
    Oksigen = valOksigen/10;
    valOksigen = 0;

    if (Oksigen >= 500){ // Membatasi pembacaan nilai ppm agar max 500 ppm
      Oksigen = 500;
    }

    //               A   ,  B ,  C ,  D , E   => Ket : A = Niali yng akan di rubah ke persentase, B = nilai normal ppm, C = Nilai max ppm, D = niali max persentase, E = nilai min persentase
    Oksigen = map(Oksigen, 3.2, 500, 100, 0); // Konversi dari ppm ke persentase, Nilai B di sesuaikan dengan nilai kondisi kalibrasi PPM
  }

  if (Oksigen <= 90 || Temp > 35) { //Jika presentase oksigen kurang sama dengan 90 atau Suhu Lebih dari 35 makan kipas nyala
    kipasState = "ON";
    digitalWrite(kipasPIN, HIGH);
  } else {
    kipasState = "OFF";
    digitalWrite(kipasPIN, LOW);
  }
  if (Temp < 35) { // Jika suhi kurang dari 35
    lampuState = "ON";
    digitalWrite(lampuPIN, HIGH);
  } else {
    lampuState = "OFF";
    digitalWrite(lampuPIN, LOW);
  }

  if (millis() - prevMillis2 >= 5000){ // Request Pengiriman data ke server setiap 5 detik
    prevMillis2 = millis();
    postState = false;
    post(Temp, Oksigen, kipasState, lampuState);
  }

  Serial.print("Suhu Bayi : ");
  Serial.println(Temp);
  Serial.print("Persentase oksigen :");
  Serial.println(Oksigen);
  Serial.println("===================================");
  delay(2000);
}

//========================================================================//
void post(float suhu, float oksigen, String kipas, String lampu) {
  while (postState == false){
    HTTPClient http;
  http.begin(urlPOST);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "id=" + String(1000) + "&suhu=" + String(suhu) + "&oksigen=" + String(oksigen) + "&kipas=" + kipas + "&lampu=" + lampu + "";
  int httpResponseCode = http.POST(httpRequestData);

  Serial.println(httpRequestData);
  Serial.println(http.getString());
  Serial.print("HTTP POST :");
  Serial.println(httpResponseCode);
  if (httpResponseCode == 200){
    postState = true;
  }
  http.end();
  }
}
