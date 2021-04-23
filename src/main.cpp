#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MeSerial.h>
#include <MeAuriga.h>
#include <MeBluetooth.h>
#include <string.h>



MeSerial meSerial(PORT5);

MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);
//SoftwareSerial BtSerial(PE0, PE1);

char test;


// WHat is dis?
MeLightSensor lightsensor_12(12);


void isr_process_encoder1(void)
{
  if(digitalRead(Encoder_1.getPortB()) == 0){
    Encoder_1.pulsePosMinus();
  }else{
    Encoder_1.pulsePosPlus();
  }
}
void isr_process_encoder2(void)
{
  if(digitalRead(Encoder_2.getPortB()) == 0){
    Encoder_2.pulsePosMinus();
  }else{
    Encoder_2.pulsePosPlus();
  }
}


void move(int direction, int speed)
{
  int leftSpeed = 0;
  int rightSpeed = 0;

  if(direction == 1){
    leftSpeed = -speed;
    rightSpeed = speed;
  }else if(direction == 2){
    leftSpeed = speed;
    rightSpeed = -speed;
  }else if(direction == 3){
    leftSpeed = -speed;
    rightSpeed = -speed;
  }else if(direction == 4){
    leftSpeed = speed;
    rightSpeed = speed;
  }

  Encoder_1.setTarPWM(leftSpeed);
  Encoder_2.setTarPWM(rightSpeed);
}

//Bluetooth functions
String Read();
void Write(char ch);
void Write(String string);


void setup() {
 // BtSerial.begin(115200);
 
  // put your setup code here, to run once:
  Serial.begin(115200);
  meSerial.begin(9600);
  //mySerial.begin(9600);

  //Serial.print("TESTING SETUP");

  meSerial.sendString("Hello World");


  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  randomSeed((unsigned long)(lightsensor_12.read() * 123456));
  delay(1000);

  // MOVE FUNCTION ????
  move(1, 50 / 100.0 * 255);


  // This makes the mBot STOP moving
  Encoder_1.setTarPWM(0);
  Encoder_2.setTarPWM(0);
  delay(500);

  meSerial.sendString("SETUP DONE");
}




void loop() {
  // put your main code here, to run repeatedly:

  //String test = "";
  char test = ' ';
  //test = meSerial.read();
  if (Serial.available()){
    test = Serial.read();
  }
  //test = Read();
  if (meSerial.available()){
    test = meSerial.read();
  }
  
  if (test != ' '){
    meSerial.print(test);
  }
  //test = meSerial.readString();
  //test = Read(); 
  
  //Serial.print(test);
  //Write(test);

}
//Bluetooth functions
String Read(){
  String input = "";
    if (Serial.available()){
      input = Serial.readString();
    }
  return input;
}
void Write(char ch){
  if (Serial.availableForWrite()){
    if (ch != NULL){
      Serial.print(ch);
    }
  }
}
void Write(String string){
  if (Serial.availableForWrite()){
    if ((string != "") && (string != NULL)){
      Serial.println(string);
    }
  }
}