#include <ESP8266WiFi.h>
#include <ESP8266Firebase.h>
#include <Servo.h>

// Insert Firebase project API Key
#define API_KEY "AIzaSyAOTAj15vDHmsF2Dq4AhyUmDJZcs-0Q3I8"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://iotsproj-a8053-default-rtdb.asia-southeast1.firebasedatabase.app/"

Firebase firebase(DATABASE_URL);

// WiFi credentials
const char* ssid = "21Savage"; //set this to your own wifi SSID and password
const char* pass = "ganggang21";

#define IRPIN 14 // D5
#define SERVOPIN 16
#define BUZZER 4

Servo door;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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
  door.attach(SERVOPIN);
  pinMode(BUZZER,OUTPUT);

}

void loop() {
  if(firebase.getInt("test/lock_status")==1){
  CheckDoor();
  }
}

void CheckDoor() {
  door.write(0); // open after allowed access
  delay(2000);// wait for ppl enter 
x`
  if (digitalRead(IRPIN) != 0) {
    delay(9000);
    if (digitalRead(IRPIN) == 1) {
      while (digitalRead(IRPIN) == 1) {
        tone(BUZZER, 1000);
        delay(500);
        noTone(BUZZER);
        delay(500);
      }
    }
  }
  door.write(90);
  firebase.setInt("test/lock_status", 0); 
  delay(2000);//close
}