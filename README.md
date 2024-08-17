# RobLockX
Done by: Abel, Jovier, Zhiyu, Wei Xiang, Zufar

## Description
RobLockX provides a multi-factor authentication using a RFID, Passcode and Fingerprint via checking against a user database on Firebase. 

It provides real-time app monitoring with a camera feed and environmental sensor data. 

It also incorporates event logging on AWS and Telegram, and data logging of environment on ThingSpeak

This system contains the following components:
1. ESP32 Authentication Board
2. ESP8266 Lock Control Board
3. ESP8266 Environmental Monitoring Board
4. Monitoring Mobile App

## Setting up the system
The system was done with the intention that there is an admin staff registering users in the Firebase database and manual registration of fingerprints via the "FingerRegister code"

### Board Dependencies
The following should be pasted into the "Additional Board Manager URLs" under the Preferences section
```
http://arduino.esp8266.com/stable/package_esp8266com_index.json,https://dl.espressif.com/dl/package_esp32_index.json,https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
This installs the required board dependencies for ESP32 & ESP8266. including functional libraries for hardware and WiFi.

### Libraries Required
To be manually installed.

ESP32 Authentication Board:
1. Adafruit Unified Sensor (v1.1.14)
2. Adafruit Fingerprint Sensor Library (v2.1.3)
3. Arduino_JSON (v0.2.0)
4. Arduinojson (v7.1.0)
5. PubSubClient (v2.8)
6. MFRC522 (GitHub Community, v1.4.11)
7. ESP32 Firebase (Rupak Poddar, v1.0.0) 
8. UniversalTelegramBot (v1.3.0)
9. Keypad (Mark Stanley, v3.1.1)


ESP8266 Lock Control:
1. ESP8266 Firebase (Rupak Poddar, v1.3.1)

ESP8266 Environment Monitoring:
1. Adafruit Unified Sensor (v1.1.14)
2. Arduino_JSON (v0.2.0)
3. DHT sensor library (v1.4.2)
4. ThingSpeak (v1.3.3)
