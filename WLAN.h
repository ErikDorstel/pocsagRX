#ifndef SX1278_WLAN_H
#define SX1278_WLAN_H

#include <WiFi.h>

String wlanSSID="";
String wlanSecret="";
String gwURL="";

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) { Serial.println("WLAN AP " + WiFi.SSID() + " connected"); }

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) { Serial.println("WLAN AP disconnected"); }

void initWLAN() {
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(WiFiStationConnected,ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected,ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
  WiFi.setHostname("pocsagGateway"); }

void connectWLAN() {
  WiFi.disconnect();
  if (wlanSSID!="" && wlanSecret!="") { WiFi.begin(wlanSSID.c_str(),wlanSecret.c_str()); } }

#endif
