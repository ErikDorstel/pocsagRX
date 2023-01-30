#include <Preferences.h>
#include <nvs_flash.h>

Preferences flash;

void getFlash() {
  flash.begin("SX1278FSK",true);
  Serial.print("Center Frequency: "); Serial.print(flash.getDouble("centerFreq",439.9875),5); Serial.println(" MHz");
  Serial.print("Rx Frequency Offset: "); Serial.print(flash.getDouble("rxOffset",7.446),3); Serial.println(" kHz");
  Serial.print("Bitrate: "); Serial.print(flash.getDouble("bitrate",1.2)*1000,0); Serial.println(" bps");
  Serial.print("Shift Frequency: +/- "); Serial.print(flash.getDouble("shift",4.5)*1000,0); Serial.println(" Hz");
  Serial.print("Rx Bandwidth: "); Serial.print(flash.getDouble("rxBandwidth",5.2),1); Serial.println(" kHz");
  Serial.print("AFC Bandwidth: "); Serial.print(flash.getDouble("afcBandwidth",25),1); Serial.println(" kHz");
  Serial.print("BOS Mode: "); Serial.println(flash.getBool("isBOS",false));
  Serial.print("WLAN SSID: "); Serial.println(flash.getString("wlanSSID",""));
  Serial.print("WLAN Secret: "); if (flash.getString("wlanSecret","")!="") { Serial.println("xxxx"); } else { Serial.println(); }
  Serial.print("Gateway URL: "); Serial.println(flash.getString("gwURL",""));
  flash.end(); }

void readFlash() {
  flash.begin("SX1278FSK",true);
  modem.stopSequencer();
  modem.setFrequency(flash.getDouble("centerFreq",439.9875),flash.getDouble("rxOffset",7.446));
  modem.setBitrate(flash.getDouble("bitrate",1.2));
  modem.setShift(flash.getDouble("shift",4.5));
  modem.setRxBandwidth(flash.getDouble("rxBandwidth",5.2));
  modem.setAfcBandwidth(flash.getDouble("afcBandwidth",25));
  modem.startSequencer();
  modem.restartRx(true);
  modem.isBOS=flash.getBool("isBOS",false);
  wlanSSID=flash.getString("wlanSSID","");
  wlanSecret=flash.getString("wlanSecret","");
  gwURL=flash.getString("gwURL","");
  flash.end();
  Serial.println("Flash: read");
  connectWLAN(); }

void writeFlash() {
  flash.begin("SX1278FSK",false);
  flash.putDouble("centerFreq",modem.centerFreq);
  flash.putDouble("rxOffset",modem.rxOffset);
  flash.putDouble("bitrate",modem.bitrate);
  flash.putDouble("shift",modem.shift);
  flash.putDouble("rxBandwidth",modem.rxBandwidth);
  flash.putDouble("afcBandwidth",modem.afcBandwidth);
  flash.putBool("isBOS",modem.isBOS);
  flash.putString("wlanSSID",wlanSSID);
  flash.putString("wlanSecret",wlanSecret);
  flash.putString("gwURL",gwURL);
  flash.end();
  Serial.println("Flash: written"); }

void eraseFlash() {
  nvs_flash_erase(); nvs_flash_init();
  Serial.println("Flash: erased"); }
