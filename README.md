# RobLockX
Done by: Tan Ming Yao Abel, Ong Jovier, Lim Zhiyu, Ang Wei Xiang, Muhamad Zufar bin Zakir

This system contains the following components:
1. ESP32 Authentication Board
2. ESP8266 Lock Control Board
3. ESP8266 Environmental Monitoring Board
4. Monitoring Mobile App

## Features
ESP32 Authentication Board:
1. RFID, Fingerprint, Keypad (Passoword) authentication (in order)
2. Compares with existing user records in Firebase
3. Alerts on Telegram whenever authentication failed, door locking and unlocking
4. Logging on AWS for the events mentioned above

ESP8266 Lock Control Board:
1. Controls the lock system upon successful authentication
2. Buzzer system with IR sensor to alert if the door has been left open for too long

ESP8266 Environmental Monitoring Board:
1. Logging of temperature, humidity, light levels on ThingSpeak securely

Mobile app :
1. Used for live feed monitoring via ESP-Cam
2. Shows the current tempoerature, humidity, light level

## Setting up the system
The code is useable without any further configurations with the exception of the user registration

The system was done with the intention that there is an admin staff registering users in the Firebase database and manual registration of fingerprints

If any reproduction is needed, fields for Firebase, ThingSpeak and Telegram shall be replaced with the user's own ones

