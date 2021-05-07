#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MeSerial.h>
#include <MeAuriga.h>
#include <MeBluetooth.h>
#include <string.h>

/*****************************************************************************************/
//    Types
/*****************************************************************************************/
// Enumeration for mower states
typedef enum {
  MOWER_IDLE = 0,
  MOWER_AUTO_RUN,
  MOWER_MAN_FORWARD,
  MOWER_MAN_BACKWARDS,
  MOWER_MAN_LEFT,
  MOWER_MAN_RIGHT,
  MOWER_FAULT = 99
} mowerState_t;

//enumeration for linesensor return states
typedef enum {
  LINESENSOR_NONE,
  LINESENSOR_LEFT,
  LINESENSOR_RIGHT,
  LINESENSOR_BOTH
} linesensorState_t;

//globals vars
mowerState_t mowerStateGlobal = MOWER_IDLE;
linesensorState_t linesensorStateGlobal = LINESENSOR_NONE;
MeSerial meSerial(PORT5);
MeEncoderOnBoard leftMotor(SLOT1);
MeEncoderOnBoard rightMotor(SLOT2);
MeLightSensor lightsensor_12(12);
MeUltrasonicSensor ultrasonic_6(6); //Ultrasonic sensor on port 6
MeLineFollower linefollower_7(7);   //Line follow sensor on port 7
MeGyro gyro_0(0, 0x69);

// Gyro variables
float posZ = 0;

//MeAuriga functions
void isr_process_leftMotor(void);
void isr_process_rightMotor(void);
void move(int direction, int speed);
void _loop();
void _delay(float seconds);

/*****************************************************************************************/
//    Private functions declarations
/*****************************************************************************************/
//Functions to move the robot.
void moveForward();
void moveBackward();
void moveLeft();
void moveRight();
void moveStop();

//Sesnsor functions
void updateLinesensorState();
void collision();
int ultraSonicDistance(); // reads the distance
void autoRun(void);
void updateLinesensorState(void);
void sendPosVectorToPi();

//Bluetooth functions
String Read();
void Write(char ch);
void Write(String string);
void updateState(String data);

void mowerDriveState(void);

/*****************************************************************************************/
//   Main loop and setup functions
/*****************************************************************************************/

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
  gyro_0.begin();
  randomSeed(analogRead(0));

  posZ = gyro_0.getAngle(3);
}

void loop() {
 
  String btdata = Read();
  String data = btdata.substring(0,2);

  
  if (data != "" && data != nullptr){
    //test = data;
    updateState(data);
  }

  
  mowerDriveState();
  _loop();
}


/*****************************************************************************************/
//   Functions
/*****************************************************************************************/


//position functions
void sendPosVectorToPi(){
  long distance = ((rightMotor.getCurPos()*124.4)/360);
  meSerial.println(String(posZ) + " " + String(distance));
}

// long getPos(){
//   return ((rightMotor.getCurPos()*124.4)/360);
// }

//Auriga functions
void _loop() {
  leftMotor.loop();
  rightMotor.loop();

  posZ = gyro_0.getAngle(3);
  gyro_0.update();
}

