#ifndef SX1278_ISR_H
#define SX1278_ISR_H

#ifndef HeltecLoRaV2
  #define DIO0 25
  #define DIO1 26
  #define DIO2 27
  #define DIO3 32
#else
  #define DIO0 26
  #define DIO1 35
  #define DIO2 34
  #define DIO3 32
#endif

volatile bool detectDIO0Flag=false;
volatile bool detectDIO3Flag=false;

portMUX_TYPE mutexDIO0=portMUX_INITIALIZER_UNLOCKED;
QueueHandle_t queueDIO1;
UBaseType_t queueSizeDIO1=1024;
portMUX_TYPE mutexDIO3=portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR dio0ISR() {
  portENTER_CRITICAL_ISR(&mutexDIO0);
    detectDIO0Flag=true;
  portEXIT_CRITICAL_ISR(&mutexDIO0); }

void IRAM_ATTR dio1ISR() {
  static uint8_t buffer=0; static uint8_t bufferMask=128;
  if (digitalRead(DIO2)==LOW) { buffer|=bufferMask; }
  bufferMask>>=1; if (bufferMask==0) {
    xQueueSendFromISR(queueDIO1,&buffer,NULL);
    buffer=0; bufferMask=128; } }

void IRAM_ATTR dio3ISR() {
  portENTER_CRITICAL_ISR(&mutexDIO3);
    detectDIO3Flag=true;
  portEXIT_CRITICAL_ISR(&mutexDIO3); }

#endif
