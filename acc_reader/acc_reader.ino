#include <Wire.h>

// Connection map
//ESP32      GY521/MPU6050
//3V3 ...... VCC
//GND ...... GND
//D25 ...... SCL
//D21 ...... SCA


//default I2C addr of the accelerometer
#define MPU_ADDR 0x68

//Conversion rates
#define A_R 16384.0 //32768 / 2
#define G_R 131.0 //32768 / 250

#define RAD_TO_DEG 57.295779 // 180/PI

int16_t AcX, AcY, AcZ;
int16_t maxX, maxY, maxZ;
int16_t temp, gx, gy, gz;
float Acc[2];

String values;

long prev_t;
float dt;

void setup() {
  Wire.begin(21,22); // D2 (GPIO12) = SDA | D1(GPIO22)=SCL | default I2C ports
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 => turn on MPU
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);
  Serial.println("setup done");
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,6,true);

  //Store measures
  AcX = Wire.read()<<8|Wire.read();
  AcY = Wire.read()<<8|Wire.read();
  AcZ = Wire.read()<<8|Wire.read();

  if (AcX > maxX) {
    maxX = AcX;
  }

  if (AcY > maxY) {
    maxY = AcY;
  }

  if (AcZ > maxZ) {
    maxZ = AcZ;
  }

  values = "X: "+String(AcX) + " Y: "+String(AcY)+" Z: "+String(AcZ)+ " Maxes: ("+String(maxX)+","+String(maxY)+","+String(maxZ)+")";
  Serial.println(values);
  delay(10);

}
