//#define HeltecLoRaV2

#include "SX1278.h"

SX1278FSK modem(false,0);

#include "WLAN.h"
#include "CLI.h"

void messageReceived(double rssi,uint8_t error, uint32_t ric, char function, String dau, String message) {
  String postValue="dme=" + modem.esp32ID;
  postValue+="&rssi=" + urlencode(String(rssi,1));
  postValue+="&error=" + String(error);
  postValue+="&ric=" + String(ric);
  postValue+="&function=" + String(function);
  postValue+="&dau=" + urlencode(dau);
  postValue+="&message=" + urlencode(message);
  postHTTPS(postValue); }

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK, MISO, MOSI, CS);
  initWLAN();
  modem.initChip();
  modem.beginPOCSAG(messageReceived);
  readFlash(); }

void loop() { modem.pocsagWorker(); cliWorker(); }
