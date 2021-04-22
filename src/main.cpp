#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MeSerial.h>
#include <MeAuriga.h>



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

MeEncoderOnBoard Encoder_1(SLOT1);
MeEncoderOnBoard Encoder_2(SLOT2);


// WHat is dis?
MeLightSensor lightsensor_12(12);


//state machine for mower
void mower_drive_state(mower_state_t state);




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




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  meSerial.begin(9600);
  //mySerial.begin(9600);

  Serial.print("TESTING SETUP");

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
  

  //delay(1000);

  Encoder_1.loop();
  Encoder_2.loop();



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