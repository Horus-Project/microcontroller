# graboid
ESP32 vibration detection and logging project

## Components
* Espressif ESP32 WROOM 32
* 5pcs Mini SD Card Module Memory Module Micro SD Card Module AVR ARM

### Accelerometers
* [3-Axis, 12-bit/8-bit Digital Accelerometer MMA8452Q](https://www.electronicaembajadores.com/datos/pdf2/ss/ssac/ssac002.pdf)
* 3-Axis Gyroscope & Accelerometer GY-521

| ESP32 | GY521/MPU6050 |
|:-:|:-:|
| 3V3 | VCC|
| GND | GND |
| D25 | SCL |
| D21 | SCA |

## ESP32 Programming

### SD Card module
| ESP32 | SD module |
|:-:|:-:|
| 3V3 | 3V3 |
| GND | GND |
| D5 | CS |
| D18 | CLK |
| D19 | MISO |
| D23 | MOSI |

* [SD Card Module with Arduino: How to Read/Write Data](https://create.arduino.cc/projecthub/electropeak/sd-card-module-with-arduino-how-to-read-write-data-37f390)
