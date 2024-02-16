//============Library yang digunakan===============//
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <FirebaseESP32.h>

// Variabel server
// const char* ssid = "Al -Firdaus";
// const char* password = "Adipapableki*#7";

const char* ssid = "Kompor-otomatis";
const char* password = "Kompor-otomatis";

const String urlPOST_suhu = "https://iot-uripgumulya.com/juan/suhu.php";
const String urlPOST_history = "https://iot-uripgumulya.com/juan/history.php";
const String urlPOST_pindah = "https://iot-uripgumulya.com/juan/pindah.php";
const String urlPOST_status = "https://iot-uripgumulya.com/juan/post-status.php";
const String urlGET_status = "https://iot-uripgumulya.com/juan/get-status.php";
const String urlGET_counter = "https://iot-uripgumulya.com/juan/cekCounter.php";

String payload1;
String Arrpayload1[3];

String payload2;
String Arrpayload2[3];

String web_submitState, web_SP;

unsigned long prevMillis1;
unsigned long prevMillis2;
unsigned long prevMillis3;
unsigned long prevMillis4;
unsigned long prevMillis5;
unsigned long prevMillis6;

//=================kebutuhan Serial komunikasi============/
String data;
String ArrData[6];
String SendData;

String Temp = "0";
String SP = "0";
String submitState = "0";
String StopState = "0";

int Counter;

bool parsing = false;
bool pushHistory = false;

bool post1State = false;
bool post2State = false;
bool post3State = false;
bool post4State = false;
bool get1State = false;
bool get2State = false;


void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(2, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println("WiFi connected");
  delay(100);

  delay(2000);
  digitalWrite(2, HIGH);
}

void loop() {

  if (millis() - prevMillis1 >= 1000) {
    prevMillis1 = millis();
    while (Serial2.available()) {
      char d = Serial2.read();
      data += d;

      if (d == '$') {
        parsing = true;
      }

      if (parsing) {
        int index = 0;
        for (int i = 0; i < data.length(); i++) {
          if (data[i] == '#') {
            index++;
            ArrData[index] = "";
          } else {
            ArrData[index] += data[i];
          }
        }
        Serial.println(ArrData[1]);
        Serial.println(ArrData[2]);
        Serial.println(ArrData[3]);
        Serial.println(ArrData[4]);
        Serial.println("");
        SP = ArrData[1];
        Temp = ArrData[2];
        submitState = ArrData[3];
        StopState = ArrData[4];
        parsing = false;
        data = "";
      }
    }
  }

  if (millis() - prevMillis3 >= 1000) {
    prevMillis3 = millis();
    get_Counter();
  }

  if (StopState == "1") {
    pushHistory = true;
  }

  if (millis() - prevMillis6 >= 3000 && pushHistory == true) {
    prevMillis6 = millis();
    post_History(Counter);
    delay(1000);
    post_Pindah();
    pushHistory = false;
  }

  if (submitState == "1") {
    if (post4State == false) {
      post_Status(SP, submitState);
      post4State = false;
    }
    if (millis() - prevMillis4 >= 3000 && submitState == "1" ) {
      prevMillis4 = millis();
      post_Suhu(Temp, Counter);
    }
  }

  else if (submitState == "0") {
      post_Status(SP, submitState);
      post4State = false;
  }

  if (millis() - prevMillis2 >= 3000) {
    prevMillis2 = millis();
    get_Status();
    Serial2.println("#" + web_submitState + "#" + web_SP + "#$");
    Serial1.println("#" + web_submitState + "#" + web_SP + "#$");
  }

  delay(1000);
}


void post_Suhu(String a, int b) {
  post1State = false;
  while (post1State == false) {
    HTTPClient http;
    http.begin(urlPOST_suhu);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "suhu=" + String(a) + "&counter=" + String(b);
    int httpResponseCode = http.POST(httpRequestData);

    Serial.println(httpRequestData);
    Serial.println(http.getString());
    Serial.print("HTTP POST :");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      post1State = true;
    }
    http.end();
  }
}

void post_History(int a) {
  post2State = false;
  while (post2State == false) {
    HTTPClient http;
    http.begin(urlPOST_history);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "counter=" + String(a);
    int httpResponseCode = http.POST(httpRequestData);

    Serial.println(httpRequestData);
    Serial.println(http.getString());
    Serial.print("HTTP POST :");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      post2State = true;
    }
    http.end();
  }
}

void post_Pindah() {
  post3State = false;
  while (post3State == false) {
    HTTPClient http;
    http.begin(urlPOST_pindah);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "";
    int httpResponseCode = http.POST(httpRequestData);

    Serial.println(httpRequestData);
    Serial.println(http.getString());
    Serial.print("HTTP POST :");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      post3State = true;
    }
    http.end();
  }
}

void post_Status(String a, String b) {
  while (post4State == false) {
    HTTPClient http;
    http.begin(urlPOST_status);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "set_value=" + String(a) + "&status=" + String(b);
    int httpResponseCode = http.POST(httpRequestData);

    Serial.println(httpRequestData);
    Serial.println(http.getString());
    Serial.print("HTTP POST :");
    Serial.println(httpResponseCode);
    if (httpResponseCode == 200) {
      post4State = true;
    }
    http.end();
  }
}

void get_Counter() {
  Serial.print("Fetching " + urlGET_counter + "... ");
  while (get2State == false) {
    HTTPClient http;
    http.begin(urlGET_counter);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.print("HTTP ");
      Serial.println(httpResponseCode);
      payload2 = http.getString();
      Serial.println();
      Serial.println(payload2);
      get2State = true;
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.println(":-(");
      get2State = false;
    }
    http.end();
  }

  if (get2State == true) {
    int index = 0;
    payload2 = "#" + payload2;
    for (int i = 0; i < payload2.length(); i++) {
      if (payload2[i] == '#') {
        index++;
        Arrpayload2[index] = "";
      } else {
        Arrpayload2[index] += payload2[i];
      }
    }

    // Serial.println(Arrpayload2[1]);
    // Serial.println("");

    Counter = Arrpayload2[1].toInt() + 1;
    Serial.println(Counter);
    // web_SP = Arrpayload1[1];
    // web_submitState = Arrpayload1[2];

    get2State = false;
    payload2 = "";
  }
}

void get_Status() {
  Serial.print("Fetching " + urlGET_status + "... ");
  while (get1State == false) {
    HTTPClient http;
    http.begin(urlGET_status);

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.print("HTTP ");
      Serial.println(httpResponseCode);
      payload1 = http.getString();
      Serial.println();
      Serial.println(payload1);
      get1State = true;
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.println(":-(");
      get1State = false;
    }
    http.end();
  }

  if (get1State == true) {
    int index = 0;
    payload1 = "#" + payload1;
    for (int i = 0; i < payload1.length(); i++) {
      if (payload1[i] == '#') {
        index++;
        Arrpayload1[index] = "";
      } else {
        Arrpayload1[index] += payload1[i];
      }
    }

    // Serial.println(Arrpayload1[1]);
    // Serial.println(Arrpayload1[2]);
    // Serial.println("");

    web_SP = Arrpayload1[1];
    web_submitState = Arrpayload1[2];

    get1State = false;
    payload1 = "";
  }
}