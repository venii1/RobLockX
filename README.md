# RobLockX
Done by: Abel, Jovier, Zhiyu, Wei Xiang, Zufar

## Description
RobLockX provides a multi-factor authentication with event and enviromental logging. It provides authentication in very critical cases like records storage at police facilities.  

Features:
1. Authentication using a RFID, Passcode and Fingerprint via checking against a user database on Firebase.
2. Automated lock control whenever authentication is successful
3. It provides real-time app monitoring with a camera feed and environmental sensor data
4. Authentication events logging on Telegram (To the security guard on duty) and AWS IoT Core (For centralized logging management)

Components:
1. ESP32 Authentication Board
2. ESP8266 Lock Control Board
3. ESP8266 Environmental Monitoring Board
4. Monitoring Mobile App + ESP-Cam

## Sections of this Repository
* [Setting up the system](#setting-up-the-system)
  * [Prerequisites](#prerequisities)
  * [Board Dependencies](#board-dependencies)
  * [Libraries Required](#libraries-required)
  * [Connecting the System](#connecting-the-system)
    * [ESP32](#esp-32)
    * [ESP8266 Lock Control](#esp8266-lock-control)
    * [ESP8266 Environmental Monitoring](#esp8266-environmental-monitoring)
    * [ESP-CAM](#esp-cam)  
  * [Utilizing the System](#utilizing-the-system)
    * [ESP32](#esp32)
    * [ESP8266 Lock Control](#esp-8266-lock-control)
    * [ESP8266 Environmental Monitoring](#esp-8266-environmental-monitoring)
    * [ESP-CAM](#esp-cam-feed)
    * [Mobile App](#mobile-app)

## Setting up the system
The system was done with the intention that there is an admin staff registering users in the Firebase database and manual registration of fingerprints via the "FingerRegister code"

### Prerequisites
1. Have an AWS account with access to AWS IoT Core
2. Firebase account with realtime database
3. ThingSpeak account
4. Telegram account with a bot made via BotFather

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

### Connecting the System
Do note for all the components VCC & GND pin shall be connected to the common 3.3V and GND pin of their respective MCUs
#### ESP 32
##### Keypad
![image](https://github.com/user-attachments/assets/5b19af69-5982-420d-ab3f-84e2f2ab446e)\
C1: GPIO 22\
C2: GPIO 4\
C3: GPIO 27\
C4: GPIO 2\
R1: GPIO 25\
R1: GPIO 26\
R1: GPIO 15\
R1: GPIO 33\
(GPIO33 is labelled wrongly on the ESP32 as GPIO23, just take note and refer to official pinout while connecting system)
##### RFID Reader
SDA: GPIO 5\
SCK: GPIO 18\
MOSI: GPIO 23\
MISO: GPIO 19\
RST: GPIO 0
##### Fingerprint Scanner
Tx: GPIO 14\
Rx: GPIO 12
##### PIR Sensor
Data: GPIO 32
#### ESP8266 Lock Control
##### Buzzer
Signal: GPIO 4
##### Servo Motor
Signal: GPIO 16
##### IR Sensor
OUT: GPIO 14
#### ESP8266 Environmental Monitoring
##### DHT Sensor
Data: GPIO 13
##### LDR
![image](https://github.com/user-attachments/assets/c20f3b4c-70ff-4f88-88c0-bae001fcdda9)
#### ESP CAM
To be connected to a Serial-to-USB module (CP2102 was used in our case)\
U0R: Connect to CP2102 Tx Pin\
U0T: Connect to CP2102 Rx Pin\
5V and GND of each component to be connected to each other
* The CP2102 shall be plugged into the laptop where the ESP-CAM code is being ran

### Utilizing the system
Each system will has their own folder within the repository here
All relevant libraries stated above should be installed before 
#### ESP32
1. Run "ESP32_FingerRegister" to register fingerprint and remember what ID it is stored as for administrative purposes of making new users in the database
2. Run "final_authentication" to start the main program of authentication
3. Users may use their own Firebase, AWS and ThingSpeak chat ID and certificates
   * For Firebase, refer to this GitHub repo by Rupak Poddar and its example codes:  
      * https://github.com/Rupakpoddar/ESP32Firebase  
   * For AWS, refer to this link to setup:  
     * https://how2electronics.com/connecting-esp32-to-amazon-aws-iot-core-using-mqtt/  
   * For Telegram bot message logging, refer to this tutorial:  
     * https://www.electronicwings.com/esp32/send-a-telegram-message-using-esp32
#### ESP 8266 Lock Control
1. Run "8266Door" code
2. Users may use Serial Communication for ESP32 to enable the locking system or by using a commoen field in a database like what was done here as a form of communication
3. Refer to this guide by Rupak Poddar to setup, read and write to your own Firebase via ESP8266: https://github.com/Rupakpoddar/ESP8266Firebase

#### ESP 8266 Environmental Monitoring
1. Run "ESP8266_Monitoring" to start the system
2. Users may use their own ThingSpeak channelID and WriteAPIKey to write to their own ThingSpeak channels
   * The "Fingerprint" field should remain constant (unless ThingSpeak updates it, use this webpage to check for the fingerprint of the thingspeak.com domain: https://iplocation.io/ssl-certificate-fingerprint)
#### ESP Cam Feed
1. Run "ESP-CAM.ino" inside the ESP-CAM folder
2. The "camera_index.h", "camera_pins.h" & "app_httpd.cpp" MUST be in the same file location as the ESP-CAM.ino before running the code
3. Refer to the "ESP32 CAM Setup Guide" PDF provided for a step-by-step guide
4. The laptop running the Camera should be connected in the same subnet as the mobile app for monitoring (i.e. Mobile HotSpot in our case)
#### Mobile App
1. The .aia file was provided
2. User can change to own ThingSpeak by importing .aia file into MIT App Inventor to customize the code
3. The IP address of the ESP-CAM obtained shall be entered into the "IP address" field of the app









