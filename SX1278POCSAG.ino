#include "SX1278.h"

SX1278FSK modem(false,0);

#include "WLAN.h"
#include "CLI.h"

void messageReceived(uint8_t error, uint32_t ric, char function, String dau, String message) {
  String postValue="error=" + String(error);
  postValue+="&ric=" + String(ric);
  postValue+="&function=" + String(function);
  postValue+="&dau=" + urlencode(String(function));
  postValue+="&message=" + urlencode(String(message));
  postHTTPS(postValue); Serial.println(postValue); }

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK, MISO, MOSI, CS);
  initWLAN();
  modem.initChip();
  modem.beginPOCSAG(messageReceived);
  readFlash(); }

void loop() { modem.pocsagWorker(); cliWorker(); }
