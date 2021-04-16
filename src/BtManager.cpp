#include <BtManager.h>
    BtManager::BtManager(){
        Serial.begin(115200);
    }
    String Read(){
        String input = "";
        if (Serial.available()){
            input = Serial.readString();
        }
        return input;
    }
    void Write(char ch){
        if (Serial.availableForWrite()){
            Serial.print(ch);
        }
    }
    void Write(String string){
        if (Serial.availableForWrite()){
            Serial.println(string);
        }
    }