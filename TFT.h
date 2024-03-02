#ifndef SX1278_TFT_H
#define SX1278_TFT_H

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>

#define TFT_MISO  12
#define TFT_MOSI  13
#define TFT_CLK   14
#define TFT_CS    15
#define TFT_DC    17
#define TFT_RST    4
#define TFT_LED   32
#define Touch_CS  33
#define Touch_IRQ 34

SPIClass *hspi=new SPIClass();
Adafruit_ILI9341 tft=Adafruit_ILI9341(hspi,TFT_DC,TFT_CS,TFT_RST);
XPT2046_Touchscreen ts(Touch_CS,Touch_IRQ);

uint32_t tftTimer=0;

void tftOn() { digitalWrite(TFT_LED,LOW); tftTimer=millis()+10000; }
void tftOff() { digitalWrite(TFT_LED,LOW); }

void initTFT() {
  pinMode(TFT_LED,OUTPUT); tftOff();
  hspi->begin(TFT_CLK,TFT_MISO,TFT_MOSI);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(0,0);
  ts.begin(*hspi);
  ts.setRotation(1); }

void tftWorker() { if (tftTimer && millis()>=tftTimer) { tftTimer=0; tftOff(); } }

#endif
