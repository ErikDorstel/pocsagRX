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
    telnetSession=telnetServer.available();
    sessionActive=1;
    Log.print(0,"Telnet Session connected\r\n> "); }

  if (sessionActive && (!telnetSession.connected())) {
    sessionActive=0;
    Log.print(0,"Telnet Session disconnected\r\n"); }

  if (telnetSession && (!telnetSession.connected())) {
    Log.print(0,"inactive Telnet Session terminated\r\n");
    telnetSession.stop(); } }

#endif
