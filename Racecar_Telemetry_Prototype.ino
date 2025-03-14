/* 
  Version 1

  Recording telemetry and data to a csv file on an SD card.
  Intended use on E30
*/

#include <SPI.h>
#include <SD.h>
#include<string.h>

Sd2Card card;
SdVolume volume;
SdFile root;

//Chip select
const int chipSelect = 10;

//Sensor pins
int accelSensor = A0;
int brakeSensor = A1;

int accelData = 0;
int brakeData = 0;

String labelOne = "Accelerator";
String labelTwo = "Brake";
String labelThree = "Time";

//Telemtry on and file creation
bool telemetryOn = false;
bool fileCreated = false;
File myFile;

//Counts for filtering system
int onCount = 0;
int offCount = 0;
int filterAmount = 5;
int filterAmountON = 10;

//Counts for time
double startTime = 0;
double elapsedTime = 0;

//Switch setup
int switchSensor = 2;
int switchState = 0;


void setup() {
  Serial.begin(9600);

  // Pin setup
  pinMode(accelSensor, INPUT);
  pinMode(brakeSensor, INPUT);
  pinMode(switchSensor, INPUT);


  while (!Serial){
    ; // wait for serial port connection
  }

  Serial.print("\nInitializing SD card...");

  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!SD.begin(chipSelect)){
    Serial.println("\nInitialization failed or card not inserted");
    return;
  }
  Serial.println("\nCard initialized");
  
}

void loop() {
  while (telemetryOn){
    if (fileCreated == false){
      //File name creation and file count check
      String fileName = fileNameCreate();

      //Open file for writing
      myFile = SD.open(fileName.c_str(), FILE_WRITE);

      //Test file function
      fileCheck(myFile, fileName);

      fileCreated = true;

      myFile.print(labelThree);
      myFile.print(",");
      myFile.print(labelOne);
      myFile.print(",");
      myFile.print(labelTwo);
      myFile.print("\n");

      startTime = millis();

    }

    // Read data and print to slected space
    accelData = analogRead(accelSensor);
    brakeData = analogRead(brakeSensor);
    elapsedTime = (millis() - startTime) / 1000; // Display seconds

    myFile.print(elapsedTime); /////////////---------> Convert to myFile.print for printing to file and then check output
    myFile.print(",");
    myFile.print(accelData);
    myFile.print(",");
    myFile.print(brakeData); 
    myFile.print("\n");
    
    switchState = digitalRead(switchSensor); // Check switch status

    if (switchState == LOW){ // Switch filtering
      offCount++;
      Serial.println("Shutting Down");
    } else {
      //Serial.println("On"); //------> For testing purposes
      offCount = 0;
    }

    if (offCount >= filterAmountON){// Use filter to shutdown telemetry
      telemetryOn = false;
    }

    delay(20);
  }
  myFile.close();
  fileCreated = false;


  switchState = digitalRead(switchSensor); // Check switch status

  if (switchState == HIGH){// Switch filtering
    onCount++;
    Serial.println("Initialising...");
  } else {
    onCount = 0;
    //Serial.println("Off"); //-------> For testing purposes
  }

  if (onCount >= filterAmount){//Use filter to start telemetry and file creation
    telemetryOn = true;
  }

  delay(1000);

}

///////////////////////////////////// Below is user made functions \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//

String fileNameCreate(){
  String fileDefault = "data";

  File root = SD.open("/");
  int amountFiles = countFiles(root);
  root.close();

  Serial.println("Amount of files: ");
  Serial.println(amountFiles);
  amountFiles++;
  String fileName = fileDefault + String(amountFiles) + ".csv";

  Serial.println(fileName);

  return fileName;
}


void fileCheck(File fileMyFile, String nameFile){
    if (fileMyFile){ // Check it opened and then write to
    Serial.println("\nFile exists. Now writing to file");

    //fileMyFile.println("Testing 123...");
  } else {
    Serial.println("\nFile did not open.");
  }

  /*
  fileMyFile = SD.open(nameFile.c_str(), FILE_READ);

  if (fileMyFile){
    while (fileMyFile.available()){
      char c = fileMyFile.read();
      Serial.print(c);
    }
    fileMyFile.close();
  } else {
    Serial.println("Error opening file");
  }
  */
}


int countFiles(File dir) {
  int count = 0;

  while (true) {
    File entry = dir.openNextFile();
    if (!entry) { //no more files
      break;
    }
    if (!entry.isDirectory()) {
      count++;
    }
    entry.close();
  }

  return count;
}
