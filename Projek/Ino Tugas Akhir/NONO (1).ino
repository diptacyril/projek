//=========================================
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#endif

//Provide the token generation process info.
#include <addons/TokenHelper.h>
//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

//Busser pin
#define BuzzerPin D7
bool isBuzzerOn = false;
unsigned long buzzerStartTime = 0;
const unsigned long buzzerDuration = 60000;

// Insert your network credentials
#define WIFI_SSID "WIFI GOA"
#define WIFI_PASSWORD "tuhanyanganeh"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCssRvGcTI4BU0uKhlrSkL5rRNWis-PBEE"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://control-agate-default-rtdb.firebaseio.com/" 

#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//some importent variables
String sValue;
bool signupOK = false;
int led1 = 2; //PIN D4 NODEMCU ESP8266 AMICA


//unsigned long sendDataPrevMillis = 0;
//int count = 0;


//=========================================
// data
//=========================================
float smoke;
float CO2;
float CO;
float CH4;

//=========================================
//LCD
//=========================================
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

//=========================================
//Milis Data
//=========================================
unsigned long previousMillis = 0;
const long interval = 3000; 
String arrData[4];

///////////////////////////////////////////////////////
// Pesan Sesuai Kadar Gas
String TentukanKondisiSmoke (float smoke)
{
  String KondisiSmoke;
  if (smoke >= 0 && smoke < 5)
  {
    KondisiSmoke = "Normal";
  }

  if (smoke >= 5 && smoke < 15)
  {
    KondisiSmoke = "Hati-hati";
  }
 
 if (smoke >= 15)
  {
    KondisiSmoke = "Bahaya";
  }
  return KondisiSmoke;
}

String TentukanKondisiCO2 (float CO2)
{
  String KondisiCO2;
  if (CO2 >= 0 && CO2 < 550)
  {
    KondisiCO2 = "Normal";
  }

  if (CO2 >= 550 && CO2 < 800)
  {
    KondisiCO2 = "Hati-hati";
  } 
  
  if (CO2 >= 800)
  {
    KondisiCO2 = "Bahaya";
  } 
  return KondisiCO2;
}

String TentukanKondisiCO (float CO)
{
  String KondisiCO;
  if (CO >= 0 && CO < 4)
  {
    KondisiCO = "Normal";
  }

  if (CO >= 4 && CO < 9)
  {
    KondisiCO = "Hati-hati";
  }
  if (CO >= 9)
  {
    KondisiCO = "Bahaya";
  } 
  return KondisiCO;
}

String TentukanKondisiCH4 (float CH4)
{
  String KondisiCH4;
  if (CH4 >= 0 && CH4 < 2)
  {
    KondisiCH4 = "Normal";
  }

  if (CH4 >= 2 && CH4 < 9)
  {
    KondisiCH4 = "Hati-hati";
  } 
  
  if (CH4 >= 9)
  {
    KondisiCH4 = "Bahaya";
  } 
  return KondisiCH4;
}

