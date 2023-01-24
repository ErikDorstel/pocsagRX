String cmdLine;

void doParse() {
  cmdLine.trim();
  String value=cmdLine.substring(cmdLine.lastIndexOf(" ")+1);
  if (cmdLine.startsWith("debug")) { modem.debug=value.toInt(); Serial.print("Debug Level: "); Serial.println(modem.debug); }
  if (cmdLine.startsWith("monitor rx")) { modem.monitorRx=!modem.monitorRx; }
  if (cmdLine.startsWith("get bos")) { Serial.print("BOS Mode: "); Serial.println(modem.isBOS); }
  if (cmdLine.startsWith("set bos")) { modem.isBOS=!modem.isBOS; Serial.print("BOS Mode: "); Serial.println(modem.isBOS); }
  if (cmdLine.startsWith("get version")) { modem.printChip(); }
  if (cmdLine.startsWith("get rxstat")) { modem.printRx(); }
  if (cmdLine.startsWith("get rxconf")) {
    Serial.print("Center Frequency: "); Serial.print(modem.centerFreq,5); Serial.println(" MHz");
    Serial.print("Rx Frequency Offset: "); Serial.print(modem.rxOffset,3); Serial.println(" kHz");
    Serial.print("Bitrate: "); Serial.print(modem.bitrate*1000,0); Serial.println(" bps");
    Serial.print("Shift Frequency: +/- "); Serial.print(modem.shift*1000,0); Serial.println(" Hz");
    Serial.print("Rx Bandwidth: "); Serial.print(modem.rxBandwidth,1); Serial.println(" kHz");
    Serial.print("AFC Bandwidth: "); Serial.print(modem.afcBandwidth,1); Serial.println(" kHz"); } }

void cliWorker() {
  if (Serial.available()) {
    char serByte=Serial.read();
    if (serByte==13 || serByte==10) { Serial.println(); doParse(); cmdLine=""; }
    else { Serial.write(serByte); cmdLine+=String(serByte); } } }
