#ifndef SX1278_FSK_H
#define SX1278_FSK_H

#include <SPI.h>
#include "SX1278ISR.h"

#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 5
#define RST 16

#define regOpMode 0x1
#define regBrMSB 0x2
#define regBrLSB 0x3
#define regShiftMSB 0x4
#define regShiftLSB 0x5
#define regFreqMSB 0x6
#define regFreqMID 0x7
#define regFreqLSB 0x8
#define regRxLna 0xc
#define regRxCfg 0xd
#define regRssiTresh 0x10
#define regRssi 0x11
#define regRxBw 0x12
#define regAfcBw 0x13
#define regOokPeak 0x14
#define regAfcFei 0x1a
#define regAfcMSB 0x1b
#define regAfcLSB 0x1c
#define regFeiMSB 0x1d
#define regFeiLSB 0x1e
#define regPreambleDetect 0x1f
#define regRxTimeout2 0x21
#define regRxTimeout3 0x22
#define regSynCfg 0x27
#define regSynByte1 0x28
#define regSynByte2 0x29
#define regSynByte3 0x2a
#define regSynByte4 0x2b
#define regPckCfg1 0x30
#define regPckCfg2 0x31
#define regPckLength 0x32
#define regSeqConfig1 0x36
#define regSeqConfig2 0x37
#define regIrqFlags1 0x3e
#define regDioMap1 0x40
#define regDioMap2 0x41
#define regChipVersion 0x42

#define gainValues {0,0,-6,-12,-24,-36,-48,0}

double bwValues[21]=  { 2.6, 3.1, 3.9, 5.2, 6.3, 7.8, 10.4, 12.5, 15.6, 20.8, 25, 31.3, 41.7, 50, 62.5, 83.3, 100, 125, 166.7, 200, 250 };
uint8_t bwIntegers[21]={ 23,  15,   7,  22,  14,   6,   21,   13,    5,   20, 12,    4,    19,11,    3,   18,  10,   2,    17,   9,   1 };

uint8_t syncWord[4]={0x7c,0xd2,0x15,0xd8};

uint8_t bcdCodes[16]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x2a,0x55,0x20,0x2d,0x29,0x28};

class SX1278FSK {
  public:
    bool monitorRx;
    uint8_t debug;
    bool isBOS;
    double centerFreq;
    double rxOffset;
    double bitrate;
    double shift;
    double rxBandwidth;
    double afcBandwidth;

    SX1278FSK(bool _monitorRx=false, uint8_t _debug=0) {
      monitorRx=_monitorRx; debug=_debug; }

    uint8_t readSPI(uint8_t addr) {
      digitalWrite(CS, LOW);
      SPI.transfer(addr);
      uint8_t value = SPI.transfer(0x00);
      digitalWrite(CS, HIGH);
      return value; }

    void writeSPI(uint8_t addr, uint8_t value) {
      digitalWrite(CS, LOW);
      SPI.transfer(addr | 0x80);
      SPI.transfer(value);
      digitalWrite(CS, HIGH); }

    void setReg(uint8_t addr, uint8_t msb, uint8_t lsb, uint8_t value) {
      uint8_t oldValue=readSPI(addr);
      uint16_t mask=(1<<(msb-lsb+1))-1;
      value&=mask; mask<<=lsb; value<<=lsb;
      uint8_t newValue=(oldValue&(~mask))|value;
      writeSPI(addr,newValue); }

    uint8_t getReg(uint8_t addr, uint8_t msb, uint8_t lsb) {
      uint8_t value=readSPI(addr);
      uint16_t mask=(1<<(msb-lsb+1))-1;
      return (value&(mask<<lsb))>>lsb; }

