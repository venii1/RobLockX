#include <Adafruit_Sensor.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Firebase.h>
#include <WiFi.h>
#include <Adafruit_Fingerprint.h>
#include <UniversalTelegramBot.h>
#include <Keypad.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#define BOTtoken "7407922049:AAF9pOst42PPk_GBWnHVSHayXJ0O8i8-6j8"  // your Bot Token (Get from Botfather)
#define CHAT_ID "712007697"
#define AWS_IOT_PUBLISH_TOPIC   "RobLockX/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "RobLockX/sub"
#define THINGNAME "RobLockx"
#define _SSID "Jovier"          // Your WiFi SSID
#define _PASSWORD "balls1234"      // Your WiFi Password
#define REFERENCE_URL "https://iotsproj-a8053-default-rtdb.asia-southeast1.firebasedatabase.app/"  // Firebase Project Realtime Database reference URL

#define SS_PIN 5
#define RST_PIN 0

WiFiClientSecure teleClient;
UniversalTelegramBot bot(BOTtoken, teleClient);
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

String field6Url = "http://api.thingspeak.com/channels/2601547/fields/6/last.json?api_key=HZ65LY1R18ORL8EI";
String jsonBuffer;

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 28800;
const int daylightOffset_sec = 0;
const char AWS_IOT_ENDPOINT[] = "azem33yau4w2x-ats.iot.us-east-1.amazonaws.com";

// Amazon Root CA 1
static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";


// Device Certificate    
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAILrpCcCCZ/zBASbaJrfPzxIbFhpMA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNDA3MjUwMjM5
NTNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDJzxZ3Z3CBMT6FzbdJ
0M5oakWtG78+yx6dKZUF7lHZcp4G/drqLF5+PGZVD7Ieh9APh45UX1W3U+UFTgsj
2wYc9h310UGwJvvoyApZb94QDccxF9c1X12xjGaoP2/ckWywjPaLidxw6mUXqGMx
Djb8GHB1YBkqg3EtnmQPf19lqMxgFm74u21nZhcZI3C/4/BGgOfP76e43aoktnu+
Vpn98NFAWwUujvRcmtp9KoDpsjuFM8xq71Z9Ry+aGwinNGMVArAiJGJxt1A4y+rt
dUftN2+/7ZfH3RGIc2IE+BETFSGr56PIL1T0/4HmsmnsuigIi/Ei/KbyZhMRwowp
asAxAgMBAAGjYDBeMB8GA1UdIwQYMBaAFNL7yeeNt8QKEi6yJUiDH/mNGqm2MB0G
A1UdDgQWBBQ2DUjM8WXElFyx69BeBbKRVrEh0DAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAT7IKWQYx2lTAo35Omx1ru16+
0W9NnPhaiVSjAOvfiY67u8dnv5iywqChmjyeXsS/06Upl4+kl/8PTvF/QrLg5AF/
RvG63s7Ip/pa1cKlVB4YUIPneKJByt7aQ+YclEEdPNLUlGecpTqtgWjoS18xkOls
aZXQxt2QgjmRSwI3dtGtrDddFSx6VXR6Gwg30wUy3n6RZR/jJRtlBIksjULye2xv
BZCaxgY+fe6NsmIUlUU7IeWctR4qG7Ts+6b4JA6+my44GRCgKU+By6+06xoK+HYx
H1gz7VgJG1zpREOpVkr+Kz3gb0Fef14kYyx3bJJ0Wkm9T/CCNmxOyG1fRHkFOw==
-----END CERTIFICATE-----
)KEY";

