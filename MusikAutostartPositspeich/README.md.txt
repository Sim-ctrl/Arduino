# ESP8266 MP3-Player mit SD-Karte und PCM5102

Dieses Projekt spielt MP3-Dateien von einer MicroSD-Karte über einen PCM5102 DAC (3,5mm Klinkenausgang) ab. Beim Einschalten startet die Wiedergabe SOFORT GENAU DA, wo sie beim letzten Ausschalten gestoppt wurde.


***

## Verwendete Hardware

- **NodeMCU ESP8266 (AZ-Delivery ESP8266MOD12-F)**
- **GY-PCM5102 I2S DAC Modul** (mit 3,5mm Klinkenausgang)
- **MicroSD-Kartenleser Modul** 
- MicroSD-Karte (max. 32 GB, FAT32 formatiert)
- Billiglautsprecher mit 3,5mm Klinke

***

## Verkabelung

### GY-PCM5102 → NodeMCU

| PCM5102 | NodeMCU |
|---|---|
| VIN | 3.3V |
| GND | GND |
| BCK | D8 |
| LRCK | D4 |
| DIN | RX |
| SCK | GND |
| XSMT | 3.3V |

> **Hinweis:** XSMT muss zwingend auf 3.3V liegen, sonst ist der DAC stummgeschaltet.  
> SCK auf GND setzen damit der PCM5102 seinen Clock intern per PLL aus BCK ableitet.

### SPI SD-Kartenleser → NodeMCU

| SD-Modul | NodeMCU |
|---|---|
| VCC | 3.3V |
| GND | GND |
| CS | D2 |
| MOSI | D7 |
| SCK | D5 |
| MISO | D6 |

***
> **JA CS muss auf D2 obwohl EddisTechlab gesagt hat das muss auf D4 - in meinem Fall ging nur D2**

## SD-Karte vorbereiten

- Format: **FAT32** (nicht exFAT, nicht FAT16)
- MP3-Dateien direkt ins Root-Verzeichnis (kein Unterordner)
- Dateinamen in der Playlist im Code eintragen, z.B.:
  ```
  test1.mp3
  test2.mp3
  test3.mp3
  ```
- Empfohlene MP3-Qualität laut perplexy: **128 kbps, 44.1 kHz** (höhere Bitraten können auf dem ESP8266 Aussetzer verursachen)

***

## Bibliotheken (Arduino IDE)

Alle über den Bibliotheksmanager installieren:

- **ESP8266Audio** von Earle F. Philhower III
- **SD** (ESP8266-eigene Version, wird automatisch genutzt)
- **SPI** (bereits in ESP8266-Core enthalten)

***

## Code-Übersicht

```cpp
#define SD_CS_PIN 4  // D2 = GPIO4
```

Der Code besteht aus drei Kernfunktionen:

- `savePosition()` – Schreibt aktuelle Dateinummer und Byte-Position ins EEPROM
- `startPlayback(idx, seekPos)` – Öffnet die MP3-Datei und springt direkt zur gespeicherten Position
- `loop()` – Treibt den MP3-Decoder, speichert alle 5 Sekunden die Position, wechselt automatisch zur nächsten Datei wenn eine fertig ist

***

## Funktionsweise

### Autostart beim Einschalten
`setup()` startet sofort nach dem Booten die Wiedergabe – kein Tastendruck nötig.

### Positionsspeicherung
Alle 5 Sekunden wird die aktuelle Byte-Position der MP3-Datei per `EEPROM.put()` + `EEPROM.commit()` im Flash gespeichert. Bei einem Stromausfall gehen maximal ~5 Sekunden verloren.

### Wiederaufnahme
Beim nächsten Einschalten liest `setup()` Dateinummer und Position aus dem EEPROM und springt mit `file->seek()` direkt dorthin.

### Automatische Playlist
Ist eine Datei fertig, springt der Code automatisch zur nächsten. Nach der letzten Datei beginnt die Playlist von vorne (Loop).

***

## Bekannte Einschränkungen

- Der PCM5102 gibt ein **Line-Level-Signal** aus – passive Lautsprecher funktionieren ohne Verstärker nicht. Kopfhörer oder aktive Boxen direkt an die 3,5mm Buchse anschließen.
- MP3-Dateien mit **mehr als 192 kbps** können auf dem ESP8266 gelegentlich Aussetzer verursachen.
- Der **UART (RX-Pin / GPIO3)** wird für I2S (DIN) verwendet – während der Wiedergabe ist kein Serial-Debugging über diesen Pin möglich.