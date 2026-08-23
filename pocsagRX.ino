#define HeltecLoRaV2

#include "SX1278.h"
SX1278FSK modem(false,0);
#include "CLI.h"

void setup() {
  initWLAN();
  initTELNET();
  modem.initChip();
  modem.beginPOCSAG();
  readFlash(); }

void loop() { modem.pocsagWorker(); cliWorker(); wlanWorker(); telnetWorker(); }
