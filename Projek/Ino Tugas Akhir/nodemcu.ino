#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define RX_PIN 2 // Pin D2 pada NodeMCU sebagai RX
#define TX_PIN 3 // Pin D3 pada NodeMCU sebagai TX
#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define FIREBASE_HOST "your_firebase_project_id"
#define FIREBASE_AUTH "your_firebase_auth_token"

SoftwareSerial serial(RX_PIN, TX_PIN);
FirebaseData firebaseData;
const String firebasePath = "/sensorData";

void setup() {
  Serial.begin(9600);
  serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  if (serial.available()) {
    String data = serial.readStringUntil('\n');
    Serial.print("Received data from Arduino: ");
    Serial.println(data);

    // Parsing data dari Arduino
    float smoke, CO2, CO, CH4;
    sscanf(data.c_str(), "%f,%f,%f,%f", &smoke, &CO2, &CO, &CH4);

    // Kirim data ke Firebase
    String path = firebasePath + "/smoke";
    Firebase.RTDB.setFloat(&firebaseData, path.c_str(), smoke);
    path = firebasePath + "/co2";
    Firebase.RTDB.setFloat(&firebaseData, path.c_str(), CO2);
    path = firebasePath + "/co";
    Firebase.RTDB.setFloat(&firebaseData, path.c_str(), CO);
    path = firebasePath + "/methane";
    Firebase.RTDB.setFloat(&firebaseData, path.c_str(), CH4);

    if (Firebase.RTDB.setFloat(&firebaseData, "", 0)) {
      Serial.println("Data berhasil dikirim ke Firebase");
    } else {
      Serial.println("Gagal mengirim data ke Firebase");
      Serial.println("Alasan: " + firebaseData.errorReason());
    }
  }
}
