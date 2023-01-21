# SX1278POCSAG
SX1278 and ESP32 based POCSAG receiver
#### Features
* uses Semtech SX1278 as FSK receiver
* receives 1200 bps, 4500 Hz shift POCSAG messages
* console logging of received messages
* works with direct connected ESP32 (see pin description below)
* also works with HELTEC SX1278 based LoRa module
#### SPI bus
* GPIO 19 - MISO
* GPIO 23 - MOSI
* GPIO 18 - SCLK
* GPIO 5 - CS
#### other SX1278 connections
* GPIO 25 - DIO0
* GPIO 26 - DIO1
* GPIO 27 - DIO2
* GPIO 32 - DIO3 (for future using)
* GPIO 16 - RESET
#### Development Hardware
![IMAGE ALT TEXT HERE](https://www.dorstel.de/github/)
#### Console Screenshot
![IMAGE ALT TEXT HERE](https://www.dorstel.de/github/)