// Connection map
//ESP32      GY521/MPU6050
//3V3 ...... VCC
//GND ...... GND
//D25 ...... SCL
//D21 ...... SCA

#include <Wire.h>
#include <WiFi.h>
#include "time.h"
#include "SD.h"

#define DEBUG 1
#define SD_DEBUG 0
#define LED 2
#define LOGGING_CICLE 60000 // 1 minute

int16_t ax, ay, az; // Accelerometer measures
int16_t temp; // temperature measure
int16_t gx, gy, gz; // Gyroscope measures
int16_t maxax, maxay, maxaz = -24000; // Threshold to detect relevant movement

String values;
String engine_on;

unsigned long t; // timer

//timer and WiFi
const char* ssid       = "Redmi";
const char* password   = "017c1b12a8ee";

const char* ntp_server = "pool.ntp.org";
const long  gmt_offset_sec = 3600;
const int   daylight_offset_sec = 3600;

struct tm timeinfo;

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
  getLocalTime(&timeinfo);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void setup() {
  Serial.begin(115200);
  accelerometer_setup();
  pinMode(LED, OUTPUT);
  timer_setup();
  SD_setup();
  calibrate(&maxax, &maxay, &maxaz);
  Serial.println("setup done");
}

void loop() {
  read_acc(&ax, &ay, &az, &temp, &gx, &gy, &gz);
  if (ay > maxay || az > maxaz || ax > maxax) {
    digitalWrite(LED, HIGH);
    engine_on = "1";
  } else {
    digitalWrite(LED, LOW);
    engine_on = "0";
  }

  //write values
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  
  String log_message = String(timestamp) + ";" + engine_on + "\n";
  if (DEBUG) {
    values = String(timestamp) + ";" + values + "\n";
    Serial.print(values);
  }
  char results[1024];
  strcpy(results, log_message.c_str());
  append_file(SD, "/results.txt", results);
  
  delay(10);
}
