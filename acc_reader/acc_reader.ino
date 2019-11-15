#include <Wire.h>

// Connection map
//ESP32      GY521/MPU6050
//3V3 ...... VCC
//GND ...... GND
//D25 ...... SCL
//D21 ...... SCA


//default I2C addr of the accelerometer
#define MPU_ADDR 0x68
#define DEBUG 1

int16_t AcX, AcY, AcZ;
int16_t maxX, maxY, maxZ;

String values;

#define CALIBRATION_DURATION 3000 // calibration is 3s long
unsigned long t;

void setup() {
  Wire.begin(21,22); // D2 (GPIO12) = SDA | D1(GPIO22)=SCL | default I2C ports
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 => turn on MPU
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(115200);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDR,6,true);
  
  Serial.println("calibrating");
  calibrate();
  Serial.println("setup done");
}

void loop() {

  read_acc();

  if ( AcY > maxY && AcZ > maxZ) { //AcX > maxX &&
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }
  
  delay(10);
}


String calibration_result;
void calibrate()
{
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

void read_acc() 
{
  AcX =Wire.read()<<8|Wire.read();
  AcY =Wire.read()<<8|Wire.read();
  AcZ =Wire.read()<<8|Wire.read();

  if(DEBUG) {
    values = "X: "+String(AcX) + " Y: "+String(AcY)+" Z: "+String(AcZ)+ " Maxes: ("+String(maxX)+","+String(maxY)+","+String(maxZ)+")";
    Serial.println(values);
  }
}
