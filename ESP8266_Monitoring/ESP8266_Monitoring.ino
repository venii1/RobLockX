#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>

// ThingSpeak fingerprint
const char* fingerprint = "96:13:d2:f9:1b:b6:ef:9f:9e:ed:5d:43:b3:e6:1e:52:a3:ea:16:ac"; // this is the correct value from iplocation.io

// WiFi credentials
const char* ssid = "Jovier"; //set this to your own wifi SSID and password
const char* pass = "balls1234";

// DHT sensor settings
#define DHTTYPE DHT22
#define DHTPIN 13 // D7 which is GPIO13 for NodeMCU

// Initiate DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Variables for storing data
float Temperature;
float Humidity;
int LDRValue, field6;
unsigned long lastMillis = millis();
unsigned long serialMillis = millis();

// ThingSpeak credentials
int channelID = 2601547;  // Your ThingSpeak channel ID
String writeAPIKey = "G8CUO2ONNFZBBNP9";  // Your WriteAPIKey for the channel
String readAPIKey = "HZ65LY1R18ORL8EI";
const char* server = "https://api.thingspeak.com/update";
String field6Url = "http://api.thingspeak.com/channels/2601547/fields/6/last.json?api_key=HZ65LY1R18ORL8EI";
String jsonBuffer;

WiFiClientSecure client; 

void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Serial.begin(115200); 
  delay(100);
  dht.begin();
  Serial.print("Trying to Connect with ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP Address: ");
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
  ThingSpeak.begin(client);
  
}

void loop() {
  lastMillis = millis(); 
  serialMillis = millis();
  
  while ((millis() - lastMillis) < 45000) {
    serialMillis = millis();
    while ((millis() - serialMillis) < 1000);
    if ((millis() - serialMillis) >= 1000) {
      field6 = getfield6();
      if (field6 == 1) {
        Serial.println(field6);
        digitalWrite(2, HIGH);
        Serial.println("PIN 2 IS " + digitalRead(2));
        } else {
          digitalWrite(2, LOW);
        } 
    }
  }
  writeData();
}

void writeData() {
  Temperature = dht.readTemperature();
  Humidity = dht.readHumidity();
  LDRValue = analogRead(A0);
  // Get the values of the humidity
  if (isnan(Temperature) || isnan(Humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Write to the ThingSpeak channel
  if ((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    client.setFingerprint(fingerprint);
    http.begin(client, server);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = "api_key=" + writeAPIKey;
    httpRequestData += "&field1=" + String(Temperature);
    httpRequestData += "&field2=" + String(Humidity);
    httpRequestData += "&field3=" + String(LDRValue);

    int httpCode = http.POST(httpRequestData);
    if (httpCode == 200) {
      Serial.println("Channels update successful.");
    } else {
      Serial.println("Data upload failed .....");
      Serial.println("Problem updating channel. HTTP error code " + String(httpCode));
    } 
    http.end();
  }
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