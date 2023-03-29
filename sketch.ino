#include <RTClib.h>
#include <SD.h>

// for sd card
#define CS_PIN 10
File root;

RTC_DS1307 rtc;

void setup() {
  Serial.begin(9600);
  


  // for rtc
  if(!rtc.begin()){
    Serial.println("RTC not found!");
    Serial.flush();
    abort();
  }

  // for sd card module
  Serial.print("Initializing SD card... ");

  if (!SD.begin(CS_PIN)) {
    Serial.println("Card initialization failed!");
    while (true);
  }

  Serial.println("initialization done.");
  Serial.println("Files in the card:");
  root = SD.open("/");
  printDirectory(root, 0);
  Serial.println("");

  // checking if a file exists or not
  if(SD.exists("tempLog.txt")){
    Serial.println("tempLog.txt exists.");
  }
  else {
    Serial.println("tempLog.txt does not exist !");
    File logFile = SD.open("tempLog.txt", FILE_WRITE);
    if(SD.exists("tempLog.txt")){
      Serial.println("tempLog.txt file created.");
    }

    // closing the file:
    logFile.close();

  }
  Serial.println("");

}

// for rtc
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// for ntc temperature sensor
const float BETA = 3950; // should match the Beta Coefficient of the thermistor

void loop() {

  // RTC here

  DateTime now = rtc.now();
  
  int year = now.year();
  int month = now.month();
  int date = now.day();
  String day = daysOfTheWeek[now.dayOfTheWeek()];
  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();

  // NTC temperature
  
  int analogValue = analogRead(A3);
  float celsius = 1 / (log(1 / (1023. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;

  char strBuf[100];

  // converting the sensor value to string
  char charCelsius[10];
  dtostrf( celsius, 3, 2, charCelsius );

  // storing the realtime data to a buffer string
  sprintf(strBuf, "%d:%d:%d, %s" ,hour,minute,second, charCelsius);

  // writing the sensor data to the sd card
  File logFile = SD.open("tempLog.txt", FILE_WRITE);
  if (logFile) {
    logFile.println(strBuf);
    logFile.close();
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening tempLog.txt");
  }

  // re-opening the file for reading:
  logFile = SD.open("tempLog.txt");
  if (logFile) {
    // read from the file until there's nothing else in it:
    while (logFile.available()) {
      Serial.write(logFile.read());
    }
    // closing the file:
    logFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening tempLog.txt");
  }
  Serial.println("");

  delay(1000);

}

// checking the files in the sd card
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}