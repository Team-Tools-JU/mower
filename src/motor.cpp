#include "motor.h"


// MeEncoderOnBoard leftMotor(SLOT1);
// MeEncoderOnBoard rightMotor(SLOT2);


//Velocity
// int speed = 100;

//Speed for each wheel
int leftSpeed = 0;
int rightSpeed = 0;



// void isr_process_leftMotor(void){

//     if (digitalRead(leftMotor.getPortB()) == 0){
//         leftMotor.pulsePosMinus();
//     }
//     else{
//         leftMotor.pulsePosPlus();
//     }
// }
// void isr_process_rightMotor(void){

//     if (digitalRead(rightMotor.getPortB()) == 0){
//         rightMotor.pulsePosMinus();
//     }
//     else{
//         rightMotor.pulsePosPlus();
//     }
// }




// void motorTextTest(){

//     Serial.print("Writing from motors");

//     // attachInterrupt(leftMotor.getIntNum(), isr_process_leftMotor, RISING);
//     // attachInterrupt(rightMotor.getIntNum(), isr_process_rightMotor, RISING);
// }


// void setSpeed(int leftSpeed, int rightSpeed){

//     leftMotor.setTarPWM(leftSpeed);
//     rightMotor.setTarPWM(rightSpeed);
// }


// void move(int direction, int speed)
// {
//   int leftSpeed = 0;
//   int rightSpeed = 0;

//   if(direction == 1){
//     leftSpeed = -speed;
//     rightSpeed = speed;
//   }else if(direction == 2){
//     leftSpeed = speed;
//     rightSpeed = -speed;
//   }else if(direction == 3){
//     leftSpeed = -speed;
//     rightSpeed = -speed;
//   }else if(direction == 4){
//     leftSpeed = speed;
//     rightSpeed = speed;
//   }

//   leftMotor.setTarPWM(leftSpeed);
//   rightMotor.setTarPWM(rightSpeed);
// }



// void moveLeft(){
//     leftSpeed = -speed;
//     rightSpeed = speed;

//     setSpeed(leftSpeed, rightSpeed);
// }

// void moveRight(){
//     leftSpeed = speed;
//     rightSpeed = -speed;

//     setSpeed(leftSpeed, rightSpeed);
// }

// void moveForward(){
//     leftSpeed = speed;
//     rightSpeed = speed;

//     setSpeed(leftSpeed, rightSpeed);
// }

// void moveBackward(){    
//     leftSpeed = -speed;
//     rightSpeed = -speed;

//     setSpeed(leftSpeed, rightSpeed);
// }


// What is this??

//   attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
//   attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);




// void stopMotor(){
//     leftMotor.setTarPWM(0);
//     rightMotor.setTarPWM(0);
// }