//Device's private key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAyc8Wd2dwgTE+hc23SdDOaGpFrRu/PssenSmVBe5R2XKeBv3a
6ixefjxmVQ+yHofQD4eOVF9Vt1PlBU4LI9sGHPYd9dFBsCb76MgKWW/eEA3HMRfX
NV9dsYxmqD9v3JFssIz2i4nccOplF6hjMQ42/BhwdWAZKoNxLZ5kD39fZajMYBZu
+LttZ2YXGSNwv+PwRoDnz++nuN2qJLZ7vlaZ/fDRQFsFLo70XJrafSqA6bI7hTPM
au9WfUcvmhsIpzRjFQKwIiRicbdQOMvq7XVH7Tdvv+2Xx90RiHNiBPgRExUhq+ej
yC9U9P+B5rJp7LooCIvxIvym8mYTEcKMKWrAMQIDAQABAoIBAC9VBdTtiuKSNTLW
w/VpJvAwdLUcO+ezZjPAS/Z/Je1VR6EmKxEfZO5AplOvAbzNZ8wspAI/9e+wmpd6
7Xv0Y+OpCe/cq4ZwLkczm9HMPg4P3Tcze0NGYchBwNYXQto9tXtXJIAnzm8hWqKI
2PLpfZX0+XNeHajt+T6U1q0X170UxlhgSjxp0edZ1ghElj3CMrTq62/OlyGKBxwf
8Xl7q3pl0BFDvQiDPPNKyLyyXxHF8K/WdupnyQTWqs7AcsrqrDQGYUSc0ETxav0h
pED+hKUG3kqq88EYi3aJjEoh0ANzQf15gSphB3sf+6FzBb6oFbspVT1NEqb9jv1d
ssctzv0CgYEA7avdaElFHD4TQiHRhZOFmes/fuyR2LDyDo6DG28UCa3YoOuZDlY5
z0bL1/PfPiuypC6E7/Kr7Qq3IV5jja7Z7RxfUAOUhwZT2ZbKYadM3yDLQE2H7X8b
BNGZKI5TW66L57/hAe8wYha3Y0uSYKbrkl5SGVJUdH7b8XVOBJVpoSMCgYEA2V85
GWTDxd//QIVKlQM5VuMynUnb+Cp+MSTMNO7+Q2Zwt7dXHn7s6WtJAWEN6JZxd/b7
fmfuFSXBm/3OicDipQailnsHFyF64ZBTJ5meWPvuFkXKq0yiMk16S8n7ZOEb5a/P
j5zV8nMIvFX2uq8vMTuWYwE701GCq5jQINZMEJsCgYBTSHc0VQB/rgznjpwx94jI
b9P3MQTGbvLTQQMWOO1HmwCfGw+uI27sgD1r9gwTaBKYT6jLwDiYvrl/Wuj9u/in
itqnWinQytnVFG7km1Bnm8E9Ti3EflbEcIH/4Se6p8Zktistl1hX9ufaRqALeFJy
HtUcBGqNy+pciSOGnai/CwKBgGXHXlbzhW87SKsPPjKzhb+XNfXLxYFXdCrSjgMi
/E7WwmRjJhmTWJ6aq15H8Vk8R/jh3jP7udCREyxVFMYPWI/ooWxQQquj84N2LllW
ZnxHyd0H5LN8eF2RRXgDJQDBHmzXexpFVy/kiOyWW6ZhrHki63GD/F0fx01NifY2
IPuNAoGBAMADxvHJ54M8Dr70kRjBVXCfT7O3x3oXe1d2uPndjQ92CHs6VLguLEso
r5eDcVce1JFHNcy+ppYVUiD8/P9twcyx/fydWNXtcBr7jqlu4lvOkhFPtwXP4snM
LlvQQ6aTIJkZrHdYvBKW/cIdUHTbjH7mc9Ye+pgBWS4XgJaaoI9r
-----END RSA PRIVATE KEY-----
)KEY";

