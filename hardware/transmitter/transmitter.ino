/*
   PROJECT: LoRa Bus Tracker
   UNIT:    TRANSMITTER (Bus Unit)
   STATUS:  Sends "Status: Searching..." if no GPS lock.
*/

#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

// --- LORA PINS ---
#define SCK_PIN    18
#define MISO_PIN   19
#define MOSI_PIN   23
#define SS_PIN     5
#define RST_PIN    14
#define DIO0_PIN   2

// --- GPS PINS (Updated) ---
#define RXPin      26 
#define TXPin      27 
#define GPSBaud    9600

// --- OBJECTS ---
TinyGPSPlus gps;
HardwareSerial gpsSerial(2); // Use UART2

void setup() {
  Serial.begin(115200);
  
  // 1. Start GPS
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  Serial.println("\n--- TRANSMITTER STARTING ---");

  // 2. Start LoRa
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  
  if (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa Error: Check wiring!");
    while (1);
  }
  Serial.println("✅ LoRa Online.");
}

void loop() {
  // 1. Read data from GPS
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  // 2. Decide what to send
  if (gps.location.isValid()) {
    // SCENARIO A: Good Signal -> Send Location
    String msg = "Lat:" + String(gps.location.lat(), 6) + ",Lon:" + String(gps.location.lng(), 6);
    
    Serial.print("✅ GPS LOCK! Sending: ");
    Serial.println(msg);
    
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
  } 
  else {
    // SCENARIO B: No Signal -> Send "Searching" Status
    // This proves to the Receiver that the radio is working
    Serial.println("⚠️ GPS Searching... (Sending status to Receiver)");
    
    LoRa.beginPacket();
    LoRa.print("Status: Searching for Satellites...");
    LoRa.endPacket();
  }
  
  delay(1000); // Wait 1 second
}