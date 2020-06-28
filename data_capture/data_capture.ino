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
#define WIFI_MAX_ATTEMPTS 10;

int16_t ax, ay, az; // Accelerometer variables
int16_t temp;
int16_t gx, gy, gz; // Gyroscope variables

String values;

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
  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED || wifi_attempts > WIFI_MAX_ATTEMPTS) {
      delay(500);
      wifi_attempts++;
      // Serial.print(".");
  }
  // Serial.println(" CONNECTED");
  if (WiFi.status() == WL_CONNECTED) {
    configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);
    getLocalTime(&timeinfo);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
  }
}

void setup() {
  Serial.begin(115200);
  accelerometer_setup();
  pinMode(LED, OUTPUT);
  timer_setup();
  SD_setup();
  Serial.println("setup done");
}

void loop() {
  read_acc(&ax, &ay, &az, &temp, &gx, &gy, &gz);

  //write values
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  values = "ax:" + String(ax) + " ay:" + String(ay) + " az:" + String(az) + " temp:" + String(temp) + " gx:" + String(gx) + " gy:" + String(gy) + " gz:" + String(gz) + "\n";
  String log_message = String(timestamp) + "|" + values + "\n";
  if (DEBUG) {
    Serial.print(values);
  }
  char results[1024];
  strcpy(results, log_message.c_str());
  append_file(SD, "/data_capture.txt", results);
  
  delay(10);
}
