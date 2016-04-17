void initIMU(){
    
  /* Initialise the sensors */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  if(!gyro.begin())
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Serial.print("Ooops, no L3GD20 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
}

void imuStuff(){
    /* Get a new sensor event */
  sensors_event_t event;
  sensors_vec_t   orientation;
   
  /* Display the results (acceleration is measured in m/s^2) */
  accel.getEvent(&event);
  //Serial.print(F("ACCEL "));
  currX = constrain (event.acceleration.x, -9.8, 9.8);
  currY = constrain (event.acceleration.y, -9.8, 9.8);
  currZ = constrain (event.acceleration.z, -9.8, 9.8);
  Serial.print("X: "); Serial.print(currX); Serial.print("  ");
  Serial.print("Y: "); Serial.print(currY); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");

  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  mag.getEvent(&event);
  //Serial.print(F("MAG   "));
  //Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  //Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  //Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");
  if (dof.magGetOrientation(SENSOR_AXIS_Z, &event, &orientation))
    {
      Serial.println(orientation.heading);
      magnetDirection = orientation.heading;
    }

  /* Display the results (gyrocope values in rad/s) */
  gyro.getEvent(&event);
  //Serial.print(F("GYRO  "));
  //Serial.print("X: "); Serial.print(event.gyro.x); Serial.print("  ");
  //Serial.print("Y: "); Serial.print(event.gyro.y); Serial.print("  ");
  //Serial.print("Z: "); Serial.print(event.gyro.z); Serial.print("  ");Serial.println("rad/s ");  

  //Serial.println(F(""));

  delay(100);
}

