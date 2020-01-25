#include <Wire.h>
// accelerometer
#define MPU_ADDR 0x68 //default I2C addr
#define MPU_PWR_MGMT_1 0x6B

#define CALIBRATION_DURATION 3000 // calibration is 3s long

void accelerometer_setup() {
  Serial.println("setting up accelerometer");
  Wire.begin(21,22); // D2 (GPIO12) = SDA | D1(GPIO22)=SCL | default I2C ports
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(MPU_PWR_MGMT_1); // PWR_MGMT_1 => turn on MPU
  Wire.write(0);
  Wire.endTransmission(true);
}

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
  String calibration_result = "max_x: "+String(max_x)+" max_y: "+String(max_y)+" max_z: "+String(max_z);
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

  //a_x = abs(a_x);
  //a_y = abs(a_y);
  //a_z = abs(a_z);

  if(DEBUG) {
    values = "X:"+String(a_x);// + ", Y:"+String(a_y)+", Z:"+String(a_z);// + " Maxes: ("+String(max_x)+","+String(max_y)+","+String(max_z)+") ";
    /*if (a_x > max_x) {
      values = values + "X";
    }
    if (a_y > max_y) {
      values = values + "Y";
    }
    if (a_z > max_z) {
      values = values + "Z";
    }*/
  }
}