    void initSPI() {
      pinMode(CS, OUTPUT); digitalWrite(CS, HIGH);
      SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0)); }

    void resetChip() {
      digitalWrite(RST, LOW); delay(1);
      digitalWrite(RST, HIGH); delay(25); }

    void initChip() {
      pinMode(RST, OUTPUT); digitalWrite(RST, HIGH);
      delay(25); initSPI(); resetChip(); printChip(); }

    void regDump() {
      for (uint8_t reg=0x00;reg<=0x42;reg++) { uint8_t value=readSPI(reg);
      Serial.print(reg,HEX); Serial.print(" : "); Serial.print(value,HEX); Serial.print("\t"); Serial.print(value,DEC); Serial.print("\t"); Serial.println(value,BIN); } }

    void setFrequency(double _centerFreq, double _rxOffset=0) {
      centerFreq=_centerFreq; rxOffset=_rxOffset;
      Serial.print("Center Frequency: "); Serial.print(centerFreq+(rxOffset/1000),5); Serial.println(" MHz");
      uint32_t value=(uint32_t)round((centerFreq+(rxOffset/1000))*(1<<14));
      writeSPI(regFreqMSB,(value & 0xFF0000) >> 16);
      writeSPI(regFreqMID,(value & 0x00FF00) >> 8);
      writeSPI(regFreqLSB,value & 0x0000FF); }

    void setBitrate(double _bitrate) {
      bitrate=_bitrate;
      Serial.print("Bitrate: "); Serial.print(bitrate*1000,0); Serial.println(" bps");
      uint16_t value=(uint16_t)round(32000.0/bitrate);
      writeSPI(regBrMSB,(value & 0xFF00) >> 8);
      writeSPI(regBrLSB,value & 0x00FF); }

    void setShift(double _shift) {
      shift=_shift;
      Serial.print("Shift: +/- "); Serial.print(shift*1000,0); Serial.println(" Hz");
      uint16_t value=(uint16_t)round(shift*(1<<11)/125.0);
      writeSPI(regShiftMSB,(value & 0xFF00) >> 8);
      writeSPI(regShiftLSB,value & 0x00FF); }

    void setRxBandwidth(double _rxBandwidth) {
      uint8_t selected=20; for (uint8_t idx=0;idx<=20;idx++) {
        if (bwValues[idx]>=_rxBandwidth) { selected=idx; break; } }
      Serial.print("Rx Bandwidth: "); Serial.print(bwValues[selected],1); Serial.println(" kHz");
      setReg(regRxBw,4,0,bwIntegers[selected]); rxBandwidth=bwValues[selected]; }

    void setRxBwAuto() { setRxBandwidth(shift+(bitrate/2)); }

    void setAfcBandwidth(double _afcBandwidth) {
      uint8_t selected=20; for (uint8_t idx=0;idx<=20;idx++) {
        if (bwValues[idx]>=_afcBandwidth) { selected=idx; break; } }
      Serial.print("AFC Bandwidth: "); Serial.print(bwValues[selected],1); Serial.println(" kHz");
      setReg(regAfcBw,4,0,bwIntegers[selected]); afcBandwidth=bwValues[selected]; }

    void setAfcBwAuto(double error=12) { setAfcBandwidth(2*(shift+(bitrate/2))+error); }

    void setModeFskRxCont() {
      setReg(regOpMode,7,7,0); // Mode 0:FSK/OOK 1:LoRa
      setReg(regOpMode,6,5,0); // Modulation 0:FSK 1:OOK
      setReg(regOpMode,3,3,1); // Frequency Mode 0:High 1:Low
      setReg(regOpMode,2,0,1); // Transceiver Mode 0:Sleep 1:Standby 2:FSTx 3:Tx 4:FSRx 5:Rx
      setReg(regOokPeak,5,5,1); // Bit Synchronizer 0:Off 1:On
      setReg(regRxCfg,7,7,0); // Restart Rx on Collision 0:Off 1:On
      setReg(regRxCfg,4,4,1); // AFC Auto 0:Off 1:On
      setReg(regRxCfg,3,3,1); // AGC Auto 0:Off 1:On
      setReg(regRxCfg,2,0,6); // Rx Interrupt triggers 6:Preamble AGC+AFC 7:Preamble+RSSI AGC+AFC
      setReg(regAfcFei,0,0,0); // AFC Auto Clear 0:Off 1:On
      setReg(regPreambleDetect,7,7,1); // Preamble Detector 0:Off 1:On
      setReg(regPreambleDetect,6,5,1); // Preamble Detector Size 0:1 Byte 1:2 Bytes 2:3 Bytes
      setReg(regPreambleDetect,4,0,10); // Preamble Detector Errors tolerated
      setReg(regSynCfg,5,5,0); // Preamble Polarity 0:AA 1:55
      setReg(regSynCfg,4,4,1); // Sync Word Rx/Tx Processing 0:Off 1:On
      setReg(regSynCfg,2,0,3); // Sync Byte Count x:x+1
      writeSPI(regSynByte1,syncWord[0]); // Frame Sync Byte 1
      writeSPI(regSynByte2,syncWord[1]); // Frame Sync Byte 2
      writeSPI(regSynByte3,syncWord[2]); // Frame Sync Byte 3
      writeSPI(regSynByte4,syncWord[3]); // Frame Sync Byte 4
      setReg(regPckCfg1,7,7,1); // Packet Length 0:Fixed 1:Variable
      setReg(regPckCfg1,6,5,0); // DC Free 0:Off 1:Manchester 2:Whitening
      setReg(regPckCfg1,4,4,0); // CRC Rx/Tx Processing 0:Off 1:On
      setReg(regPckCfg1,2,1,0); // Address Filtering 0:Off
      setReg(regPckCfg2,6,6,0); // Data Mode 0:Continuous 1:Packet
      setReg(regPckCfg2,2,0,7); // Packet Length MSB
      setReg(regPckLength,7,0,255); } // Packet Length LSB

    void startSequencer() {
      setReg(regOpMode,2,0,1); // Transceiver Mode 0:Sleep 1:Standby 2:FSTx 3:Tx 4:FSRx 5:Rx
      setReg(regRxTimeout2,7,0,4); // Preamble Timeout 0:Off x:x*16*Tbit
      setReg(regRxTimeout3,7,0,0); // Frame Sync Timeout 0:Off x:x*16*Tbit
      setReg(regSeqConfig1,4,3,1); // Sequence from Start to 0:Low Power 1:Rx 2:Tx
      setReg(regSeqConfig2,4,3,0); // Sequence from Rx Timeout to 0:Rx Restart 1:Tx 2:Low Power 3:Off
      setReg(regSeqConfig1,7,7,1); } // Sequencer Start

    void stopSequencer() {
      setReg(regSeqConfig1,6,6,1); // Sequencer Stop
      setReg(regRxTimeout2,7,0,0); // Preamble Timeout 0:Off x:x*16*Tbit
      delay(100);
      setReg(regOpMode,2,0,4); } // Transceiver Mode 0:Sleep 1:Standby 2:FSTx 3:Tx 4:FSRx 5:Rx

    void initDioIf() {
      setReg(regDioMap1,7,6,1); // DIO0 Mapping 0:Sync Word 1:RSSI/Preamble Detect
      setReg(regDioMap1,5,4,0); // DIO1 Mapping 0:Clock
      setReg(regDioMap1,3,2,0); // DIO2 Mapping 0:Data
      setReg(regDioMap1,1,0,0); // DIO3 Mapping 0:Timeout 1:RSSI/Preamble Detect
      setReg(regDioMap2,0,0,1); // Map Detect Interrupt 0:RSSI 1:Preamble
      pinMode(DIO0, INPUT); pinMode(DIO1, INPUT); pinMode(DIO2, INPUT); pinMode(DIO3, INPUT);
      attachInterrupt(DIO0,dio0ISR,RISING);
      attachInterrupt(DIO1,dio1ISR,RISING);
      attachInterrupt(DIO3,dio3ISR,RISING); }

    bool available() { if (writePtr==readPtr) { return false; } else { return true; } }

    uint8_t read() { uint8_t lastByte=ringBuffer[readPtr]; readPtr++; readPtr%=128; return lastByte; }

    void restartRx(bool withPLL) {
      if (!withPLL) { setReg(regRxCfg,6,6,1); }
      else { setReg(regRxCfg,5,5,1); } }

    double getAFC() {
      uint8_t valueMSB=readSPI(regAfcMSB);
      uint8_t valueLSB=readSPI(regAfcLSB);
      int16_t value=(valueMSB<<8)|valueLSB;
      return (double)value/16.384; }

    double getFEI() {
      uint8_t valueMSB=readSPI(regFeiMSB);
      uint8_t valueLSB=readSPI(regFeiLSB);
      int16_t value=(valueMSB<<8)|valueLSB;
      return (double)value/16.384; }

    double getGain() { double gain[8]=gainValues; return gain[getReg(regRxLna,7,5)]; }

    void setRssiTresh(int tresh) {
      writeSPI(regRssiTresh,(uint8_t)(tresh*-2)); }

    double getRSSI() { return readSPI(regRssi)/-2.0; }

    uint8_t searchSync(uint8_t rxByte) {
      static uint32_t syncBuffer;
      for (uint8_t bitPos=0;bitPos<=7;bitPos++) {
        syncBuffer<<=1; syncBuffer|=(rxByte&128)>>7; rxByte<<=1;
        if (syncBuffer==0x7cd215d8) { return 7-bitPos; } }
      return 255; }

    bool checkParity(uint32_t codeWord) {
      uint8_t count=0; for (uint8_t idx=0;idx<=31;idx++) {
        if (codeWord&(1<<idx)) { count++; } }
      if (count%2) { return false; } else { return true; } }

    void printChip() {
      Serial.print("SX1278 Chip Version: "); Serial.print(getReg(regChipVersion,7,4),DEC);
      Serial.print(" Hardware Revision: "); Serial.println(getReg(regChipVersion,3,0),DEC); }

    void printRx() {
      Serial.print("RSSI: "); Serial.print(getRSSI(),1); Serial.print(" dBm");
      Serial.print("   Gain: "); Serial.print(getGain(),1); Serial.print(" dBm");
      Serial.print("   AFC: "); Serial.print(getAFC(),3); Serial.print(" kHz");
      Serial.print("   FEI: "); Serial.print(getFEI(),3); Serial.println(" kHz"); }

    void beginPOCSAG(bool _isBOS=false) {
      isBOS=_isBOS;
      setModeFskRxCont();
      initDioIf();
      restartRx(true);
      startSequencer();
      timerRx=millis()+1000;
      Serial.println("POCSAG Rx started ..."); }

    void consoleDE(uint8_t code) {
      if ((code>0x0 && code<0x20) || code>0x7e) { Serial.print("["); Serial.print(code,DEC); Serial.print("]"); }
      else { switch(code) {
        case 0x0: break;
        case 0x5b: Serial.print("\u00c4"); break;
        case 0x5c: Serial.print("\u00d6"); break;
        case 0x5d: Serial.print("\u00dc"); break;
        case 0x7b: Serial.print("\u00e4"); break;
        case 0x7c: Serial.print("\u00f6"); break;
        case 0x7d: Serial.print("\u00fc"); break;
        case 0x7e: Serial.print("\u00df"); break;
        default: Serial.write(code); } } }

    void decodePOCSAG() {
      if (millis()>=timerRx) { timerRx=millis()+1000;
        restartRx(false);
        if (monitorRx) { if (needCR) { needCR=false; Serial.println(); } printRx(); } }

      if (detectDIO0Flag) { detectDIO0Flag=false;
        if (needCR) { needCR=false; Serial.println(); }
        if (debug>1) { Serial.println("Preamble Detected!"); }
        if (rxOffset==0) { rxOffset=getAFC(); Serial.print("Auto Rx Offset: "); Serial.print(rxOffset,3); Serial.println(" kHz"); }
        if (debug) { printRx(); }
        if (isBOS) { isText=false; } else { isText=true; }
        timerRx=millis()+1000; }

      if (available()) {
        uint8_t rxByte=read();
        uint8_t bitShift=searchSync(rxByte);

        if (bitShift!=255) {
          timerRx=millis()+1000;
          if (debug>1) {
            if (needCR) { needCR=false; Serial.println(); }
            Serial.print("Frame Sync Detected! Bit Shift: "); Serial.println(bitShift); }
          uint32_t batch[16]={0};

          for (uint8_t idx=0;idx<=63;idx++) {
            uint8_t codeWord=idx>>2;
            batch[codeWord]<<=bitShift; batch[codeWord]|=rxByte&((1<<bitShift)-1);
            while (!available()) {} rxByte=read();
            batch[codeWord]<<=8-bitShift; batch[codeWord]|=rxByte>>bitShift;
            if (idx==63 && bitShift!=0) { searchSync(rxByte); } }

          for (uint8_t idx=0;idx<=15;idx++) {
            if (batch[idx]==0x7a89c197) { isIdle=true; } else { isIdle=false; }

            if (!(batch[idx]&(1<<31))) { isAddress=true; } else { isAddress=false; }

            if (checkParity(batch[idx])) { parity=true; } else { parity=false; }

            if (debug>2) {
              if (needCR) { needCR=false; Serial.println(); }
              if (isIdle) { Serial.print("Idle "); }
              if (isAddress) { Serial.print("Address "); } else { Serial.print("Message "); }
              Serial.print(batch[idx],BIN);
              if (parity) { Serial.println(" Parity Ok"); } else { Serial.println(" Parity Failed"); } }

            if ((!isIdle) && isAddress) {
              if (needCR) { needCR=false; Serial.println(); }
              ric=((batch[idx]&0x7fffe000)>>10)|(idx>>1);
              if (debug) { Serial.print("  RIC: "); Serial.println(ric,DEC); }
              function=(batch[idx]&0x1800)>>11;
              if (isBOS) {
                switch(function) {
                  case 0b00: if (debug) { Serial.println("    Sub RIC: A"); } isText=true; break;
                  case 0b01: if (debug) { Serial.println("    Sub RIC: B"); } isText=true; break;
                  case 0b10: if (debug) { Serial.println("    Sub RIC: C"); } isText=true; break;
                  default: if (debug) { Serial.println("    Sub RIC: D"); } isText=true; break; } }
              else {
                switch(function) {
                  case 0b00: if (debug) { Serial.println("    Message Type: Numeric"); } isText=false; break;
                  case 0b01: if (debug) { Serial.println("    Message Type: 1"); } isText=false; break;
                  case 0b10: if (debug) { Serial.println("    Message Type: 2"); } isText=false; break;
                  default: if (debug) { Serial.println("    Message Type: Text"); } isText=true; } } }

            if (isAddress) { text=0; textPos=0; number=0; numberPos=0; }

            if ((!isAddress) && (isText)) {
              if ((!needCR) && debug) { needCR=true; Serial.print("    "); }
              for (uint8_t bitPos=30;bitPos>=11;bitPos--) {
                text>>=1; text|=(batch[idx]&(1<<bitPos))>>(bitPos-7);
                textPos++; if (textPos>=7) { text>>=1; if (debug) { consoleDE(text); } text=0; textPos=0; } } }

            if ((!isAddress) && (!isText)) {
              if ((!needCR) && debug) { needCR=true; Serial.print("    "); }
              for (uint8_t bitPos=30;bitPos>=11;bitPos--) {
                number<<=1; number|=(batch[idx]&(1<<bitPos))>>bitPos;
                numberPos++; if (numberPos>=4) { if (number<=15 && debug) { Serial.write(bcdCodes[number]); } number=0; numberPos=0; } } } } } } }

  private:
    uint32_t timerRx;
    bool needCR=false;
    bool isText;
    bool isIdle;
    bool isAddress;
    uint32_t ric;
    uint8_t function;
    bool parity;
    uint8_t text=0;
    uint8_t textPos=0;
    uint8_t number=0;
    uint8_t numberPos=0; };

#endif
