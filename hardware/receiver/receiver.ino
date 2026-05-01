/*
   PROJECT: LoRa Bus Tracker - CLOUD TRANSMITTER
   UNIT:    RECEIVER (Unit B) -> FIREBASE
*/

#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h> // Used to send data to Firebase

// --- 📝 USER SETTINGS ---
const char* ssid     = "Mi A3";       
const char* password = "mudiyam10";

// Paste your Firebase URL here (Must end with "/")
const char* databaseURL = "https://bustracker-ffb4d-default-rtdb.asia-southeast1.firebasedatabase.app/"; 

// --- LORA PINS ---
#define SS_PIN    5
#define RST_PIN   14
#define DIO0_PIN  2

// --- GLOBALS ---
String currentLat = "0.0";
String currentLon = "0.0";
String lastStatus = "Waiting for signal...";
int rssi = 0;
float lastLat = 0.0;
float lastLon = 0.0;
const float MOVE_THRESHOLD = 0.0002;

void setup() {
  Serial.begin(115200);

  // 1. Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n✅ WiFi Connected!");

  // 2. Initialize LoRa
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  if (!LoRa.begin(433E6)) { Serial.println("❌ LoRa Error!"); while (1); }
  Serial.println("✅ LoRa Ready.");
}

void sendToCloud() {
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;

    // We point to /bus.json to create a "bus" object in the database
    String url = String(databaseURL) + "bus.json"; 
    
    http.begin(url);
    http.addHeader("Content-Type", "application/json");

    // Construct JSON Payload
    String json = "{";
    json += "\"lat\":\"" + currentLat + "\",";
    json += "\"lon\":\"" + currentLon + "\",";
    json += "\"rssi\":\"" + String(rssi) + "\",";
    json += "\"status\":\"" + lastStatus + "\"";
    json += "}";

    // Send PUT request (PUT overwrites the data, so we always have the latest)
    int httpResponseCode = http.PUT(json);

    if (httpResponseCode > 0) {
      Serial.print("☁️ Cloud Updated: "); Serial.println(httpResponseCode);
    } else {
      Serial.print("❌ Cloud Error: "); Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("❌ WiFi Disconnected");
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String incoming = "";
    while (LoRa.available()) { incoming += (char)LoRa.read(); }
    incoming.trim();

    rssi = LoRa.packetRssi();
    bool newData = false;

    if (incoming.startsWith("Lat:")) {
      int commaIndex = incoming.indexOf(",");
      int lonIndex = incoming.indexOf("Lon:");
      
      float newLat = incoming.substring(incoming.indexOf("Lat:") + 4, commaIndex).toFloat();
      float newLon = incoming.substring(lonIndex + 4).toFloat();

      if (abs(newLat - lastLat) > MOVE_THRESHOLD || abs(newLon - lastLon) > MOVE_THRESHOLD) {
        currentLat = String(newLat, 6);
        currentLon = String(newLon, 6);
        lastLat = newLat;
        lastLon = newLon;
        lastStatus = "MOVING"; 
        newData = true;
      } else {
        if (lastStatus != "STATIONARY") {
          lastStatus = "STATIONARY";
          newData = true; 
        }
      }
    } 
    // ADD THIS NEW BLOCK TO CATCH THE SEARCHING STATUS
    else if (incoming.startsWith("Status:")) {
      String newStatus = incoming.substring(8); // Extract the message
      if (lastStatus != newStatus) {
        lastStatus = newStatus;
        newData = true; // Trigger a Firebase update
      }
      Serial.println("Bus Status: " + lastStatus);
    }
    
    if (newData) {
      sendToCloud();
    }
  }
}