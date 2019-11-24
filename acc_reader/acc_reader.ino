#include <Wire.h>

// Connection map
//ESP32      GY521/MPU6050
//3V3 ...... VCC
//GND ...... GND
//D25 ...... SCL
//D21 ...... SCA

#include "FS.h"
#include "SD.h"
#include "SPI.h"

// accelerometer
#define MPU_ADDR 0x68 //default I2C addr
#define MPU_PWR_MGMT_1 0x6B


#define DEBUG 1
#define LED 2

int16_t AcX, AcY, AcZ; // Accelerometer variables
int16_t maxX, maxY, maxZ; // Threshold to detect relevant movement

String values;
String calibration_result;

#define CALIBRATION_DURATION 3000 // calibration is 3s long
unsigned long t; // timer

void setup() {
  Wire.begin(21,22); // D2 (GPIO12) = SDA | D1(GPIO22)=SCL | default I2C ports
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_PWR_MGMT_1); // PWR_MGMT_1 => turn on MPU
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  SDSetup();
  calibrate();
  Serial.println("setup done");
}

void loop() {

  read_acc();

  if ( AcY > maxY && AcZ > maxZ) { //AcX > maxX &&
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
  
  delay(10);
}

void calibrate()
{
  Serial.println("calibrating");
  t = millis();
  while(t < CALIBRATION_DURATION){
    // read values, keep calibration maxes
    read_acc();
    if (AcX > maxX) {
      maxX = AcX;
    }
    if (AcY > maxY) {
      maxY = AcY;
    }
    if (AcZ > maxZ) {
      maxZ = AcZ;
    }
    delay(10);
    t = millis();
  }
  calibration_result = "maxX: "+String(maxX)+" maxY: "+String(maxY)+" maxZ: "+String(maxZ);
  Serial.println(calibration_result);
}

// Puts the lectures of the accelerometer 
// in the global vairables
void read_acc() 
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,6,true);
  
  AcX = Wire.read()<<8|Wire.read();
  AcY = Wire.read()<<8|Wire.read();
  AcZ = Wire.read()<<8|Wire.read();

  AcX = abs(AcX);
  AcY = abs(AcY);
  AcZ = abs(AcZ);

  if(DEBUG) {
    values = "X: "+String(AcX) + " Y: "+String(AcY)+" Z: "+String(AcZ)+ " Maxes: ("+String(maxX)+","+String(maxY)+","+String(maxZ)+") ";
    if (AcX > maxX) {
      values = values + "X";
    }
    if (AcY > maxY) {
      values = values + "Y";
    }
    if (AcZ > maxZ) {
      values = values + "Z";
    }

    values = values + "\n";
    char tab2[1024];
    strcpy(tab2, values.c_str());
    Serial.println(values);
    appendFile(SD, "/results.txt", tab2);
  }
}

void SDSetup() {
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
  writeFile(SD, "/results.txt", "Hello!");
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}
