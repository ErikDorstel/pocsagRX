#ifndef SX1278_WLAN_H
#define SX1278_WLAN_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

WiFiClientSecure *client=new WiFiClientSecure;
HTTPClient https;

String wlanSSID="";
String wlanSecret="";
String gwURL="";
int httpStatus;

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) { if (modem.needCR) { modem.needCR=false; Serial.println(); } Serial.println("WLAN AP: " + WiFi.SSID() + " connected"); }

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) { if (modem.needCR) { modem.needCR=false; Serial.println(); } Serial.println("WLAN AP: disconnected"); }

void initWLAN() {
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(WiFiStationConnected,ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected,ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.setHostname("pocsagGateway"); }

void connectWLAN() {
  WiFi.disconnect();
  if (wlanSSID!="" && wlanSecret!="") { WiFi.begin(wlanSSID.c_str(),wlanSecret.c_str()); } }

void postHTTPS(String postData) {
  if (WiFi.status()==WL_CONNECTED && gwURL!="" && postData!="") {
    client->setInsecure();
    https.begin(*client,gwURL);
    httpStatus=https.POST(postData);
    if (modem.debug) {
      if (modem.needCR) { modem.needCR=false; Serial.println(); }
      if (modem.debug>1) { Serial.print("    HTTP POST: "); Serial.println(postData); }
      Serial.print("    HTTP Status: "); Serial.println(httpStatus); }
    https.end(); } }

String urlencode(String value) {
  String result=""; char x; char x0; char x1;
  for (int idx=0;idx<value.length();idx++) {
    x=value.charAt(idx);
    if (x==' ') { result+='+'; }
    else if (isalnum(x)) { result+=x; }
    else { x1=(x&0xf)+'0';
      if ((x & 0xf)>9) { x1=(x&0xf)-10+'A'; }
      x=(x>>4)&0xf; x0=x+'0';
      if (x>9) { x0=x-10+'A'; }
      result+='%'; result+=x0; result+=x1; } }
  return result; }

#endif
