void dataSD(String dataString){
  File dataFile = SD.open("data.csv", FILE_WRITE);
  dataFile.println(dataString);
  dataFile.flush();
  Serial.println("Printed to SD");
}

void logSD(String logString){
  File logFile = SD.open("log.txt", FILE_WRITE);
  logFile.println(logString);
  logFile.flush();
  Serial.println("Printed to SD");
}

String dataLogging(){
  String falling = "0";
  if(isFalling()){
    falling = "1";
  }

  String bdry = "1";
  if(!inBdryBox()){
    falling = "0";
  }

  String nichrome = "0";
  if(nichromeStarted && !nichromeFinished){
    nichrome = "1";
  }
  else if(nichromeStarted && nichromeFinished){
    nichrome = "2";
  }
  
  String dataString = (String)millis() + ","
                    + imuData + ","
                    + (String)longit + "," 
                    + (String)lat + "," 
                    + (String)alt + ","
                    + falling + "," 
                    + bdry + ","
                    + nichrome;
  
  return dataString;
}

