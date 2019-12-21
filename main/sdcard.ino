#include "time.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

void SD_setup() {
  Serial.println("setting up SD card module");
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return;
  }
}

void append_file(fs::FS &fs, const char * path, const char * message) {
    if (SD_DEBUG) {
      Serial.printf("Appending to file: %s\n", path);
    }

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        if (SD_DEBUG) {
            Serial.println("Failed to open file for appending");
        }
        return;
    }
    if(file.print(message)){
        if (SD_DEBUG) {
            Serial.println("Message appended");
        }
    } else {
        if (SD_DEBUG) {
            Serial.println("Append failed");
        }
    }
    file.close();
}
