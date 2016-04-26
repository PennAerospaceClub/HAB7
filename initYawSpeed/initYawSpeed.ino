#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>
#include <VarSpeedServo.h>
#include <Math.h>
#include <SoftwareSerial.h>

/* Assign a unique ID to the sensors */
Adafruit_9DOF                dof   = Adafruit_9DOF();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

#define SERVOPINY 11
#define SERVOPINX 10
#define SERVOPINZ1 9
#define SERVOPINZ2 6
const float pi = 3.14159265358;

VarSpeedServo myservoY; VarSpeedServo myservoX;
VarSpeedServo myservoZ1; VarSpeedServo myservoZ2;
SoftwareSerial port1(3,5);

int goalDirection = 90; //0 to 359  //updated every time IMU receives data
int currLat;    //Format (8 bit int): 00000000; xx'xx.xxxx"
int currLong;
int phillyLong; //preset.
int phillyLat;  //preset.
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
  Serial.begin(9600);
  port1.begin(9600);
    
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

  //serial
  String printthis = String(magnetDirection, 2) + "," + String(currX, 2) + "," +
                                                        String(currY, 2) + "," +
                                                        String(currY, 2) + + ",#";
  if (port1.isListening()) {
    port1.println(printthis);
  }

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
  if (thisDemand > 0) {
    thisDemand = 360 - thisDemand;
    zservos();
  }
  else {
    zservos();
  }


}

void zservos () {
  if (abs(last1Demand + last2Demand - thisDemand) > acceptableRange) {


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
    else if (thisDemand >= 180 && thisDemand < 360) {
      firstSpeed = (int)(abs(180 - last1Demand) * 0.86 + 100);
      secondSpeed = (int)(abs(thisDemand - 180 - last1Demand) * 0.86 + 100);

      myservoZ1.write(180, firstSpeed, false);
      myservoZ2.write(thisDemand - 180, secondSpeed, false);

      last1Demand = 180;
      last2Demand = thisDemand - 180;
    }
  }
}



/*
   Computes the absolute heading, where W = 0 degrees, based on current location using the haversign formula.

   @returns: heading in 00000000 format
   params:
   - phillyLat: Latitude of Philly (00000000)
   - phillyLong: Longtitude of Philly (00000000)
   - currLat: current latatitude of baloon (00000000)
   - currLong: current Longtitude of balloon (00000000)
*/
int updateGoalDirection(int phillyLat, int phillyLong, int currLat, int currLong) {
  double lat1, long1, lat2, long2;
  double b;     //bearing from current location to philly;
  
  lat2 = degMinsToRad(phillyLat);
  long2 = degMinsToRad(phillyLong);
  lat1 = degMinsToRad(currLat);
  lat2 = degMinsToRad(currLong);

  // note the return of atan2 is from -pi to pi radians. Furthermore, the bearing assumes that north is 0 degrees
  b = atan2(cos(lat2) * sin(long2-long1), cos(lat1) * sin(lat2) - sin(lat2 - lat1) * cos(lat2) * cos(long2 - long1));

  b = radToDegs(b);   //convert radian result to degrees (-180 -- 180 degrees)
  b = (int) (b + 360) % 360; //convert degrees from (-180 -> 180 degrees to 0 -> 360 scale, where 0 is North);
  b = (int) (b + 90) % 360; //convert from 0 is North to 0 degrees is W.
  
  return b;
}


/*
   Convers degrees minutes to radians

   Param:
   - coord: the coordinate to be convereted, in the format: 00000000 xx'xx.xxxx"
*/
double degMinsToRad (int coord) {
  //coord format: 00000000:  xx'xx.xxxx"
  int deg;    // degrees
  double degDecimals;   //degrees
  int mins;   // minutes
  double radResult;

  deg = coord / 1000000;  //extract first two digits of of coordinate
  mins = coord - (deg * 1000000);   //extract last 6 digits of coordinate
  degDecimals = (double) deg + (double) mins / 60.0;   //convert degMinutes to degrees.
  radResult = (degDecimals * pi) / 180.0;
  return radResult;
}

/*
 * converts radian to degrees (not degrees. minutes)
 * 
 * @return: degree equivalent of radian input
 * @param:
 * - rads: radians
 */
double radToDegs (int rads) {
  return (rads * 180) / pi;
}

