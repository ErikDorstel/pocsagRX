#include "SX1278.h"

SX1278FSK modem(false,0);

#include "WLAN.h"
#include "CLI.h"

void messageReceived(uint8_t error, uint32_t ric, char function, String dau, String message) {
  Serial.print(error); Serial.print(" ");
  Serial.print(ric); Serial.print(" ");
  Serial.print(function); Serial.print(" ");
  Serial.print(dau); Serial.print(" ");
  Serial.println(message); }

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK, MISO, MOSI, CS);
  initWLAN();
  modem.initChip();
  modem.beginPOCSAG(messageReceived);
  readFlash(); }

void loop() { modem.pocsagWorker(); cliWorker(); }
