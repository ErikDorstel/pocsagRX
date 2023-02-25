#ifndef SX1278_TELNET_H
#define SX1278_TELNET_H

WiFiServer telnetServer(23);
WiFiClient telnetSession;

uint8_t sessionActive=0;
bool isAuth=false;
String telnetPass="pocsag";
String passBuffer="";
int isIAC=0;

void writeTelnet(const char value) { if (telnetSession.connected() && isAuth) { telnetSession.write(value); } }

void printTelnet(const char* value) { if (telnetSession.connected() && isAuth) { telnetSession.print(value); } }

void initTELNET() {
  telnetServer.begin();
  Log.writeTelnet=writeTelnet;
  Log.printTelnet=printTelnet; }

void telnetWorker() {
  if (((!telnetSession) || (!isAuth)) && telnetServer.hasClient()) {
    telnetSession=telnetServer.available();
    sessionActive=1; isAuth=false; passBuffer="";
    Log.print(0,"TELNET Session from %s connected\r\n",telnetSession.remoteIP().toString().c_str());
    telnetSession.print("Password: "); }

  if ((sessionActive || telnetSession) && (!telnetSession.connected())) {
    telnetSession.stop();
    sessionActive=0; isAuth=false; passBuffer="";
    Log.print(0,"TELNET Session disconnected\r\n"); }

  if (telnetSession && (!isAuth) && telnetSession.available()) {
    char telnetByte=telnetSession.read();
    if (isIAC==3) { isIAC=0; }
    if (telnetByte==255 && (!isIAC)) { isIAC++; }
    else if (telnetByte==255 && isIAC) { isIAC=0; }
    else if (isIAC) { isIAC++; }
    else if (telnetByte==127) { telnetSession.write(telnetByte); passBuffer.remove(passBuffer.length()-1); }
    else if (telnetByte==10) {}
    else if (telnetByte==13) {
      if (passBuffer==telnetPass) { isAuth=true; Log.print(0,"TELNET Authentication from %s passed\r\n> ",telnetSession.remoteIP().toString().c_str()); passBuffer=""; Log.needCR=true; }
      else { if (passBuffer!="") { Log.print(0,"TELNET Authentication from %s failed\r\n",telnetSession.remoteIP().toString().c_str()); } passBuffer=""; telnetSession.print("Password: "); } }
    else { passBuffer+=String(telnetByte); } } }

#endif
