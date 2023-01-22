#include "SX1278.h"

SX1278FSK modem;

bool monitorRx=false;
bool debug=false;

uint32_t timerRSSI;

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK, MISO, MOSI, CS);
  modem.initChip();
  modem.printChip();
  timerRSSI=millis()+1000;
  modem.setFrequency(439.995);
  //modem.setFrequency(433.33);
  modem.setBitrate(1.2);
  modem.setShift(4.5);
  modem.setRxBandwidth(17);
  modem.setAfcBandwidth(10);
  modem.setModeFskRxCont();
  modem.initDioIf();
  modem.restartRx(true);
  //modem.regDump();
  modem.startSequencer(); }

void loop() {
  static bool isIdle; static bool isAddress; static uint8_t function; static bool parity; static uint32_t ric;
  static uint8_t text=0; static uint8_t textPos=0; static uint8_t number=0; static uint8_t numberPos=0;

  if (millis()>=timerRSSI) { timerRSSI=millis()+1000;
    modem.restartRx(false);
    if (monitorRx) { modem.printRx(); } }

  if (detectDIO0Flag) { detectDIO0Flag=false; Serial.println("Preamble Detected!"); timerRSSI=millis()+1000; modem.printRx(); }

  if (modem.available()) {
    uint8_t rxByte=modem.read();
    uint8_t bitShift=modem.searchSync(rxByte);

    if (bitShift!=255) {
      timerRSSI=millis()+1000;
      Serial.print("Frame Sync Detected! Bit Shift: "); Serial.println(bitShift);
      uint32_t batch[16]={0};

      for (uint8_t idx=0;idx<=63;idx++) {
        uint8_t codeWord=idx>>2;
        batch[codeWord]<<=bitShift; batch[codeWord]|=rxByte&((1<<bitShift)-1);
        while (!modem.available()) {} rxByte=modem.read();
        batch[codeWord]<<=8-bitShift; batch[codeWord]|=rxByte>>bitShift;
        if (idx==63 && bitShift!=0) { modem.searchSync(rxByte); } }

      for (uint8_t idx=0;idx<=15;idx++) {
        if (batch[idx]==0x7a89c197) { isIdle=true; } else { isIdle=false; }

        if (!(batch[idx]&(1<<31))) { isAddress=true; } else { isAddress=false; }

        if (modem.checkParity(batch[idx])) { parity=true; } else { parity=false; }

        if (debug) {
          Serial.println();
          if (isIdle) { Serial.print("Idle "); }
          if (isAddress) { Serial.print("Address "); } else { Serial.print("Message "); }
          Serial.print(batch[idx],BIN);
          if (parity) { Serial.println(" Parity Ok"); } else { Serial.println(" Parity Failed"); } }

        if ((!isIdle) && isAddress) {
          ric=((batch[idx]&0x7fffe000)>>10)|(idx>>1); Serial.print("    RIC: "); Serial.println(ric,DEC);
          function=(batch[idx]&0x1800)>>11; switch(function) {
            case 0: Serial.println("    Message Type: Numeric"); Serial.print("    "); break;
            case 1: Serial.println("    Message Type: 1"); Serial.print("    "); break;
            case 2: Serial.println("    Message Type: 2"); Serial.print("    "); break;
            default: Serial.println("    Message Type: Text"); Serial.print("    "); } }

        if (isAddress) { text=0; textPos=0; number=0; numberPos=0; }

        if ((!isIdle) && (!isAddress) && (function==3)) {
          for (uint8_t bitPos=30;bitPos>=11;bitPos--) {
            text>>=1; text|=(batch[idx]&(1<<bitPos))>>(bitPos-7);
            textPos++; if (textPos>=7) { text>>=1; Serial.write(text); text=0; textPos=0; } } }

        if ((!isIdle) && (!isAddress) && (function==0)) {
          for (uint8_t bitPos=30;bitPos>=11;bitPos--) {
            number<<=1; number|=(batch[idx]&(1<<bitPos))>>bitPos;
            numberPos++; if (numberPos>=4) { if (number<=15) { Serial.write(bcdCode[number]); } number=0; numberPos=0; } } } }

      Serial.println(); } } }
