# SX1278POCSAG
SX1278 and ESP32 based POCSAG receiver
#### Features
* uses Semtech SX1278 as FSK receiver
* also works with Semtech SX1276
* receives 1200 bps, 4500 Hz shift POCSAG messages
* serial console logging of received messages
* works with direct connected ESP32 (see pin description below)
* also works with HELTEC SX1278 based LoRa module (requires modification of pin define statements)
* serial console Command Line Interface for configuration and debugging
* write/read configuration to/from flash
* german BOS mode and ASCII codes
* decoding of ROT1 encoded messages
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
![IMAGE ALT TEXT HERE](https://www.dorstel.de/github/SX1278POCSAG_b_v1.1.png)
#### Receiver Configuration
* center frequency in MHz and Rx error offset frequency in kHz
  * the Rx error offset is the measured AFC value when Rx error offset is set to 0
* bitrate in kbps (e.g. 1.2 for 1200 bps)
* frequency shift in kHz
  * this means the single sided shift (e.g. 4.5 for +/- 4500 Hz)
* Rx bandwidth in kHz (choose the next larger value from table in SX1278.h)
  * this means the single sided bandwidth
  * calculation: RXbandwidth >= Fshift+(bitrate/2)
  * the auto function uses the above formula
* AFC bandwidth in kHz (choose the next larger value from table in SX1278.h)
  * calculation: AFCbandwidth >= 2*(Fshift+(bitrate/2))+Ferror
  * Ferror means the maximum center frequency offset error of the SX1278 module
  * the auto function uses the above formula
#### define statements for the SX1278 based HELTEC LoRa module
    // SX1278.h
    #define SCK 5
    #define MISO 19
    #define MOSI 27
    #define CS 18
    #define RST 14
    // SX1278ISR.h
    #define DIO0 26
    #define DIO1 35
    #define DIO2 34
    #define DIO3 32 // currently not in use
