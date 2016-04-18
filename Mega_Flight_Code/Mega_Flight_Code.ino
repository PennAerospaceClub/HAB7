//Maximum Runtime ~50 days due to millis() resetting

//IMU
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>
#include <Time.h>
#include <Adafruit_SI1145.h> //UV
#include <Servo.h> //TODO: Update to slow servo speed
//SD
#include <SD.h>
#include <SPI.h>
//Serial
#include <SoftwareSerial.h>

//UV
Adafruit_SI1145 uv = Adafruit_SI1145();

//Data
String imuData = "";
String uvData = "";

//IMU
Adafruit_9DOF                dof   = Adafruit_9DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

//Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001); //10dof

//Position
//Boundary Box UPDATE DAY OF LAUNCH WITH MOST RECENT SIMULATION
long int minLat = 39596216;//xx°xx.xxxx' 40010950
long int maxLat = 40297450; //40337066
long int minLong = 76208483; //75462700
long int maxLong = 77093716; //77044366
long int maxWantedAlt = 10000; //Maximum wanted altitude (BDRY) 31000

//Initialize Location Data
long int lat = -1; 
long int longit = -1;
long int alt = -1; //altitude in meters
long int maxAlt = 0; //measures in meters

//GPS Utility
String field = "";
String slat = "";
String slongit = "";
String salt = "";
boolean gooddata = false;
unsigned long int gpsMaxSearchTime = millis() + 5000;

//SD
int CSPIN = 53;

//Sanity
boolean sane = false;
boolean inBdry = false;
boolean falling = true;
boolean initSD = false;
boolean initUV = false;


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
Servo releaseServo;

//Solar Panel
#define SOLAR_PIN A0
int solarVal = 0;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); //GPS
  
  //LED
  pinMode(LED_GREEN, OUTPUT); digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_YELLOW, OUTPUT); digitalWrite(LED_YELLOW, HIGH);
  pinMode(LED_RED, OUTPUT); digitalWrite(LED_RED, HIGH);

  //Solar
  pinMode(SOLAR_PIN, INPUT);

  //IMU
  initIMU();
  delay(1000);

  //Nichrome
  pinMode(NICHROME_PIN, OUTPUT);
  digitalWrite(NICHROME_PIN, LOW);

  //Servo
  releaseServo.attach(SERVO_PIN);
  releaseServo.write(10);
  delay(1000);

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

  Serial.print("GPS, ");
  readGPS();
  //Serial.print(lat); Serial.print(", "); Serial.print(longit); Serial.print(", "); Serial.println(alt); 

  Serial.print("IMU, ");
  runIMU();
  //Serial.println(imuData);

  Serial.print("UV, ");
  runUV();
  //Serial.println(uvData);

  Serial.print("Nich, ");
  nichromeCheck();
  servoCheck(); //for now

  Serial.println("Solar");
  solarVal = analogRead(SOLAR_PIN);
  
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


