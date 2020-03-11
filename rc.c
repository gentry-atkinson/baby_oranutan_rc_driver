/************************************************
*Author: Gentry Atkinson
*Organization: Bobcat Robotics
*Date: 26 Feb 2020
*Description: this file is being written as a driver for the fairyweight robot
*    Uncle Traveling Matt but it should serve as a general purpose program for
*    tank-style drive with a 3-channel mixed controller.
************************************************/

#include <pololu/orangutan.h>

//Connected pins on Baby Orangutan. Change to fit your setup
#define headlight_pin IO_D7
#define throttle_pin IO_B1
#define steering_pin IO_B0
#define ch3_pin IO_B2

void read_pins(int*, int*, int*);
void set_neutral_pulse(int*, int*);

int main() {
  set_motors(0, 0);
  red_led(1);

  //Start Up section
  //Give the receiver time to start
  delay_ms(250);
  pulse_in_start((unsigned char[]){throttle_pin, steering_pin, ch3_pin}, 3);
  delay_ms(250);
  set_digital_output(headlight_pin, LOW);
  set_digital_input(throttle_pin, PULL_UP_ENABLED);
  set_digital_input(steering_pin, PULL_UP_ENABLED);
  set_digital_input(ch3_pin, PULL_UP_ENABLED);
  red_led(0);

  int throttle=0, steering=0, ch3=0;
  int motor1Out, motor2Out;
  int neutralThrottlePulse=0, neutralSteeringPulse=0;

  set_neutral_pulse(&neutralThrottlePulse, &neutralSteeringPulse);

  //The Forever loop
  while(1)
  {
    read_pins(&throttle, &steering, &ch3);

    //Headlight control
    if (ch3 > neutralThrottlePulse){
     set_digital_output(headlight_pin, HIGH);
    }
    else{
     set_digital_output(headlight_pin, LOW);
    }

    //Throttle or steering will be 0 if signal has dropped
    if(throttle==0 || steering==0){
      set_motors(0,0);
      motor1Out=0;
      motor2Out=0;
    }
    else{
      //Throttle will be 1000 (full reverse) to 2000 (full forward)
      //Steering will be 1000 (full left) to 2000 (full right)
      //(signal - neutral) will range roughly from -500 to 500
      motor1Out = (throttle-neutralThrottlePulse) - (steering-neutralSteeringPulse);
      motor2Out = -(throttle-neutralThrottlePulse) - (steering-neutralSteeringPulse);
    }

    //motor outputs must be -255 to 255
    if (motor1Out < -255) motor1Out = -255;
    else if (motor1Out > 255) motor2Out = 255;

    if (motor2Out < -255) motor2Out = -255;
    else if (motor2Out > 255) motor2Out = 255;
    set_motors(motor1Out, motor2Out);
  }//end Forever loop

  return 0;
}

void set_neutral_pulse(int* neutralThrottlePulse, int *neutralSteeringPulse){
  static struct PulseInputStruct pulseInfo;
  set_digital_output(headlight_pin, HIGH);
  int tp=0, sp=0;

  //Hold until a throttle signal is read
  do{
    delay_ms(50);
    get_pulse_info(0, &pulseInfo);
    tp = pulse_to_microseconds(pulseInfo.lastHighPulse);
  }while((get_ticks()-pulseInfo.lastPCTime) > 10000 || tp < 1300 || tp > 1700);
  *neutralThrottlePulse= tp;

  //Hold until a steeing signal is read
  do{
    delay_ms(50);
    get_pulse_info(1, &pulseInfo);
    sp = pulse_to_microseconds(pulseInfo.lastHighPulse);
  }while((get_ticks()-pulseInfo.lastPCTime) > 10000 || sp < 1300 || sp > 1700);
  *neutralSteeringPulse= sp;

  set_digital_output(headlight_pin, LOW);
  return;
}

void read_pins(int* throttle, int* steering, int* ch3){

  red_led(1);
  static struct PulseInputStruct pulseInfo;

  get_pulse_info(0, &pulseInfo);
  //freeze if signal drops
  if((get_ticks()-pulseInfo.lastPCTime) > 400000){
    *throttle = 0;
    *steering = 0;
    return;
  }
  *throttle = (pulse_to_microseconds(pulseInfo.lastHighPulse));

  get_pulse_info(1, &pulseInfo);
  *steering = (pulse_to_microseconds(pulseInfo.lastHighPulse));
  //freeze if signal drops
  if((get_ticks()-pulseInfo.lastPCTime) > 400000){
    *throttle = 0;
    *steering = 0;
    return;
  }

  //no need to freeze
  get_pulse_info(2, &pulseInfo);
  *ch3 = (pulse_to_microseconds(pulseInfo.lastHighPulse));

  red_led(0);
  return;
}
