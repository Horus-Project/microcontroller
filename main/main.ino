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

int16_t a_x, a_y, a_z = 0; // Accelerometer variables
int16_t max_x, max_y, max_z = 0; // Threshold to detect relevant movement

String values;
String engine_on;

unsigned long t; // timer

//timer and WiFi
const char* ssid       = "";
const char* password   = "";

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
  calibrate();
  Serial.println("setup done");
}

void loop() {
  read_acc();
  if (a_y > max_y && a_z > max_z) { //a_x > max_x &&
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
