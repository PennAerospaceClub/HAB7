//Maximum Runtime ~50 days due to millis() resetting

#include <Wire.h>
#include <Time.h>
#include <Adafruit_SI1145.h> //UV
#include <Servo.h> //TODO: Update to slow servo speed
#include <SD.h> //SD
#include <SPI.h> //SD

//Data
String imuData = "";

//Position
//Boundary Box UPDATE DAY OF LAUNCH WITH MOST RECENT SIMULATION
const long int minLat = 39000000;       //xx°xx.xxxx' 40010950 //39596216
const long int maxLat = 40000000;       //40337066             //40297450
const long int minLong = 75000000;      //75462700             //76208483
const long int maxLong = 76000000;      //77044366             //77093716
const long int maxWantedAlt = 10000; //Maximum wanted altitude (BDRY) 31000

//Initialize Location Data
long int lat = -1; 
long int longit = -1;
long int alt = -1; //altitude in meters
long int maxAlt = 0; //measures in meters

//GPS Utility
String field = "";
boolean gooddata = false;
unsigned long int gpsMaxSearchTime = millis() + 5000;

//SD
int CSPIN = 53;

//Sanity
boolean sane = false;
boolean inBdry = false;
boolean falling = true;
boolean initSD = false;


//LED
const int LED_GREEN = 10;
const int LED_YELLOW = 9;
const int LED_RED = 8;
//Blinking lights
boolean onLED = false;

//Timing (second and 5 second writes)
unsigned long int nextWrite1 = 0;
unsigned long int nextWrite5 = 0;

//Nichrome
boolean servoRan = false;
const int NICHROME_PIN = 11;
boolean nichromeStarted = false;
boolean nichromeFinished = false;
unsigned long int nichromeEndTime = 0;
int nichromeCounter = 0;

//Servo
const int SERVO_PIN = 12;
Servo valve;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); //GPS
  Serial2.begin(9600); //arduino
  
  //LED
  pinMode(LED_GREEN, OUTPUT); digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_YELLOW, OUTPUT); digitalWrite(LED_YELLOW, HIGH);
  pinMode(LED_RED, OUTPUT); digitalWrite(LED_RED, HIGH);

  //Nichrome
  pinMode(NICHROME_PIN, OUTPUT);
  digitalWrite(NICHROME_PIN, LOW);

  //Servo
  valve.attach(SERVO_PIN); //Servo that controls valve. 

  //End Setup
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);

  nextWrite5 = millis() + 5000;
  nextWrite1 = millis() + 1000;
}

void loop() {
  while(!sane){
      sane = initSane();
      readGPS();
      Serial.print(lat); Serial.print(", "); Serial.print(longit); Serial.print(", "); Serial.println(alt); 
  }

  readGPS();
  nichromeCheck();
  //servoCheck();

  if(Serial2.available()) {
    delay(10);
    imuData = Serial2.readStringUntil('#');
  }
  
  //Time Controlled: SD, Serial LED
  if(millis() >= nextWrite5){
    String dataString = dataLogging();
    dataSD(dataString);
    Serial.println(dataString);
    
    nextWrite5 = millis() + 5000;
  }

  if(millis() >= nextWrite1){
    if(onLED){
      digitalWrite(LED_GREEN, LOW);
      onLED = false;
    }
    else{
      digitalWrite(LED_GREEN, HIGH);
      onLED = true;
    }
    nextWrite1 = millis() + 1000;
  }
}