// Fingerprint stuff
volatile int finger_status = -1;
HardwareSerial mySerial(1); // TX/RX on fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 
String RFIDval;
int RFIDstatus;
int keypadStatus;
int FingerprintStatus;
String enteredPass;
// Init array that will store new NUID 
byte nuidPICC[4];

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
 {'1','2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}
};
byte rowPins[ROWS] = {25,26,15,33}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {22,4,27,2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
char customKey;
char message;
//Link Firebase Reference URL of Realtime Database
Firebase firebase(REFERENCE_URL);
int UserID;
int NumOfRecords; //Number of RFID registered
String UserPasscode; //User's passcode from Firebase
String UserRFID; //User's RFID

unsigned long now = millis();
unsigned long previousMonitor = 0;

void IRAM_ATTR ISR_1() {
  bot.sendMessage(CHAT_ID, "Lockdown was triggered!", "");
  while(1){
    firebase.setInt("test/lock_status", 0);
    Serial.println("Lockdown!");
    if (getfield6() == 0){
      return;
    }
    delay(1000);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(32, INPUT);
  attachInterrupt(13, ISR_1, HIGH);
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  teleClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  //Fingerprint stuff
  Serial.println("");
  Serial.println("\n\nAdafruit Fingerprint sensor in verification mode");
  // Initialize Serial1 with GPIO14 as RX and GPIO12 as TX
  mySerial.begin(57600, SERIAL_8N1, 14, 12);

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); 
  Serial.print(finger.templateCount); 
  Serial.println(" templates");
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  Serial.println("ESP32 Ready");
}

void loop(){
  if(digitalRead(32) == 1){ 
    //Code for the authentication
    Serial.println("Motion detected, activating system");

    for(int i =0; i <3; i++){
      RFIDstatus = readRFID();
      if(RFIDstatus == 10){
        break;
      } else if (RFIDstatus == -1) {
        timeoutAuth();
        break;
        }
      }
      delay(1000);

    if(RFIDstatus == 10){
      Serial.println("Correct RFID");
      for(int g = 0; g < 3; g++){
        keypadStatus = readKeypad();
        if(keypadStatus == 10){
          break;
        } else if (keypadStatus == -1) {
          timeoutAuth();
          break;
        }
      }
    }

      if(keypadStatus == 10){
        Serial.println("Correct Passcode");
        Serial.println("Please place your finger on the fingerprint");
        delay(3000);
        for(int x = 0; x < 3; x++){
          FingerprintStatus = readFingerprint();
          if(FingerprintStatus == 10){
            break;
          } else if (FingerprintStatus == -1) {
            timeoutAuth();
            break;
          }
        }
      }
        if (FingerprintStatus==10){
          lockSystem();
          client.loop();
        }

      if(keypadStatus == 0 || RFIDstatus == 0 || FingerprintStatus == 0){
        Serial.println("Not Employee");
        bot.sendMessage(CHAT_ID, "Authentication just failed", "");
        publishAwsMessageFailed();
      } else if (keypadStatus == -1 || RFIDstatus == -1 || FingerprintStatus == -1) {
        Serial.println("Timed Out");
        bot.sendMessage(CHAT_ID, "Authentication just timed out", "");
        publishAwsMessageTimeOut();
      }
  } 
  client.loop();
}

void timeoutAuth() {
  RFIDstatus = -1;
  keypadStatus = -1;
  FingerprintStatus = -1;
}

int readRFID(){
  unsigned long rfidMillis = millis();
  unsigned long rfidTimeout = 10000;

  while ((millis() - rfidMillis) < rfidTimeout) {

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  while ( ! rfid.PICC_IsNewCardPresent()) {
      if ((millis() - rfidMillis) >= rfidTimeout) {
        return -1;
      }
      
  }

  // Verify if the NUID has been readed
  while ( ! rfid.PICC_ReadCardSerial())
    yield();

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return 0;
    }
  RFIDval ="";
  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
    RFIDval += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "") + String(rfid.uid.uidByte[i], HEX);
    }
  Serial.println(RFIDval);

  // //Iterating through database to check if RFID matches any of the Users
  NumOfRecords = firebase.getInt("Users/Number_of_users");
  Serial.println("Getting num users");
  for (UserID = 1; UserID <= NumOfRecords ; UserID++){
    Serial.println("Finding records");
    UserRFID = firebase.getString("Users/User_"+String(UserID)+"/RFID");
    delay(100);
    if (UserRFID == RFIDval){
      Serial.print("RFID Matched:\t");
      Serial.println(UserRFID);
      rfid.PICC_HaltA();
      // Stop encryption on PCD
      rfid.PCD_StopCrypto1();
      return 10;
      }
    }

  Serial.print("Was not able to match RFID to a user!");
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  return 0;
  
  /* Format for reading of data
  data = firebase.getString("Directory/Key");
  Serial.print("Received Data:\t");
  Serial.println(data);
  */
  }
  return -1;    
}



int readKeypad(){
  enteredPass = "";
  for(int i =0; i < 8; i++)
  {
    unsigned long keypadMillis = millis();
    unsigned long keypadTimeout = 10000;
    char customKey = customKeypad.getKey();
    while(customKey == NO_KEY) {
      yield();  // remove this if it got problem need settle at pioneer mall 4pm (crimewatch reference)
      customKey = customKeypad.getKey();
      if ((millis() - keypadMillis) >= keypadTimeout) {
      return -1;
      }
    }
    Serial.print(customKey);
    enteredPass += customKey;
  }

  UserPasscode = firebase.getString("Users/User_"+ String(UserID) +"/Passcode");
  if(enteredPass == UserPasscode){
    return 10;
  }
  else {
    return 0;
  }
}

