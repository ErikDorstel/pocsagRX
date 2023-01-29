#include "Flash.h"

String cmdLine;

void help() {
  Serial.println("debug [0-3]");
  Serial.println("monitor");
  Serial.println("get version");
  Serial.println("get stat");
  Serial.println("get conf");
  Serial.println("get register");
  Serial.println("set freq [137-525]");
  Serial.println("set offset [0-100|auto]");
  Serial.println("set bitrate [0.075-250]");
  Serial.println("set shift [0.6-200]");
  Serial.println("set rxbw [2.6-250|auto]");
  Serial.println("set afcbw [2.6-250|auto]");
  Serial.println("set bos");
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
  if (cmdLine.startsWith("debug")) { modem.debug=value.toInt(); Serial.print("Debug Level: "); Serial.println(modem.debug); }
  else if (cmdLine.startsWith("monitor")) { modem.monitorRx=!modem.monitorRx; }
  else if (cmdLine.startsWith("get version")) { modem.printChip(); }
  else if (cmdLine.startsWith("get stat")) { modem.printRx(); }
  else if (cmdLine.startsWith("get conf")) {
    Serial.print("Center Frequency: "); Serial.print(modem.centerFreq,5); Serial.println(" MHz");
    Serial.print("Rx Frequency Offset: "); Serial.print(modem.rxOffset,3); Serial.println(" kHz");
    Serial.print("Bitrate: "); Serial.print(modem.bitrate*1000,0); Serial.println(" bps");
    Serial.print("Shift Frequency: +/- "); Serial.print(modem.shift*1000,0); Serial.println(" Hz");
    Serial.print("Rx Bandwidth: "); Serial.print(modem.rxBandwidth,1); Serial.println(" kHz");
    Serial.print("AFC Bandwidth: "); Serial.print(modem.afcBandwidth,1); Serial.println(" kHz");
    Serial.print("BOS Mode: "); Serial.println(modem.isBOS); }
  else if (cmdLine.startsWith("get register")) { modem.regDump(); }
  else if (cmdLine.startsWith("set freq")) { modem.stopSequencer(); modem.setFrequency(value.toDouble()); modem.startSequencer(); modem.restartRx(true); }
  else if (cmdLine.startsWith("set offset")) { if (value!="auto") { modem.rxOffset=value.toDouble(); }
    modem.stopSequencer(); modem.setFrequency(modem.centerFreq,modem.rxOffset); modem.startSequencer(); modem.restartRx(true); }
  else if (cmdLine.startsWith("set bitrate")) { modem.setBitrate(value.toDouble()); }
  else if (cmdLine.startsWith("set shift")) { modem.stopSequencer(); modem.setShift(value.toDouble()); modem.startSequencer(); modem.restartRx(true); }
  else if (cmdLine.startsWith("set rxbw")) { if (value=="auto") { modem.setRxBwAuto(); } else { modem.setRxBandwidth(value.toDouble()); } }
  else if (cmdLine.startsWith("set afcbw")) { if (value=="auto") { modem.setAfcBwAuto(); } else { modem.setAfcBandwidth(value.toDouble()); } }
  else if (cmdLine.startsWith("set bos")) { modem.isBOS=!modem.isBOS; Serial.print("BOS Mode: "); Serial.println(modem.isBOS); }
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
