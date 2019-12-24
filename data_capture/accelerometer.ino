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

// Puts the lectures of the accelerometer
// in the global vairables
void read_acc() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,14,true);

  a_x = Wire.read()<<8|Wire.read();
  a_y = Wire.read()<<8|Wire.read();
  a_z = Wire.read()<<8|Wire.read();

  temp = Wire.read()<<8|Wire.read();

  g_x = Wire.read()<<8|Wire.read();
  g_y = Wire.read()<<8|Wire.read();
  g_z = Wire.read()<<8|Wire.read();
}
