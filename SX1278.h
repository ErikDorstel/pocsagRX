#ifndef SX1278_FSK_H
#define SX1278_FSK_H

#include "Log.h"
Logging Log;
#include "BCH3121.h"
CBCH3121 bch;
#include <SPI.h>
#include "SX1278ISR.h"
#include "WLAN.h"
#include "TELNET.h"

#ifndef HeltecLoRaV2
  #define SCK 18
  #define MISO 19
  #define MOSI 23
  #define CS 5
  #define RST 16
#else
  #define SCK 5
  #define MISO 19
  #define MOSI 27
  #define CS 18
  #define RST 14
#endif

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

struct globalErrors { uint32_t corrected; uint32_t uncorrected; };

const double gainValues[8]={0,0,-6,-12,-24,-36,-48,0};

const double bwValues[21]=  { 2.6, 3.1, 3.9, 5.2, 6.3, 7.8, 10.4, 12.5, 15.6, 20.8, 25, 31.3, 41.7, 50, 62.5, 83.3, 100, 125, 166.7, 200, 250 };
const uint8_t bwIntegers[21]={ 23,  15,   7,  22,  14,   6,   21,   13,    5,   20, 12,    4,    19,11,    3,   18,  10,   2,    17,   9,   1 };

const uint8_t syncWord[4]={0x7c,0xd2,0x15,0xd8};

const char bcdCodes[16]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x2a,0x55,0x20,0x2d,0x29,0x28};

class SX1278FSK {
  public:
    bool monitorRx;
    bool isBOS;
    String daufilter="";
    double centerFreq;
    double rxOffset;
    double bitrate;
    double shift;
    double rxBandwidth;
    double afcBandwidth;
    globalErrors errorCount;
    uint32_t messageCount;
    uint32_t upTime;
    String esp32ID;

