#ifndef BtManager_H
#define BtManager_h
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>

class BtManager {
    public:
        BtManager();
        String Read();
        void Write(char ch);
        void Write(String string);


};



#endif