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


MeSerial meSerial(PORT5);
MeEncoderOnBoard leftMotor(SLOT1);
MeEncoderOnBoard rightMotor(SLOT2);
MeLightSensor lightsensor_12(12);
MeLineFollower linefollower_6(6); //Line follow sensor on port 6

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

String Read();
void Write(char ch);
void Write(String string);

void mower_drive_state(mower_state_t state);



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
  


  moveStop();
  leftMotor.setTarPWM(0);
  rightMotor.setTarPWM(0);
  _loop();
}




/*********************  Below is all functions  *********************************/

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


void mower_drive_state(mower_state_t state){
  switch(state){
    case MOWER_IDLE:
      //motor speed 0
      break;
    case MOWER_AUTO_RUN:
      //If obsticle ->reverse and turn 
      //else forward
      break;
    case MOWER_MAN_FORWARD:
      //Move forward
      break;
    case MOWER_MAN_BACKWARDS:
      //Move backwards
      break;
    case MOWER_MAN_LEFT:
      //Move left
      break;
    case MOWER_MAN_RIGHT:
      //Move right
      break;

    case MOWER_FAULT:
      //error handling
      break;

    default:
      break;
  }

}