    SX1278FSK(bool _monitorRx=false, uint8_t _debug=0) {
      monitorRx=_monitorRx; Log.debug=_debug;
      SPI.begin(SCK, MISO, MOSI, CS); }

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
      delay(25); initSPI(); resetChip(); printChip();
      esp32ID=String(ESP.getEfuseMac(),HEX); }

    void regDump() {
      for (uint8_t reg=0x00;reg<=0x42;reg++) { uint8_t value=readSPI(reg);
        Log.print(0,"0x%02x:     0x%02x     %3u     0b%s\r\n",reg,value,value,String(value,BIN).c_str()); } }

    void setFrequency(double _centerFreq, double _rxOffset=0) {
      centerFreq=_centerFreq; rxOffset=_rxOffset;
      Log.print(0,"Center Frequency: %s MHz\r\n",String(centerFreq+(rxOffset/1000),5).c_str());
      uint32_t value=(uint32_t)round((centerFreq+(rxOffset/1000))*(1<<14));
      writeSPI(regFreqMSB,(value & 0xFF0000) >> 16);
      writeSPI(regFreqMID,(value & 0x00FF00) >> 8);
      writeSPI(regFreqLSB,value & 0x0000FF); }

    void setBitrate(double _bitrate) {
      bitrate=_bitrate;
      Log.print(0,"Bitrate: %s bps\r\n",String(bitrate*1000,0).c_str());
      uint16_t value=(uint16_t)round(32000.0/bitrate);
      writeSPI(regBrMSB,(value & 0xFF00) >> 8);
      writeSPI(regBrLSB,value & 0x00FF); }

    void setShift(double _shift) {
      shift=_shift;
      Log.print(0,"Shift: +/- %s Hz\r\n",String(shift*1000,0).c_str());
      uint16_t value=(uint16_t)round(shift*(1<<11)/125.0);
      writeSPI(regShiftMSB,(value & 0xFF00) >> 8);
      writeSPI(regShiftLSB,value & 0x00FF); }

    void setRxBandwidth(double _rxBandwidth) {
      uint8_t selected=20; for (uint8_t idx=0;idx<=20;idx++) {
        if (bwValues[idx]>=_rxBandwidth) { selected=idx; break; } }
      Log.print(0,"Rx Bandwidth: %s kHz\r\n",String(bwValues[selected],1).c_str());
      setReg(regRxBw,4,0,bwIntegers[selected]); rxBandwidth=bwValues[selected]; }

    void setRxBwAuto() { setRxBandwidth(shift+(bitrate/2)); }

    void setAfcBandwidth(double _afcBandwidth) {
      uint8_t selected=20; for (uint8_t idx=0;idx<=20;idx++) {
        if (bwValues[idx]>=_afcBandwidth) { selected=idx; break; } }
      Log.print(0,"AFC Bandwidth: %s kHz\r\n",String(bwValues[selected],1).c_str());
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
      timerRx=millis()+2000;
      setReg(regSeqConfig1,6,6,1); // Sequencer Stop
      setReg(regRxTimeout2,7,0,0); // Preamble Timeout 0:Off x:x*16*Tbit
      setReg(regRxTimeout3,7,0,0); // Frame Sync Timeout 0:Off x:x*16*Tbit
      delay(100);
      setReg(regOpMode,2,0,4); } // Transceiver Mode 0:Sleep 1:Standby 2:FSTx 3:Tx 4:FSRx 5:Rx

    void initDioIf() {
      setReg(regDioMap1,7,6,1); // DIO0 Mapping 0:Sync Word 1:RSSI/Preamble Detect
      setReg(regDioMap1,5,4,0); // DIO1 Mapping 0:Clock
      setReg(regDioMap1,3,2,0); // DIO2 Mapping 0:Data
      setReg(regDioMap1,1,0,0); // DIO3 Mapping 0:Timeout 1:RSSI/Preamble Detect
      setReg(regDioMap2,0,0,1); // Map Detect Interrupt 0:RSSI 1:Preamble
      queueDIO1=xQueueCreate(queueSizeDIO1,sizeof(uint8_t));
      pinMode(DIO0, INPUT); pinMode(DIO1, INPUT); pinMode(DIO2, INPUT); pinMode(DIO3, INPUT);
      attachInterrupt(DIO0,dio0ISR,RISING);
      attachInterrupt(DIO1,dio1ISR,RISING);
      attachInterrupt(DIO3,dio3ISR,RISING); }

    bool available() {
      uint8_t lastByte;
      return xQueuePeekFromISR(queueDIO1,&lastByte); }

    uint8_t read() {
      uint8_t lastByte;
      xQueueReceiveFromISR(queueDIO1,&lastByte,NULL);
      return lastByte; }

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

    double getGain() { return gainValues[getReg(regRxLna,7,5)]; }

    void setRssiTresh(int tresh) {
      writeSPI(regRssiTresh,(uint8_t)(tresh*-2)); }

    double getRSSI() { return readSPI(regRssi)/-2.0; }

    uint8_t searchSync(uint8_t rxByte) {
      static uint32_t syncBuffer;
      for (uint8_t bitPos=0;bitPos<=7;bitPos++) {
        syncBuffer<<=1; syncBuffer|=(rxByte&128)>>7; rxByte<<=1;
        if (syncBuffer==0x7cd215d8) { return 7-bitPos; } }
      return 255; }

    void printChip() {
      Log.print(0,"SX1278 Chip Version: %i Hardware Revision: %i\r\n",getReg(regChipVersion,7,4),getReg(regChipVersion,3,0)); }

    void printRx() {
      Log.print(0,"RSSI: %s dBm   Gain: %s dBm   AFC: %s kHz   FEI: %s kHz\r\n",String(getRSSI(),1).c_str(),String(getGain(),1).c_str(),String(getAFC(),3).c_str(),String(getFEI(),3).c_str()); }

    void beginPOCSAG() {
      setModeFskRxCont();
      initDioIf();
      restartRx(true);
      startSequencer();
      delay(500);
      timerRx=millis()+1000;
      Log.print(0,"POCSAG Rx started\r\n"); }

    String consoleDE(uint8_t code) {
      if (isROT1) { code=(code==0)?127:code-1; }
      if ((code>0x0 && code<0x20) || code>0x7e) { return "[" + String(code,DEC) + "]"; }
      switch(code) {
        case 0x0: return ""; break;
        case 0x5b: return "\u00c4"; break;
        case 0x5c: return "\u00d6"; break;
        case 0x5d: return "\u00dc"; break;
        case 0x7b: return "\u00e4"; break;
        case 0x7c: return "\u00f6"; break;
        case 0x7d: return "\u00fc"; break;
        case 0x7e: return "\u00df"; break;
        default: return String((char)code); } }

    void messageReceived() {
      Log.print(1,"    BCH Errors: %i/%i\r\n",error.corrected,error.uncorrected);
      if (gwURL!="") {
        if (message=="") { message="no message"; }
        String postValue="dme=" + esp32ID;
        postValue+="&rssi=" + urlencode(String(rssi,1));
        postValue+="&error=" + String(error.uncorrected);
        postValue+="&ric=" + String(ric);
        postValue+="&function=" + String(function);
        postValue+="&dau=" + urlencode(dau);
        postValue+="&message=" + urlencode(message);
        postHTTPS(gwURL,postValue); }
      error.corrected=0; error.uncorrected=0; message=""; messageCount++; }

    void messageFiltered() {
      Log.needCR=false; Log.print(1," filtered out\r\n");
      error.corrected=0; error.uncorrected=0; message=""; }

    void pocsagWorker() {
      if (millis()>=timerRx) { timerRx=millis()+1000;
        if (isMessageRun) { isMessageRun=false; messageReceived();
          if (isBOS) { isDAU=true; } else { isDAU=false; }
          ric=0; function=0x58; dau=""; }
        restartRx(false); upTime++;
        if (monitorRx) { printRx(); } }

      portENTER_CRITICAL(&mutexDIO0);
      if (detectDIO0Flag) { detectDIO0Flag=false; portEXIT_CRITICAL(&mutexDIO0);
        Log.print(2,"Preamble Detected!\r\n");
        if (rxOffset==0) { rxOffset=getAFC(); Log.print(0,"Auto Rx Offset: %s kHz detected\r\n",String(rxOffset,3).c_str()); }
        if (Log.debug) { printRx(); }
        Log.print(2,"Bytes queued: %i/%i\r\n",uxQueueMessagesWaitingFromISR(queueDIO1),queueSizeDIO1);
        if (isBOS) { isDAU=true; } else { isDAU=false; }
        rssi=getRSSI()-getGain();
        error.corrected=0; error.uncorrected=0; ric=0; function=0x58; dau=""; message="";
        timerRx=millis()+1000; }
      else { portEXIT_CRITICAL(&mutexDIO0); }

      if (available()) {
        uint8_t rxByte=read();
        uint8_t bitShift=searchSync(rxByte);

        if (bitShift!=255) {
          timerRx=millis()+1000;
          Log.print(2,"Frame Sync Detected! Bit Shift: %i\r\n",bitShift);
          uint32_t batch[16]={0};

          for (uint8_t idx=0;idx<=63;idx++) {
            uint8_t codeWord=idx>>2;
            batch[codeWord]<<=bitShift; batch[codeWord]|=rxByte&((1<<bitShift)-1);
            while (!available()) {} rxByte=read();
            batch[codeWord]<<=8-bitShift; batch[codeWord]|=rxByte>>bitShift;
            if (idx==63 && bitShift!=0) { searchSync(rxByte); } }

          if (rawURL!="") {
            String postValue="dme=" + esp32ID;
            String rawBatch;
            for (uint8_t idx=0;idx<=15;idx++) { rawBatch+=(char)(batch[idx]>>0); rawBatch+=(char)(batch[idx]>>8); rawBatch+=(char)(batch[idx]>>16); rawBatch+=(char)(batch[idx]>>24); }
            postValue+="&raw=" + urlencode(rawBatch);
            postHTTPS(rawURL,postValue); }

          for (uint8_t idx=0;idx<=15;idx++) {
            errors currentError=bch.decode(batch[idx]);

            if (batch[idx]==0x7a89c197) { isIdle=true; } else { isIdle=false; }

            if (!(batch[idx]&(1<<31))) { isAddress=true; } else { isAddress=false; }

            if (isBOS && dau!="") { if (daufilter!="" && (!dau.startsWith(daufilter))) { isMessageRun=false; messageFiltered(); break; } }

            if (isAddress && isMessageRun && (!isIdle)) { isMessageRun=false; messageReceived(); timerRx=millis()+1000; }

            error.corrected+=currentError.corrected; error.uncorrected+=currentError.uncorrected;
            errorCount.corrected+=currentError.corrected; errorCount.uncorrected+=currentError.uncorrected;

            Log.print(3,"%02u: ",idx); for (int8_t bit=31;bit>=0;bit--) {
              if (bit==30 || (isAddress && bit==12) || bit==10 || bit==0) { Log.write(3,0x20); }
              if (batch[idx]&(1<<bit)) { Log.write(3,0x31); } else { Log.write(3,0x30); } }
            if (isIdle) { Log.print(3," Idle"); }
            if (isAddress) { Log.print(3," Address"); } else { Log.print(3," Message"); }
            Log.print(3," BCH Error %i/%i\r\n",currentError.corrected,currentError.uncorrected);

            if (isAddress && (!isIdle)) {
              isDAU=false;
              ric=((batch[idx]&0x7fffe000)>>10)|(idx>>1);
              if (ric==4512 || ric==4520) { isROT1=true; } else { isROT1=false; }
              Log.print(1,"  RIC: %i\r\n",ric);
              if (isROT1) { Log.print(1,"    Encoded: ROT1\r\n"); }
              function=(batch[idx]&0x1800)>>11;
              if (isBOS) { function+=0x41; } else { function+=0x30; }
              switch(function) {
                case '0': Log.print(1,"    Message Type: Numeric\r\n"); isText=false; break;
                case '1': Log.print(1,"    Message Type: 1\r\n"); isText=false; break;
                case '2': Log.print(1,"    Message Type: 2\r\n"); isText=false; break;
                case '3': Log.print(1,"    Message Type: Text\r\n"); isText=true; break;
                case 'A': Log.print(1,"    Sub RIC: A\r\n"); isText=true; break;
                case 'B': Log.print(1,"    Sub RIC: B\r\n"); isText=true; break;
                case 'C': Log.print(1,"    Sub RIC: C\r\n"); isText=true; break;
                case 'D': Log.print(1,"    Sub RIC: D\r\n"); isText=true; } }

            if (isAddress) { text=0; textPos=0; number=0; numberPos=0; }

            if (!isIdle) { isMessageRun=true; }

            if ((!isAddress) && isDAU) { isText=false; if (idx==0) { Log.print(1,"    DAU Address: "); Log.needCR=true; } }

            if ((!isAddress) && isText) {
              if (!Log.needCR) { Log.print(1,"    "); Log.needCR=true; }
              for (uint8_t bitPos=30;bitPos>=11;bitPos--) {
                text>>=1; text|=(batch[idx]&(1<<bitPos))>>(bitPos-7);
                textPos++; if (textPos>=7) { text>>=1;
                  message+=String(consoleDE(text));
                  Log.needCR=false; Log.print(1,"%s",consoleDE(text).c_str()); Log.needCR=true;
                  text=0; textPos=0; } } }

            if ((!isAddress) && (!isText)) {
              if (!Log.needCR) { Log.print(1,"    "); Log.needCR=true; }
              for (uint8_t bitPos=30;bitPos>=11;bitPos--) {
                number>>=1; number|=(batch[idx]&(1<<bitPos))>>(bitPos-7);
                numberPos++; if (numberPos>=4) { number>>=4;
                  if (isDAU) { dau+=String(bcdCodes[number]); }
                  if (!isDAU) { message+=String(bcdCodes[number]); }
                  Log.write(1,bcdCodes[number]);
                  number=0; numberPos=0; } } } } } } }

  private:
    uint64_t timerRx;
    bool isText;
    bool isROT1;
    bool isIdle;
    bool isAddress;
    bool isDAU;
    errors error;
    uint32_t ric;
    char function;
    bool isMessageRun;
    String dau="";
    String message="";
    bool parity;
    double rssi;
    uint8_t text=0;
    uint8_t textPos=0;
    uint8_t number=0;
    uint8_t numberPos=0; };

#endif
