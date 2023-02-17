#ifndef SX1278_LOG_H
#define SX1278_LOG_H

class Logging {
  public:
    uint8_t debug=0;
    bool needCR=false;
    void (*writeTelnet)(const char)=nullptr;
    void (*printTelnet)(const char*)=nullptr;

    Logging() {
      Serial.begin(115200); }

    void write(uint8_t level,const char value) {
      if (level>debug) { return; }
      Serial.write(value);
      if (writeTelnet!=nullptr) { writeTelnet(value); } }

    void print(uint8_t level,const char* format,...) {
      if (level>debug) { return; }
      char buffer[64];
      char* tbuffer=buffer;
      va_list args;
      va_list copy;
      va_start(args,format);
      va_copy(copy,args);
      int len=vsnprintf(tbuffer,sizeof(buffer),format,copy);
      va_end(copy);
      if (len<0) { va_end(args); return; }
      if (len>=(int)sizeof(buffer)) {
        tbuffer=(char*)malloc(len+1);
        if (tbuffer==NULL) { va_end(args); return; }
        len=vsnprintf(tbuffer,len+1,format,args); }
      va_end(args);

      if (needCR) { needCR=false; Serial.println();
        if (printTelnet!=nullptr) { printTelnet("\r\n"); } }
      //Serial.print(tbuffer);
      Serial.write((uint8_t*)tbuffer,len);
      if (printTelnet!=nullptr) { printTelnet(tbuffer); }

      if (tbuffer!=buffer) { free(tbuffer); } } };

#endif
