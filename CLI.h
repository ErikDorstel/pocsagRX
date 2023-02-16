#include "Flash.h"

String cmdLine;

void help() {
  Serial.println("debug [0-3]");
  Serial.println("monitor");
  Serial.println("get version");
  Serial.println("get status");
  Serial.println("clear status");
  Serial.println("get configuration");
  Serial.println("get register");
  Serial.println("set frequency [137.000-525.000]");
  Serial.println("set offset [0-100|auto]");
  Serial.println("set bitrate [0.075-250]");
  Serial.println("set shift [0.6-200]");
  Serial.println("set rxbw [2.6-250|auto]");
  Serial.println("set afcbw [2.6-250|auto]");
  Serial.println("set bos");
  Serial.println("set daufilter [Filter]");
  Serial.println("set ssid [SSID]");
  Serial.println("set secret [Secret]");
  Serial.println("set gwurl [https://foo.de/foo]");
  Serial.println("connect wlan");
  Serial.println("clear wlan");
  Serial.println("restart rx");
  Serial.println("restart cpu");
  Serial.println("get flash");
  Serial.println("read flash");
  Serial.println("write flash");
  Serial.println("erase flash");
  Serial.println("help"); }

void doParse() {
  cmdLine.trim();
  if (cmdLine!="") { Serial.println(); }
  String value=cmdLine.substring(cmdLine.lastIndexOf(" ")+1);
  if (cmdLine.startsWith("deb")) { Log.debug=value.toInt(); Serial.print("Debug Level: "); Serial.println(Log.debug); }
  else if (cmdLine.startsWith("mon")) { modem.monitorRx=!modem.monitorRx; }
  else if (cmdLine.startsWith("get ver")) { modem.printChip(); }
  else if (cmdLine.startsWith("get stat")) { modem.printRx();
    Serial.print("Messages received: "); Serial.print(modem.messageCount);
    Serial.print("   Errors corrected: "); Serial.print(modem.errorCount.corrected);
    Serial.print("   uncorrected: "); Serial.print(modem.errorCount.uncorrected);
    Serial.print("   Bytes queued: "); Serial.print(uxQueueMessagesWaitingFromISR(queueDIO1)); Serial.print("/"); Serial.println(queueSizeDIO1);
    Serial.print("WLAN Status: "); if (WiFi.status()==3) { Serial.print("Up ("); } else { Serial.print("Down ("); } Serial.print(WiFi.status()); Serial.print(")");
    Serial.print("   IP: "); Serial.print(WiFi.localIP().toString());
    Serial.print("   RSSI: "); Serial.print(WiFi.RSSI()); Serial.print(" dBm");
    Serial.print("   Events Up: "); Serial.print(upEvents);
    Serial.print("   Down: "); Serial.println(downEvents);
    Serial.print("HTTP Status: "); Serial.print(httpStatus); if (httpStatus==200) { Serial.print(" OK"); }
    Serial.print("   Retried: "); Serial.print(httpRetried);
    Serial.print("   Failed: "); Serial.println(httpFailed);
    Serial.print("Uptime: "); Serial.print(modem.upTime/86400); Serial.print(" days ");
    Serial.print((double)(modem.upTime%86400)/3600.0,2); Serial.println(" hours"); }
  else if (cmdLine.startsWith("clear stat")) { modem.messageCount=0; modem.errorCount.corrected=0; modem.errorCount.uncorrected=0; upEvents=0; downEvents=0; httpRetried=0; httpFailed=0; Serial.println("Statistics cleared"); }
  else if (cmdLine.startsWith("get conf")) {
    Serial.print("Center Frequency: "); Serial.print(modem.centerFreq,5); Serial.println(" MHz");
    Serial.print("Rx Frequency Offset: "); Serial.print(modem.rxOffset,3); Serial.println(" kHz");
    Serial.print("Bitrate: "); Serial.print(modem.bitrate*1000,0); Serial.println(" bps");
    Serial.print("Shift Frequency: +/- "); Serial.print(modem.shift*1000,0); Serial.println(" Hz");
    Serial.print("Rx Bandwidth: "); Serial.print(modem.rxBandwidth,1); Serial.println(" kHz");
    Serial.print("AFC Bandwidth: "); Serial.print(modem.afcBandwidth,1); Serial.println(" kHz");
    Serial.print("BOS Mode: "); Serial.println(modem.isBOS);
    Serial.print("DAU Filter: "); Serial.println(modem.daufilter);
    Serial.print("WLAN SSID: "); Serial.println(wlanSSID);
    Serial.print("WLAN Secret: "); if (wlanSecret!="") { Serial.println("xxxx"); } else { Serial.println(); }
    Serial.print("Gateway URL: "); Serial.println(gwURL); }
  else if (cmdLine.startsWith("get reg")) { modem.regDump(); }
  else if (cmdLine.startsWith("set freq")) { modem.stopSequencer(); modem.setFrequency(value.toDouble()); modem.startSequencer(); modem.restartRx(true); }
  else if (cmdLine.startsWith("set offset")) { if (value!="auto") { modem.rxOffset=value.toDouble(); }
    modem.stopSequencer(); modem.setFrequency(modem.centerFreq,modem.rxOffset); modem.startSequencer(); modem.restartRx(true); }
  else if (cmdLine.startsWith("set bitrate")) { modem.setBitrate(value.toDouble()); }
  else if (cmdLine.startsWith("set shift")) { modem.stopSequencer(); modem.setShift(value.toDouble()); modem.startSequencer(); modem.restartRx(true); }
  else if (cmdLine.startsWith("set rxbw")) { if (value=="auto") { modem.setRxBwAuto(); } else { modem.setRxBandwidth(value.toDouble()); } }
  else if (cmdLine.startsWith("set afcbw")) { if (value=="auto") { modem.setAfcBwAuto(); } else { modem.setAfcBandwidth(value.toDouble()); } }
  else if (cmdLine.startsWith("set bos")) { modem.isBOS=!modem.isBOS; Serial.print("BOS Mode: "); Serial.println(modem.isBOS); }
  else if (cmdLine.startsWith("set dau")) { if (value=="dau" || value=="daufilter") { modem.daufilter=""; } else { modem.daufilter=value; } Serial.print("DAU Filter: "); Serial.println(modem.daufilter); }
  else if (cmdLine.startsWith("set ssid")) { if (value=="ssid") { wlanSSID=""; } else { wlanSSID=value; } Serial.print("WLAN SSID: "); Serial.println(wlanSSID); }
  else if (cmdLine.startsWith("set secret")) { if (value=="secret") { wlanSecret=""; } else { wlanSecret=value; } Serial.print("WLAN Secret: "); if (wlanSecret!="") { Serial.println("xxxx"); } else { Serial.println(); } }
  else if (cmdLine.startsWith("set gw")) { if (value=="gw" || value=="gwurl") { gwURL=""; } else { gwURL=value; } Serial.print("Gateway URL: "); Serial.println(gwURL); }
  else if (cmdLine.startsWith("connect wlan")) { connectWLAN(); }
  else if (cmdLine.startsWith("clear wlan")) { wlanSSID=""; httpStatus=0; connectWLAN(); }
  else if (cmdLine.startsWith("restart rx")) { modem.restartRx(true); Serial.println("Rx and PLL restarted"); }
  else if (cmdLine.startsWith("restart cpu")) { ESP.restart(); }
  else if (cmdLine.startsWith("get flash")) { getFlash(); }
  else if (cmdLine.startsWith("read flash")) { readFlash(); }
  else if (cmdLine.startsWith("write flash")) { writeFlash(); }
  else if (cmdLine.startsWith("erase flash")) { eraseFlash(); }
  else if (cmdLine.startsWith("help")) { help(); }
  Serial.print("> "); }

void cliWorker() {
  if (Serial.available()) {
    char serByte=Serial.read();
    if (serByte==127) { Serial.write(serByte); cmdLine.remove(cmdLine.length()-1); }
    else if (serByte==13 || serByte==10) { Serial.println(); doParse(); cmdLine=""; }
    else { Serial.write(serByte); cmdLine+=String(serByte); } } }
