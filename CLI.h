String cmdLine;

void help() {
  Serial.println("debug [0-3]");
  Serial.println("monitor rx");
  Serial.println("get version");
  Serial.println("get rxstat");
  Serial.println("get rxconf");
  Serial.println("set freq [137-525]");
  Serial.println("set bitrate [0.075-250]");
  Serial.println("set shift [0-200]");
  Serial.println("set rxbw [2.6-250|auto]");
  Serial.println("set afcbw [2.6-250|auto]");
  Serial.println("restart rx");
  Serial.println("restart cpu");
  Serial.println("get bos");
  Serial.println("set bos");
  Serial.println("help"); }

void doParse() {
  cmdLine.trim();
  String value=cmdLine.substring(cmdLine.lastIndexOf(" ")+1);
  if (cmdLine.startsWith("debug")) { modem.debug=value.toInt(); Serial.print("Debug Level: "); Serial.println(modem.debug); }
  if (cmdLine.startsWith("monitor rx")) { modem.monitorRx=!modem.monitorRx; }
  if (cmdLine.startsWith("get version")) { modem.printChip(); }
  if (cmdLine.startsWith("get rxstat")) { modem.printRx(); }
  if (cmdLine.startsWith("get rxconf")) {
    Serial.print("Center Frequency: "); Serial.print(modem.centerFreq,5); Serial.println(" MHz");
    Serial.print("Rx Frequency Offset: "); Serial.print(modem.rxOffset,3); Serial.println(" kHz");
    Serial.print("Bitrate: "); Serial.print(modem.bitrate*1000,0); Serial.println(" bps");
    Serial.print("Shift Frequency: +/- "); Serial.print(modem.shift*1000,0); Serial.println(" Hz");
    Serial.print("Rx Bandwidth: "); Serial.print(modem.rxBandwidth,1); Serial.println(" kHz");
    Serial.print("AFC Bandwidth: "); Serial.print(modem.afcBandwidth,1); Serial.println(" kHz"); }
  if (cmdLine.startsWith("set freq")) { modem.setFrequency(value.toDouble()); }
  if (cmdLine.startsWith("set bitrate")) { modem.setBitrate(value.toDouble()); }
  if (cmdLine.startsWith("set shift")) { modem.setShift(value.toDouble()); }
  if (cmdLine.startsWith("set rxbw")) { if (value=="auto") { modem.setRxBwAuto(); } else { modem.setRxBandwidth(value.toDouble()); } }
  if (cmdLine.startsWith("set afcbw")) { if (value=="auto") { modem.setAfcBwAuto(); } else { modem.setAfcBandwidth(value.toDouble()); } }
  if (cmdLine.startsWith("restart rx")) { modem.restartRx(true); Serial.println("Rx and PLL restarted"); }
  if (cmdLine.startsWith("restart cpu")) { ESP.restart(); }
  if (cmdLine.startsWith("get bos")) { Serial.print("BOS Mode: "); Serial.println(modem.isBOS); }
  if (cmdLine.startsWith("set bos")) { modem.isBOS=!modem.isBOS; Serial.print("BOS Mode: "); Serial.println(modem.isBOS); }
  if (cmdLine.startsWith("help")) { help(); }
  Serial.print("> "); }

void cliWorker() {
  if (Serial.available()) {
    char serByte=Serial.read();
    if (serByte==13 || serByte==10) { Serial.println(); doParse(); cmdLine=""; }
    else { Serial.write(serByte); cmdLine+=String(serByte); } } }
