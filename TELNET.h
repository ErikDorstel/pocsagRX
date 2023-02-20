#ifndef SX1278_TELNET_H
#define SX1278_TELNET_H

WiFiServer telnetServer(23);
WiFiClient telnetSession;

uint8_t sessionActive=0;

void writeTelnet(const char value) { if (telnetSession.connected()) { telnetSession.write(value); } }

void printTelnet(const char* value) { if (telnetSession.connected()) { telnetSession.print(value); } }

void initTELNET() {
  telnetServer.begin();
  Log.writeTelnet=writeTelnet;
  Log.printTelnet=printTelnet; }

void telnetWorker() {
  if ((!telnetSession) && telnetServer.hasClient()) {
    telnetSession.flush(); telnetSession=telnetServer.available();
    telnetSession.flush(); sessionActive=1;
    Log.print(0,"Telnet Session from %s connected\r\n>",telnetSession.remoteIP().toString().c_str()); Log.needCR=true; }

  if ((sessionActive || telnetSession) && (!telnetSession.connected())) {
    telnetSession.flush(); telnetSession.stop();
    telnetSession.flush(); sessionActive=0;
    Log.print(0,"Telnet Session disconnected\r\n"); } }

#endif