int readFingerprint(){
  unsigned long fingerprintMillis = millis();
  unsigned long fingerprintTimeout = 10000;
  while ((millis() - fingerprintMillis) <= fingerprintTimeout) {
  String UserFingerprint = firebase.getString("Users/User_"+ String(UserID) +"/Fingerprint");
  finger_status = getFingerprintIDez();
  if (finger_status == -2) {
    return 0;
    Serial.println("Not Match!");
  } else if (finger_status != -1 && finger_status != -2) {
    if (String(finger_status) != UserFingerprint) { 
      Serial.println("Not in database!");
      return 0;
    } else {
      Serial.println("Fingerprint matched!");  
      return 10;
    }
  }
  delay(50);  // 
  if ((millis() - fingerprintMillis) > fingerprintTimeout) {
    return -1;
    }
  }
}

// Returns -1 if failed, otherwise returns ID #
// Also, no clue whatever -2 does, but it shouldnt return -2 anyways (it was part of the example code)
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != 2) {
    Serial.println(p);
  } 
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != 2) {
    Serial.println(p);
  }
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -2;

  // Found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}



void lockSystem(){
  int check_Sent = firebase.setInt("test/lock_status", 1);
  while(check_Sent == 400){
    bot.sendMessage(CHAT_ID, "Lock Activation Communication Failed", "");
    check_Sent = firebase.setInt("test/lock_status", 1);
  }
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.setServer(AWS_IOT_ENDPOINT, 8883);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  Serial.println("AWS IoT Connected!");
  publishAwsMessageOpen();
  bot.sendMessage(CHAT_ID, "System just unlocked", "");
  delay(2000);
  while(firebase.getInt("test/lock_status")!= 0){}
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.setServer(AWS_IOT_ENDPOINT, 8883);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  Serial.println("AWS IoT Connected!");
  publishAwsMessageClose();
  bot.sendMessage(CHAT_ID, "System just unlocked", "");
}


int getfield6(){
  jsonBuffer = httpGETRequest(field6Url.c_str());
  Serial.println(jsonBuffer);
  JSONVar myObject = JSON.parse(jsonBuffer);
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return -1;
  }
  Serial.print("JSON object = ");
  Serial.println(myObject);
  Serial.print("field6 value : ");
  String field6Str = myObject["field6"];
  field6Str.replace("\"","");
  Serial.println(field6Str);
  int field6int = field6Str.toInt();
  return field6int;
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return payload;
}

void publishAwsMessageOpen()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }

  char dayDate[39];
  strftime(dayDate, 39, "%A, %B %d %Y %H:%M:%S", &timeinfo);

  StaticJsonDocument<200> doc;
  doc["DateTime"] = String(dayDate);
  doc["Status"] = "Door unlocked";
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void publishAwsMessageClose()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }

  char dayDate[39];
  strftime(dayDate, 39, "%A, %B %d %Y %H:%M:%S", &timeinfo);

  StaticJsonDocument<200> doc;
  doc["DateTime"] = String(dayDate);
  doc["Status"] = "Door Locked";
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void publishAwsMessageFailed()
{
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.setServer(AWS_IOT_ENDPOINT, 8883);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  Serial.println("AWS IoT Connected!");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }

  char dayDate[39];
  strftime(dayDate, 39, "%A, %B %d %Y %H:%M:%S", &timeinfo);

  StaticJsonDocument<200> doc;
  doc["DateTime"] = String(dayDate);
  doc["Status"] = "Authentication just failed";
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void publishAwsMessageTimeOut()
{
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.setServer(AWS_IOT_ENDPOINT, 8883);

  Serial.println("Connecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  Serial.println("AWS IoT Connected!");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }

  char dayDate[39];
  strftime(dayDate, 39, "%A, %B %d %Y %H:%M:%S", &timeinfo);

  StaticJsonDocument<200> doc;
  doc["DateTime"] = String(dayDate);
  doc["Status"] = "Authentication just timed out";
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);

  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}