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
int16_t temp = 0;
int16_t g_x, g_y, g_z = 0; // Gyroscope variables

String values;

unsigned long t; // timer

//timer and WiFi
const char* ssid       = "XXX";
const char* password   = "xxx";

const char* ntp_server = "pool.ntp.org";
const long  gmt_offset_sec = 3600;
const int   daylight_offset_sec = 3600;

struct tm timeinfo;

void timer_setup() {
  Serial.begin(115200);
  //Serial.printf("Connecting to %s ", ssid);
  if (ssid == "" or password == "") {
    Serial.println("SSID or PASSWORD are not set!");
  }
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      //Serial.print(".");
  }
  //Serial.println(" CONNECTED");
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
  Serial.println("setup done");
}

void loop() {
  read_acc();

  //write values
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", &timeinfo);
  values = "ax:" + String(a_x) + " ay:" + String(a_y) + " az:" + String(a_z) + " gx:" + String(g_x) + " gy:" + String(g_y) + " gz:" + String(g_z) + ";\n";
  String log_message = String(timestamp) + ";" + values + "\n";
  if (DEBUG) {
    Serial.print(values);
  }
  char results[1024];
  strcpy(results, log_message.c_str());
  append_file(SD, "/data_capture.txt", results);
  
  delay(10);
}
