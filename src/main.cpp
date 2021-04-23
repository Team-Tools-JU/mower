#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MeSerial.h>
#include <MeAuriga.h>
#include <MeBluetooth.h>
#include <string.h>






// Enumeration for mower states
typedef enum {
  MOWER_IDLE = 0,
  MOWER_AUTO_RUN,
  MOWER_MAN_FORWARD,
  MOWER_MAN_BACKWARDS,
  MOWER_MAN_LEFT,
  MOWER_MAN_RIGHT,
  MOWER_FAULT = 99
} mower_state_t;


//globals

mower_state_t mower_state_global = MOWER_IDLE;

MeSerial meSerial(PORT5);
MeEncoderOnBoard leftMotor(SLOT1);
MeEncoderOnBoard rightMotor(SLOT2);
MeLightSensor lightsensor_12(12);

MeLineFollower linefollower_7(7); //Line follow sensor on port 6

//MeAuriga functions, Don't use!
void isr_process_leftMotor(void);
void isr_process_rightMotor(void);
void move(int direction, int speed);
void _loop();
void _delay(float seconds);

//Private functions.
void moveForward();
void moveBackward();
void moveLeft();
void moveRight();
void moveStop();
boolean isBlackLine();
void collision();

String Read();
void Write(char ch);
void Write(String string);

void updateState(String data);



void mower_drive_state(void);



void setup() {
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  attachInterrupt(leftMotor.getIntNum(), isr_process_leftMotor, RISING);
  attachInterrupt(rightMotor.getIntNum(), isr_process_rightMotor, RISING);
  randomSeed((unsigned long)(lightsensor_12.read() * 123456));
  Serial.begin(115200);
  meSerial.begin(9600);



}





void loop() {
  isBlackLine();


  String btdata = Read();


  String data = btdata.substring(0,2);
  
  
  if (data != "" && data != nullptr){
    //test = data;
    updateState(data);
  }
  //Serial.print()
  //Write(test);
  
  mower_drive_state();
  _loop();
}




/*********************  Below is all functions  *********************************/

boolean isBlackLine(){

  //Right black
  if((0?(1==0?linefollower_7.readSensors()==0:(linefollower_7.readSensors() & 1)==1):(1==0?linefollower_7.readSensors()==3:(linefollower_7.readSensors() & 1)==0))){
    return true;
  }

  //Left black  
  else if((0?(2==0?linefollower_7.readSensors()==0:(linefollower_7.readSensors() & 2)==2):(2==0?linefollower_7.readSensors()==3:(linefollower_7.readSensors() & 2)==0))){
    return true;
  }

  //All black
  else if((0?(3==0?linefollower_7.readSensors()==0:(linefollower_7.readSensors() & 3)==3):(3==0?linefollower_7.readSensors()==3:(linefollower_7.readSensors() & 3)==0))){
    return true;
  }
  
  else{
    return false;
  }

}



void collision(){
  moveBackward();
  _delay(0.5);
  moveRight();
  _delay(0.5);
}



void isr_process_leftMotor(void)
{
  if(digitalRead(leftMotor.getPortB()) == 0){
    leftMotor.pulsePosMinus();
  }else{
    leftMotor.pulsePosPlus();
  }
}

void isr_process_rightMotor(void)
{
  if(digitalRead(rightMotor.getPortB()) == 0){
    rightMotor.pulsePosMinus();
  }else{
    rightMotor.pulsePosPlus();
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
  leftMotor.setTarPWM(leftSpeed);
  rightMotor.setTarPWM(rightSpeed);
}


void moveForward(){
  move(1, 50 / 100.0 * 255);
}

void moveBackward(){
  move(2, 50 / 100.0 * 255);
}

void moveLeft(){
  move(3, 50 / 100.0 * 255);
}


void moveRight(){
  move(4, 50 / 100.0 * 255);
}



void moveStop(){
  leftMotor.setTarPWM(0);
  rightMotor.setTarPWM(0);
}





void _loop() {
  leftMotor.loop();
  rightMotor.loop();
}

void _delay(float seconds) {
  if(seconds < 0.0){
    seconds = 0.0;
  }
  long endTime = millis() + seconds * 1000;
  while(millis() < endTime) _loop();
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


void updateState(String data){
  
  //Write("Inne i updateState");
  //Write(data);
  //Serial.print(data);
  if(data == "AR"){
    mower_state_global = MOWER_AUTO_RUN;
    Write("Inne i AR");
  }
  else if(data=="AS"){
    mower_state_global =  MOWER_IDLE;
    Write("Inne i AS");
  }
  else if(data=="MF"){
    mower_state_global = MOWER_MAN_FORWARD;
    Write("Inne i MF");
  }
  else if(data=="MB"){
    mower_state_global = MOWER_MAN_BACKWARDS;
    Write("Inne i MB");
  }
  else if(data=="ML"){
    mower_state_global = MOWER_MAN_LEFT;
    Write("Inne i ML");

  }
  else if(data=="MR"){
    mower_state_global = MOWER_MAN_RIGHT;
    Write("Inne i MR");
  }
  /* else{
    mower_state_global = MOWER_FAULT;
  } */

}


void mower_drive_state(){
  switch(mower_state_global){
    case MOWER_IDLE:
      moveStop();
      break;
    case MOWER_AUTO_RUN:
      if(0){
        //If obsticle ->reverse and turn 
        //TODO
      }else{
        moveForward();
      }
      break;
    case MOWER_MAN_FORWARD:
      moveForward();
      break;
    case MOWER_MAN_BACKWARDS:
      moveBackward();
      break;
    case MOWER_MAN_LEFT:
      moveLeft();
      break;
    case MOWER_MAN_RIGHT:
      moveRight();
      break;

    case MOWER_FAULT:
      //error handling
      
      break;

    default:
      break;
  }

}


