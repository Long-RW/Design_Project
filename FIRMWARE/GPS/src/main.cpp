#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

/******************************* Wifi Setting ***********************************/
char ssid[] = "";
char pass[] = "";

/******************************* API *******************************************/
String key = "My API Key";

/******************** GPS Location Stored Variables ****************************/
double latitude  = 0.0;
double longitude = 0.0;
double accuracy  = 0.0;

void setup() {
  
  Serial.begin(9600);
  Serial.print("Start");

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");

  //We start by connecting to a wifi network
  Serial.print("Connecting to");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(".");

}

void loop() {
  char bssid
}