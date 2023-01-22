# SX1278POCSAG
SX1278 and ESP32 based POCSAG receiver
#### Features
* uses Semtech SX1278 as FSK receiver
* receives 1200 bps, 4500 Hz shift POCSAG messages
* serial console logging of received messages
* works with direct connected ESP32 (see pin description below)
* also works with HELTEC SX1278 based LoRa module (requires a change in the pin define statements)
#### SPI bus
* GPIO 19 - MISO
* GPIO 23 - MOSI
* GPIO 18 - SCLK
* GPIO 5 - CS
#### other SX1278 connections
* GPIO 16 - RESET
* GPIO 25 - DIO0
* GPIO 26 - DIO1
* GPIO 27 - DIO2
* GPIO 32 - DIO3 (for future development)
#### Development Hardware
![IMAGE ALT TEXT HERE](https://www.dorstel.de/github/SX1278POCSAG_a_v1.0.png)
#### Console Screenshot (DAPNET DB0HBO)
![IMAGE ALT TEXT HERE](https://www.dorstel.de/github/SX1278POCSAG_b_v1.0.png)
#### define statements for the SX1278 based HELTEC LoRa module
    #define SCK 5
    #define MISO 19
    #define MOSI 27
    #define CS 18
    #define RST 14
    #define DIO0 26
    #define DIO1 35
    #define DIO2 34
