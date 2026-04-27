#include <Arduino.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"

#define SD_CS_PIN     4      // D2 = GPIO4
#define EEPROM_SIZE   16
#define ADDR_FILE     0
#define ADDR_POS      4
#define SAVE_INTERVAL 5000

AudioGeneratorMP3    *mp3;
AudioFileSourceSD    *file;
AudioOutputI2S       *out;
AudioOutputMixer     *mixer;
AudioOutputMixerStub *stub;

uint8_t  currentFile = 0;
uint32_t savedPos    = 0;
unsigned long lastSave = 0;

const char* playlist[] = {
  "/test1.mp3",
  "/test2.mp3",
  "/test3.mp3"
};
const uint8_t PLAYLIST_SIZE = sizeof(playlist) / sizeof(playlist[0]);

void savePosition() {
  uint32_t pos = file ? file->getPos() : 0;
  EEPROM.write(ADDR_FILE, currentFile);
  EEPROM.put(ADDR_POS, pos);
  EEPROM.commit();
  Serial.printf("Gespeichert: Datei %d, Byte %u\n", currentFile, pos);
}

void startPlayback(uint8_t idx, uint32_t seekPos = 0) {
  if (mp3 && mp3->isRunning()) mp3->stop();
  if (stub) stub->stop();
  delete file;

  file = new AudioFileSourceSD(playlist[idx]);
  if (seekPos > 0) file->seek(seekPos, SEEK_SET);

  stub = mixer->NewInput();
  stub->SetGain(1.0);
  mp3->begin(file, stub);
  Serial.printf("Spiele: %s ab Byte %u\n", playlist[idx], seekPos);
}

void setup() {
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(1000);

  EEPROM.begin(EEPROM_SIZE);
  currentFile = EEPROM.read(ADDR_FILE);
  EEPROM.get(ADDR_POS, savedPos);
  if (currentFile >= PLAYLIST_SIZE) { currentFile = 0; savedPos = 0; }
  Serial.printf("Letzter Stand: Datei %d, Pos %u\n", currentFile, savedPos);

  SPI.begin();
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("FEHLER: SD nicht gefunden!");
    return;
  }
  Serial.println("SD OK");

  audioLogger = &Serial;
  out   = new AudioOutputI2S();
  out->SetGain(4.0);
  mixer = new AudioOutputMixer(32, out);
  mp3   = new AudioGeneratorMP3();
  file  = nullptr;
  stub  = nullptr;

  startPlayback(currentFile, savedPos);
}

void loop() {
  if (mp3 && mp3->isRunning()) {
    if (!mp3->loop()) {
      Serial.printf("Fertig: %s\n", playlist[currentFile]);
      currentFile = (currentFile + 1) % PLAYLIST_SIZE;
      savedPos = 0;
      savePosition();
      startPlayback(currentFile, 0);
    }
    if (millis() - lastSave > SAVE_INTERVAL) {
      savePosition();
      lastSave = millis();
    }
  }
}