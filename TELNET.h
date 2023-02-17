#ifndef SX1278_TELNET_H
#define SX1278_TELNET_H

WiFiServer telnetServer(23);
WiFiClient telnetSession;

void writeTelnet(const char value) { if (telnetSession.connected()) { telnetSession.write(value); } }

void printTelnet(const char* value) { if (telnetSession.connected()) { telnetSession.print(value); } }

void initTELNET() {
  telnetServer.begin();
  Log.writeTelnet=writeTelnet;
  Log.printTelnet=printTelnet; }

void telnetWorker() {
  if ((!telnetSession) && telnetServer.hasClient()) {
    Log.print(0,"Telnet Session connected\r\n");
    telnetSession=telnetServer.available(); }

  if (telnetSession) { if (!telnetSession.connected()) {
    Log.print(0,"Telnet Session disconnected\r\n");
    telnetSession.stop(); } } }

#endif
