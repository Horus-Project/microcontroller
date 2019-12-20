// Connection map
//ESP32      GY521/MPU6050
//3V3 ...... VCC
//GND ...... GND
//D25 ...... SCL
//D21 ...... SCA

#include <Wire.h>
#include <WiFi.h>
#include "time.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// accelerometer
#define MPU_ADDR 0x68 //default I2C addr
#define MPU_PWR_MGMT_1 0x6B

#define DEBUG 1
#define SD_DEBUG 0
#define LED 2

int16_t a_x, a_y, a_z; // Accelerometer variables
int16_t max_x, max_y, max_z; // Threshold to detect relevant movement

String values;
String calibration_result;

#define CALIBRATION_DURATION 3000 // calibration is 3s long
unsigned long t; // timer

//timer and WiFi
const char* ssid       = "";
const char* password   = "";

const char* ntp_server = "pool.ntp.org";
const long  gmt_offset_sec = 3600;
const int   daylight_offset_sec = 3600;

struct tm timeinfo;


void calibrate() {
  Serial.println("calibrating");
  t = millis();
  unsigned long calibration_end = t + CALIBRATION_DURATION;
  while(t < calibration_end){
    // read values, keep calibration maxes
    read_acc();
    if (a_x > max_x) {
      max_x = a_x;
    }
    if (a_y > max_y) {
      max_y = a_y;
    }
    if (a_z > max_z) {
      max_z = a_z;
    }
    delay(10);
    t = millis();
  }
  calibration_result = "max_x: "+String(max_x)+" max_y: "+String(max_y)+" max_z: "+String(max_z);
  Serial.println(calibration_result);
}

// Puts the lectures of the accelerometer
// in the global vairables
void read_acc() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,6,true);

  a_x = Wire.read()<<8|Wire.read();
  a_y = Wire.read()<<8|Wire.read();
  a_z = Wire.read()<<8|Wire.read();

  a_x = abs(a_x);
  a_y = abs(a_y);
  a_z = abs(a_z);

  if(DEBUG) {
    values = "X: "+String(a_x) + " Y: "+String(a_y)+" Z: "+String(a_z)+ " Maxes: ("+String(max_x)+","+String(max_y)+","+String(max_z)+") ";
    if (a_x > max_x) {
      values = values + "X";
    }
    if (a_y > max_y) {
      values = values + "Y";
    }
    if (a_z > max_z) {
      values = values + "Z";
    }
  }
}

void SD_setup() {
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
}

void append_file(fs::FS &fs, const char * path, const char * message) {
    if (SD_DEBUG) {
      Serial.printf("Appending to file: %s\n", path);
    }

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        if (SD_DEBUG) {
            Serial.println("Failed to open file for appending");
        }
        return;
    }
    if(file.print(message)){
        if (SD_DEBUG) {
            Serial.println("Message appended");
        }
    } else {
        if (SD_DEBUG) {
            Serial.println("Append failed");
        }
    }
    file.close();
}

void timer_setup() {
  Serial.begin(115200);
  Serial.printf("Connecting to %s ", ssid);
  if (ssid == "" or password == "") {
    Serial.println("SSID or PASSWORD are not set!");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void setup() {
  Wire.begin(21,22); // D2 (GPIO12) = SDA | D1(GPIO22)=SCL | default I2C ports
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_PWR_MGMT_1); // PWR_MGMT_1 => turn on MPU
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  timer_setup();
  SD_setup();
  calibrate();
  Serial.println("setup done");
}

void loop() {
  read_acc();
  if (a_y > max_y && a_z > max_z) { //a_x > max_x &&
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }

  //write values
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  
  values = String(timestamp) + ";" + values + "\n";
  if (DEBUG) {
    Serial.println(values);
  }
  char results[1024];
  strcpy(results, values.c_str());
  append_file(SD, "/results.txt", results);
  
  delay(10);
}
