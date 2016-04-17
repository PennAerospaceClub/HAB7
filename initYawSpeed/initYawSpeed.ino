#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>
#include <VarSpeedServo.h>
#include <Math.h>

/* Assign a unique ID to the sensors */
Adafruit_9DOF                dof   = Adafruit_9DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

#define SERVOPINY 11
#define SERVOPINX 10 
#define SERVOPINZ1 9 
#define SERVOPINZ2 6
VarSpeedServo myservoY; VarSpeedServo myservoX;    
VarSpeedServo myservoZ1; VarSpeedServo myservoZ2;   

int goalDirection = 90; //0 to 359
int acceptableRange = 25;
double magnetDirection = 0;

double lastYWrite = 90;
double lastXWrite = 90;
double currY = 0;
double currX = 0;
double currZ = 0;

int last1Demand = 0;
int last2Demand = 0;
int thisDemand = 0;

void setup(void) {
  Serial.begin(115200);
  myservoY.attach(SERVOPINX);
  myservoX.attach(SERVOPINY);
  myservoZ1.attach(SERVOPINZ1);
  myservoZ2.attach(SERVOPINZ2);

  myservoY.write(90);
  myservoX.write(90);
  myservoZ1.write(0);
  myservoZ2.write(0);
  
  initIMU();
  imuStuff();

}

void loop(void) {
  
  //prints IMU data to serial, updates magnetDirection
  imuStuff();

  int servospeed;
  //y
  int temp = constrain((int)(atan(currY / currZ) * 180 / PI + 90), 0, 180);
  if (abs(temp - lastYWrite) > 6) {
    servospeed = (int)(abs(temp - lastYWrite) * 0.86 + 100);
    myservoY.write(temp, servospeed, false);
    lastYWrite = temp;
  }
  
  //x
  temp = constrain((int)(atan(currX / currZ) * 180 / PI + 90), 0, 180);
  if (abs(temp - lastXWrite) > 6) {
    servospeed = (int)(abs(temp - lastXWrite) * 0.86 + 100);
    myservoX.write(temp, servospeed, false);
    lastXWrite = temp;
  }
  

  thisDemand = (int)magnetDirection - goalDirection;
  if (thisDemand > 0){
    thisDemand = 360 - thisDemand;
    zservos();
  }
  else{
    zservos();
  }
  
  
}

void zservos () {
  if (abs(last1Demand + last2Demand - thisDemand) > acceptableRange){
    
    
    int firstSpeed;
    int secondSpeed;
   
    if (thisDemand < 180 && thisDemand >= 0) {
      firstSpeed = (int)(abs(thisDemand - last1Demand) * 0.86 + 100); 
      secondSpeed = (int)(abs(last2Demand) * 0.86 + 100);
      
      myservoZ1.write(thisDemand, firstSpeed, false); 
      myservoZ2.write(0, secondSpeed, false); 

      last1Demand = thisDemand;
      last2Demand = 0;
    }
    else if (thisDemand >= 180 && thisDemand < 360){
      firstSpeed = (int)(abs(180 - last1Demand) * 0.86 + 100);
      secondSpeed = (int)(abs(thisDemand - 180 - last1Demand) * 0.86 + 100);
      
      myservoZ1.write(180, firstSpeed, false);
      myservoZ2.write(thisDemand - 180, secondSpeed, false);

      last1Demand = 180;
      last2Demand = thisDemand - 180;
    }
  }

}

