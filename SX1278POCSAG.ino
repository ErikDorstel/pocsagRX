#include "SX1278.h"

SX1278FSK modem(false,1);

#include "CLI.h"

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK, MISO, MOSI, CS);
  modem.initChip();
  modem.setFrequency(439.9875,7.446);
  modem.setBitrate(1.2);
  modem.setShift(4.5);
  modem.setRxBwAuto();
  modem.setAfcBwAuto();
  modem.beginPOCSAG(false); }

void loop() {
  modem.decodePOCSAG(); cliWorker(); }
