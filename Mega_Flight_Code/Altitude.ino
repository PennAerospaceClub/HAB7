#define OPEN 0
#define CLOSED 90
#define DESIRED_ALT 10000 //meters
boolean toleranceZone = false; 

void altitudeControl(long alt) {
  while (alt < DESIRED_ALT){
    valve.write(CLOSED); 
  }
  if (alt >= (DESIRED_ALT - 100) && alt <= (DESIRED_ALT + 100)){
    toleranceZone = true;
    valve.write(CLOSED);
  }
  
  if (alt >= DESIRED_ALT && !toleranceZone) {
    valve.write(OPEN);
    delay(10000); // keep valve open for 10 seconds
    valve.write(CLOSED); 
  } 
}

void totalRelease() {
  
  if ((lat >= maxLat) || (lat <= minLat) || (longit >= maxLong) || 
  (longit <= minLong) || (millis()>= 10800000)){
    valve.write(OPEN);
  }
  
}
