#include "time.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

int SD_setup() {
  Serial.println("setting up SD card module");
  if(!SD.begin()){
    Serial.println("Card Mount Failed");
    return EIO;
  }
  return 0;
}

int append_file(fs::FS &fs, const char * path, const char * message) {
    int error = 0;
    if (SD_DEBUG) {
      Serial.printf("Appending to file: %s\n", path);
    }
    File file = fs.open(path, FILE_APPEND);
    if(!file){
      if (SD_DEBUG) {
        Serial.println("ERROR: Failed to open file");
      }
      return ENOENT;
    }
    if (!file.print(message)){
      error = EIO;
    }
    if (SD_DEBUG) {
      if (error) {
        Serial.println("ERROR: Append failed");
      } else {
        Serial.println("Message appended");
      }
    }
    file.close();
    return error;
}
