#include <Preferences.h>
#include <nvs_flash.h>

Preferences flash;

void getFlash() {
  flash.begin("SX1278FSK",true);
  Log.print(0,"Center Frequency: %s MHz\r\n",String(flash.getDouble("centerFreq",439.9875),5).c_str());
  Log.print(0,"Rx Frequency Offset: %s kHz\r\n",String(flash.getDouble("rxOffset",7.446),3).c_str());
  Log.print(0,"Bitrate: %s bps\r\n",String(flash.getDouble("bitrate",1.2)*1000,0).c_str());
  Log.print(0,"Shift Frequency: +/- %s Hz\r\n",String(flash.getDouble("shift",4.5)*1000,0).c_str());
  Log.print(0,"Rx Bandwidth: %s kHz\r\n",String(flash.getDouble("rxBandwidth",5.2),1).c_str());
  Log.print(0,"AFC Bandwidth: %s kHz\r\n",String(flash.getDouble("afcBandwidth",25),1).c_str());
  Log.print(0,"BOS Mode: %i\r\n",flash.getBool("isBOS",false));
  Log.print(0,"DAU Filter: %s\r\n",String(flash.getString("daufilter","")).c_str());
  Log.print(0,"WLAN SSID: %s\r\n",flash.getString("wlanSSID",""));
  if (flash.getString("wlanSecret","")!="") { Log.print(0,"WLAN Secret: xxxx\r\n"); } else { Log.print(0,"WLAN Secret:\r\n"); }
  Log.print(0,"Gateway URL: %s\r\n",String(flash.getString("gwURL","")).c_str());
  Log.print(0,"Raw URL: %s\r\n",String(flash.getString("rawURL","")).c_str());
  Log.print(0,"TELNET Password: xxxx\r\n");
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
  modem.daufilter=flash.getString("daufilter","");
  wlanSSID=flash.getString("wlanSSID","");
  wlanSecret=flash.getString("wlanSecret","");
  gwURL=flash.getString("gwURL","");
  rawURL=flash.getString("rawURL","");
  telnetPass=flash.getString("telnetPass","pocsag");
  flash.end();
  Log.print(0,"Flash: read\r\n");
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
  flash.putString("daufilter",modem.daufilter);
  flash.putString("wlanSSID",wlanSSID);
  flash.putString("wlanSecret",wlanSecret);
  flash.putString("gwURL",gwURL);
  flash.putString("rawURL",rawURL);
  flash.putString("telnetPass",telnetPass);
  flash.end();
  Log.print(0,"Flash: written\r\n"); }

void eraseFlash() {
  nvs_flash_erase(); nvs_flash_init();
  Log.print(0,"Flash: erased\r\n"); }
