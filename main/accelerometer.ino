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

void calibrate(int16_t* maxax, int16_t* maxay, int16_t* maxaz) {
  Serial.println("calibrating");

  int16_t ax, ay, az, tmp, gx, gy, gz;
  t = millis();
  unsigned long calibration_end = t + CALIBRATION_DURATION;
  while(t < calibration_end){
    // read values, keep calibration maxes
    read_acc(&ax, &ay, &az, &tmp, &gx, &gy, &gz);
    if (ax > *maxax) {
      *maxax = ax;
    }
    if (ay > *maxay) {
      *maxay = ay;
    }
    if (az > *maxaz) {
      *maxaz = az;
    }
    delay(10);
    t = millis();
  }
  String calibration_result = "max_x: "+String(*maxax)+" max_y: "+String(*maxay)+" max_z: "+String(*maxaz);
  Serial.println(calibration_result);
}

// Puts the lectures of the accelerometer
// in the global vairables
void read_acc(int16_t* ax, int16_t* ay, int16_t* az, int16_t* temp, int16_t* gx, int16_t* gy, int16_t* gz) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,14,true);

  *ax = Wire.read()<<8|Wire.read();
  *ay = Wire.read()<<8|Wire.read();
  *az = Wire.read()<<8|Wire.read();

  *temp = Wire.read()<<8|Wire.read();
  
  *gx = Wire.read()<<8|Wire.read();
  *gy = Wire.read()<<8|Wire.read();
  *gz = Wire.read()<<8|Wire.read();

  if(DEBUG) {
    values = "X:"+String(*ax) + " Y:"+String(*ay)+", Z:"+String(*az);
  }
}