void _delay(float seconds) {
  if(seconds < 0.0){
    seconds = 0.0;
  }
  long endTime = millis() + seconds * 1000;
  while(millis() < endTime) _loop();
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

// Ultrasonic sesnor function, return the distance to object in cm.
int ultraSonicDistance(){

  if(ultrasonic_6.distanceCm() < 5){
    Write("5 or less cm");
    collision();
  }
  else if(ultrasonic_6.distanceCm() < 10){
    Write("10 or less cm");
  }
  else if(ultrasonic_6.distanceCm() < 15){
    Write("15 or less cm");
  }
  else if(ultrasonic_6.distanceCm() < 20){
    Write("20 or less cm");
  }

}
// function to update the global state of the lines sensor.
void updateLinesensorState(){
  //if right is black
  if((0?(1==0?linefollower_7.readSensors()==0:(linefollower_7.readSensors() & 1)==1):(1==0?linefollower_7.readSensors()==3:(linefollower_7.readSensors() & 1)==0))){
    linesensorStateGlobal = LINESENSOR_RIGHT;
  }
  //if left is black  
  else if((0?(2==0?linefollower_7.readSensors()==0:(linefollower_7.readSensors() & 2)==2):(2==0?linefollower_7.readSensors()==3:(linefollower_7.readSensors() & 2)==0))){
   linesensorStateGlobal = LINESENSOR_LEFT;
  }
  //if both are black
  else if((0?(3==0?linefollower_7.readSensors()==0:(linefollower_7.readSensors() & 3)==3):(3==0?linefollower_7.readSensors()==3:(linefollower_7.readSensors() & 3)==0))){
    linesensorStateGlobal = LINESENSOR_BOTH;
  }
  else{
    linesensorStateGlobal = LINESENSOR_NONE;
  }
}

void collision(){
  moveBackward();
  _delay(0.5);
  moveRight();
  _delay(0.5);
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

//Functions for moving the robot
void moveForward(){
  move(1, 40 / 100.0 * 255);
}

void moveBackward(){
  move(2, 50 / 100.0 * 255);
}

void moveLeft(){
  move(3, 40 / 100.0 * 255);
}

void moveRight(){
  move(4, 40 / 100.0 * 255);
}

void moveStop(){
  leftMotor.setTarPWM(0);
  rightMotor.setTarPWM(0);
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


// Function to read the BT commands and update the state machince.
void updateState(String data){
  
  if(data == "AR"){
    mowerStateGlobal = MOWER_AUTO_RUN;
    Write("Inne i AR");
  }
  else if(data=="AS"){
    mowerStateGlobal =  MOWER_IDLE;
    Write("Inne i AS");
  }
  else if(data=="MF"){
    mowerStateGlobal = MOWER_MAN_FORWARD;
    Write("Inne i MF");
  }
  else if(data=="MB"){
    mowerStateGlobal = MOWER_MAN_BACKWARDS;
    Write("Inne i MB");
  }
  else if(data=="ML"){
    mowerStateGlobal = MOWER_MAN_LEFT;
    Write("Inne i ML");

  }
  else if(data=="MR"){
    mowerStateGlobal = MOWER_MAN_RIGHT;
    Write("Inne i MR");
  }
  /* else{
    mower_state_global = MOWER_FAULT;
  } */

}
//main state machince for the mower
void mowerDriveState(){
  switch(mowerStateGlobal){
    case MOWER_IDLE:
      moveStop();
      break;

    case MOWER_AUTO_RUN:
      autoRun();
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


//logic for autonmous operations
void autoRun(void){
  updateLinesensorState();
  // if(ultrasonic_6.distanceCm() <= 5 || linesensorStateGlobal!= LINESENSOR_NONE){   //For "Old version" right below
  if(ultrasonic_6.distanceCm() <= 5 || linesensorStateGlobal == LINESENSOR_RIGHT){
    
 
    // Print values for collsion and reset
    moveStop();
    _delay(0.01);
    sendPosVectorToPi();
    rightMotor.setPulsePos(0);

   if(ultrasonic_6.distanceCm() <= 5){
      meSerial.println("COLLISION");
    }
    // Go back and update values
    moveBackward();
    _delay(0.6);
    rightMotor.loop();
    rightMotor.updateCurPos();

    // Print values after backing
    moveStop();
    _delay(0.01);
    sendPosVectorToPi();
    rightMotor.setPulsePos(0);

    float randTime = (random( 4096 ) % 150)  + 15;


    // ---- Old version, take back if we get new line-sensor ----- //
    // if(linesensorStateGlobal == LINESENSOR_LEFT){
      //   moveRight();
      // }
      // else if(linesensorStateGlobal == LINESENSOR_RIGHT){
      //   moveLeft();
      // }
      // else{
      //   float randDir = (random( 4096 ) % 2);
      //   if(randDir){
      //     moveLeft();
      //   }else{
      //     moveRight();
      //   }
    // }

      float randDir = (random( 4096 ) % 2);
      if(randDir){
        moveLeft();
      }else{
        moveRight();
      }

    int i = 0;
    while(i++ < randTime){
      _delay(0.01);
      updateLinesensorState();
      // if(linesensorStateGlobal != LINESENSOR_NONE){    //For "Old version" right above
      if(linesensorStateGlobal == LINESENSOR_RIGHT){
        moveStop();
        break;
      }
    }

  }
  else{
    moveForward();
  }

}