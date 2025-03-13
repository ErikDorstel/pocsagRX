#ifndef SX1278_WLAN_H
#define SX1278_WLAN_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

WiFiClientSecure client;
HTTPClient https;

String wlanSSID="";
String wlanSecret="";
String gwURL="";
String rawURL="";
uint32_t upEvents;
uint32_t downEvents;
int httpStatus;
int httpMaxRetries=2;
uint32_t httpRetried;
uint32_t httpFailed;
uint32_t timerWLAN;
bool hasIP=false;
int waitConnect=0;

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Log.print(0,"WLAN AP: %s with %i dBm connected\r\n",WiFi.SSID(),WiFi.RSSI());
  upEvents++; hasIP=true; }

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Log.print(0,"WLAN AP: disconnected\r\n");
  downEvents++; hasIP=false; }

void initWLAN() {
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(WiFiStationConnected,ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected,ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.setHostname("pocsagGateway");
  WiFi.setAutoConnect(false); WiFi.setAutoReconnect(false);
  client.setInsecure(); https.setReuse(true); timerWLAN=millis()+2000; }

void connectWLAN() {
  WiFi.disconnect();
  if (wlanSSID!="" && wlanSecret!="") {
    waitConnect=10; Log.print(0,"WLAN AP: try connect\r\n");
    WiFi.begin(wlanSSID.c_str(),wlanSecret.c_str()); } }

void postHTTPS(String url, String postData) {
  uint8_t attempt=0; while (attempt<=httpMaxRetries) {
    if (hasIP) { client.connect(url.c_str(),443); https.begin(client,url);
      https.addHeader("Content-Type","application/x-www-form-urlencoded");
      httpStatus=https.POST(postData);
      Log.print(2,"    HTTP POST: %s\r\n",postData.c_str());
      Log.print(1,"    HTTP Attempt: %i    Status: %i\r\n",attempt+1,httpStatus);
      https.end(); client.stop(); } else { httpStatus=0; }
    attempt++; if (httpStatus==200) { break; } }
  if (attempt>httpMaxRetries && httpStatus!=200) { httpFailed++; }
  else if (attempt>1) { httpRetried++; } }

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

void wlanWorker() {
  if (millis()>=timerWLAN) { timerWLAN=millis()+2000; WiFi.RSSI();
      if (!hasIP) {
        if (waitConnect>0) { waitConnect--; }
        if (wlanSSID!="" && wlanSecret!="" && waitConnect==0) { connectWLAN(); } }
      else { waitConnect=0; } } }

#endif
