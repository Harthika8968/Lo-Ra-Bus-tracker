# LoRa-Based College Bus Tracker

A real-time, long-range bus tracking system that operates without cellular SIM cards on the vehicle. This project utilizes a LoRa radio module to transmit GPS coordinates from the bus to a stationary receiver, which subsequently uploads the data to a Firebase Realtime Database for web-based monitoring.

## Key Features

* **SIM-less Transmission:** The bus unit utilizes a 433MHz LoRa module to transmit coordinates, eliminating recurring cellular data costs.
* **Real-Time Cloud Synchronization:** The receiver unit connects to a local WiFi network and updates the Firebase Realtime Database via HTTP PUT requests upon detecting movement.
* **Minimalist Web Interface:** The front-end implements a clean, glassmorphism design with smooth marker animations for a refined user experience.
* **Smart Mapping:** Developed using Leaflet.js, incorporating automatic road-snapping (via OSRM) to plot an accurate historical route.
* **Proximity Alerts:** The web application requests notification permissions to alert users when the bus enters a 2.5km radius.
* **Failsafe Status Updates:** In the event of GPS signal loss, the transmitter broadcasts a "Searching for Satellites..." status to the receiver to maintain and verify radio connectivity.

## Tech Stack

* **Hardware:** ESP32, LoRa (SX1278), Neo-6M GPS Module
* **Backend:** Firebase Realtime Database
* **Frontend:** HTML5, CSS3, JavaScript (Leaflet.js)

## Hardware Components

### Transmitter Unit (Vehicle)
* 1x ESP-32 Microcontroller
* 1x LoRa Module (SX1278) with soldered male header pins
* 1x 433MHz Antenna for LoRa
* 1x Neo-6M GPS Module
* 1x Antenna for GPS Module
* 1x Breadboard & Jumper Wires

### Receiver Unit (Stationary)
* 1x ESP-32 Microcontroller
* 1x LoRa Module (SX1278) with soldered male header pins
* 1x 433MHz Antenna for LoRa
* 1x Breadboard & Jumper Wires

---

## Wiring Schematics

### Transmitter Connections
**LoRa Module (SX1278) to ESP32:**
* VCC connects to 3.3V (Red Rail)
* GND connects to GND (Blue Rail)
* NSS connects to D5
* MOSI connects to D23
* MISO connects to D19
* SCK connects to D18
* RST connects to D14
* DIOO connects to D2

**Neo-6M GPS Module to ESP32:**
* VCC connects to 3.3V (Red Rail)
* GND connects to GND (Blue Rail)
* TX connects to D26 (Sends Location)
* RX connects to D27 (Receives Data)

### Receiver Connections
**LoRa Module (SX1278) to ESP32:**
*(Recommended wire colors included for logical organization)*
* VCC connects to 3.3V (Red wire)
* GND connects to GND (Black wire)
* NSS connects to D5 (Yellow/Green wire)
* MOSI connects to D23 (Blue wire)
* MISO connects to D19 (Violet wire)
* SCK connects to D18 (Orange wire)
* RST connects to D14 (White wire)
* DIOO connects to D2 (Grey/Brown wire)

## Setup Instructions

1. **Hardware Setup:** Flash `transmitter.ino` and `receiver.ino` to two separate ESP32 boards using the Arduino IDE.
2. **WiFi Configuration:** In `receiver.ino`, update the `ssid` and `password` variables with the local network credentials.
3. **Database:** Create a Firebase Realtime Database and update the `databaseURL` in `receiver.ino` and `dbUrl` in `index.html`.
4. **Web UI:** Open `index.html` in a modern web browser to access the tracking dashboard.
