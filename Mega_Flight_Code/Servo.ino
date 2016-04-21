void servoCheck(){
  if(nichromeStarted && nichromeFinished){
  //if(millis() > 15000 && !servoRan){
    releaseServo.write(170);
    delay(1000);
    servoRan = true;
    Serial.println("Servo");
  }
}