//--------------------------------------------------------------------
void setup() 
{
  Serial.begin(9600);
  pinMode(BuzzerPin,OUTPUT);

  pinMode(led1,OUTPUT);

//=========================================
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
//Pin dan LCD
//=========================================

  lcd.begin();
  lcd.backlight();
  lcd.clear();

/* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

//--------------------------------------------------------------------
void loop()
{ 
    
  //=========================================
  //Setup Milis Untuk Menerima Data Dari Arduino
  //=========================================
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval)
  { 
    previousMillis = currentMillis;
  String data = "";
  while (Serial.available()>0)
  {
  data += char(Serial.read());
  } 
  data.trim();
  //=========================================
  
  //Baca Data Dari Arduino
  //=========================================
  if(data != "")
  {
  int index = 0;
  for(int i=0; i<= data.length(); i++) 
  {
  char delimiter = '#';
  if(data[i] != delimiter)
  arrData[index] += data[i];
  else
  index++;
  }
  if(index == 3)
  { 
  Serial.print("Data String Smoke : ");
  Serial.println(arrData[0]); //Nilai Sensor Smoke
  Serial.print("Data String CO2 : ");
  Serial.println(arrData[1]); //Nilai Sensor CO2
  Serial.print("Data String CO : ");
  Serial.println(arrData[2]); //Nilai Sensor CO
  Serial.print("Data String CH4 : ");
  Serial.println(arrData[3]); //Nilai Sensor CH4
   
  //=========================================
  //Konversi Data Array ke Float
  //=========================================

  smoke = arrData[0].toFloat();
  CO2 = arrData[1].toFloat();
  CO = arrData[2].toFloat();
  CH4 = arrData[3].toFloat();

  // Deteksi dan proses ketika nilai sensor melebihi ambang batas
  if (smoke > 15 || CO2 > 800 || CO > 9 || CH4 > 9) {
    if (!isBuzzerOn) {
      // Jika buzzer belum menyala, nyalakan buzzer dan simpan statusnya serta waktu saat ini
      digitalWrite(BuzzerPin, HIGH);
      isBuzzerOn = true;
      buzzerStartTime = millis();
    } else {
      // Jika buzzer sedang menyala, cek apakah sudah melebihi durasi
      if (millis() - buzzerStartTime >= buzzerDuration) {
        digitalWrite(BuzzerPin, LOW);
        isBuzzerOn = false;
      }
    }
  } else {
    if (isBuzzerOn) {
      // Jika buzzer sedang menyala dan nilai sensor kembali normal, matikan buzzer dan reset statusnya
      digitalWrite(BuzzerPin, LOW);
      isBuzzerOn = false;
    }
  }



String Kondisiasap = TentukanKondisiSmoke (smoke);
String FirebaseKondisismoke = "" + Kondisiasap;
String KondisiCO = TentukanKondisiCO (CO);
String FirebaseKondisiCO = "" + KondisiCO;
String KondisiCO2 = TentukanKondisiCO2 (CO2);
String FirebaseKondisiCO2 = "" + KondisiCO2;
String KondisiCH4 = TentukanKondisiCH4 (CH4);
String FirebaseKondisiCH4 = "" + KondisiCH4 ;

  if (Firebase.ready() && signupOK ) {
    
    if (Firebase.RTDB.getString(&fbdo, "/L1")) {
      if (fbdo.dataType() == "string") {
        sValue = fbdo.stringData();
        int a = sValue.toInt();
        Serial.println(a);
        if (a == 1){
          digitalWrite(led1,HIGH);
        }else{
          digitalWrite(led1,LOW);
        }
      
      }
    }
  }

    else {
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "/Detail/Smoke",smoke)){
//      Serial.println("PASSED");
       Serial.print("Smoke: ");
       Serial.println(smoke);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
        if (Firebase.RTDB.setFloat(&fbdo, "/Detail/CO2",CO2)){
//      Serial.println("PASSED");
       Serial.print("CO2: ");
       Serial.println(CO2);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
        if (Firebase.RTDB.setFloat(&fbdo, "/Detail/CO",CO)){
//      Serial.println("PASSED");
       Serial.print("CO: ");
       Serial.println(CO);
      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
        if (Firebase.RTDB.setFloat(&fbdo, "/Detail/CH4",CH4)){
//      Serial.println("PASSED");
       Serial.print("CH4: ");
       Serial.println(CH4);
      
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "/Kondisi/Smoke",FirebaseKondisismoke)){
//      Serial.println("PASSED");
       Serial.print("Firebase Kondisi Smoke: ");
       Serial.println(FirebaseKondisismoke);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "/Kondisi/CO",FirebaseKondisiCO)){
//      Serial.println("PASSED");
       Serial.print("Firebase Kondisi CO: ");
       Serial.println(FirebaseKondisiCO);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "/Kondisi/CO2",FirebaseKondisiCO2)){
//      Serial.println("PASSED");
       Serial.print("Firebase Kondisi CO2: ");
       Serial.println(FirebaseKondisiCO2);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "/Kondisi/CH4",FirebaseKondisiCH4)){
//      Serial.println("PASSED");
       Serial.print("Firebase Kondisi CH4: ");
       Serial.println(FirebaseKondisiCH4);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

 
  arrData[0] = "";
  arrData[1] = "";
  arrData[2] = "";
  arrData[3] = "";
  } 
  
  //=========================================
  //Minta Data
  //=========================================
  Serial.println("Ya");
  }
  }
