#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <string.h>
#include <iostream>
/******************************* Wifi Setting ***********************************/
char ssid[] = "Song Quynh";
char pass[] = "songquynh25042112";
#define FIREBASE_HOST "https://gpslocation-d7c58-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "am5lN8f15pMFlFU8q4FyCH6Jz6al5CNhNRec0IVk"
/******************************* API *******************************************/
String key = "My API Key";

/******************** GPS Location Stored Variables ****************************/
double latitude  = 20.972446;
double longitude = 105.8177103;

FirebaseData firebaseData;
FirebaseJson json;
//TinyGPSPlus gps;

#define S_RX    16                // Define software serial RX pin

#define S_TX    17                // Define software serial TX pin

//SoftwareSerial SoftSerial(S_RX, S_TX);    // Configure SoftSerial library

void setup(void) {

  Serial.begin(9600);


  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");

  //We start by connecting to a wifi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Wifi Connected");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

String latitudeAdd = "1/latitude";
String longitudeAdd = "1/longitude";
double i = 0.0001;
void loop() {
  //Set Data

  Firebase.setDouble(firebaseData, latitudeAdd, latitude);
  Firebase.setDouble(firebaseData, longitudeAdd, longitude);
  Serial.println("SetData Done !");
  latitude += i;
  longitude += i;
  delay(5000);
}