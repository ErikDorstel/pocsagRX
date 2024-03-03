# pocsagRX
SX1278 and ESP32 based POCSAG receiver
#### Features
* uses Semtech SX1278 as FSK receiver
* also works with Semtech SX1276
* receives 1200 bps, 4500 Hz shift POCSAG messages
* serial console logging of received messages
* works with direct connected ESP32 (see pin description below)
* also works with HELTEC SX1278 based LoRa module
  * activate the HeltecLoRaV2 define statement in SX1278POCSAG.ino to use this module
* serial console Command Line Interface for configuration and debugging
* telnet server Command Line Interface for configuration and debugging
* 3 levels of debugging
* write/read configuration to/from flash
* german BOS mode and DAU filter
* german ASCII codes
* decoding of ROT1 encoded messages
* wlan based https gateway to forwarding the messages
* error correction by BCH decoding
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
![IMAGE ALT TEXT HERE](documentation/SX1278POCSAG.png)
#### Console Screenshot (DAPNET DB0HBO)
![IMAGE ALT TEXT HERE](documentation/console.png)
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
#### define statement for the SX1278 based HELTEC LoRa module
activate the HeltecLoRaV2 define statement in SX1278POCSAG.ino to use this GPIO pins
* GPIO 19 - MISO
* GPIO 27 - MOSI
* GPIO 5 - SCLK
* GPIO 18 - CS
* GPIO 14 - RESET
* GPIO 26 - DIO0
* GPIO 35 - DIO1
* GPIO 34 - DIO2
