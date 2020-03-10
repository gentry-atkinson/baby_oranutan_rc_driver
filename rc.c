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

//For pulse_in
#define LOW_PULSE   1
#define HIGH_PULSE  2
#define ANY_PULSE   3

void read_pins(int*, int*, int*);
void set_neutral_pulse(int*, int*);

int main() {
  set_motors(0, 0);
  red_led(1);

  //Give the receiver time to start
  pulse_in_start((unsigned char[]){throttle_pin, steering_pin, ch3_pin}, 3);
  delay_ms(500);
  set_digital_output(headlight_pin, LOW);
  set_digital_input(throttle_pin, PULL_UP_ENABLED);
  set_digital_input(steering_pin, PULL_UP_ENABLED);
  set_digital_input(ch3_pin, PULL_UP_ENABLED);
  red_led(0);

  int throttle=0, steering=0, ch3=0;
  int motor1Out, motor2Out;
  int neutralThrottlePulse=0, neutralSteeringPulse=0;

  set_neutral_pulse(&neutralThrottlePulse, &neutralSteeringPulse);

  while(1)
  {
    read_pins(&throttle, &steering, &ch3);

    if (ch3 > neutralThrottlePulse){
     set_digital_output(headlight_pin, HIGH);
    }
    else{
     set_digital_output(headlight_pin, LOW);
    }
    if(throttle==0 || steering==0){
      set_motors(0,0);
      motor1Out=0;
      motor2Out=0;
    }
    else{
      motor1Out = (throttle-neutralThrottlePulse) - (steering-neutralSteeringPulse);
      motor2Out = -(throttle-neutralThrottlePulse) - (steering-neutralSteeringPulse);
      //motor1Out = -(255);
      //motor2Out = (255);
    }

    set_motors(motor1Out, motor2Out);
  }
  return 0;
}

void set_neutral_pulse(int* neutralThrottlePulse, int *neutralSteeringPulse){
  static struct PulseInputStruct pulseInfo;
  set_digital_output(headlight_pin, HIGH);

  do{
    get_pulse_info(0, &pulseInfo);
  }while((get_ticks()-pulseInfo.lastPCTime) > 1000);
  *neutralThrottlePulse=(pulse_to_microseconds(pulseInfo.lastHighPulse));

  do{
    get_pulse_info(1, &pulseInfo);
  }while((get_ticks()-pulseInfo.lastPCTime) > 1000);
  *neutralSteeringPulse=(pulse_to_microseconds(pulseInfo.lastHighPulse));

  set_digital_output(headlight_pin, LOW);

}

void read_pins(int* throttle, int* steering, int* ch3){

  red_led(1);

  static struct PulseInputStruct pulseInfo;
  get_pulse_info(0, &pulseInfo);

  //freeze if signal drops
  if((get_ticks()-pulseInfo.lastPCTime) > 200000){
    *throttle = 0;
    *steering = 0;
    return;
  }

  *throttle = (pulse_to_microseconds(pulseInfo.lastHighPulse));

  get_pulse_info(1, &pulseInfo);
  *steering = (pulse_to_microseconds(pulseInfo.lastHighPulse));

  if((get_ticks()-pulseInfo.lastPCTime) > 200000){
    *throttle = 0;
    *steering = 0;
    return;
  }

  get_pulse_info(2, &pulseInfo);
  *ch3 = (pulse_to_microseconds(pulseInfo.lastHighPulse));


  red_led(0);
  return;
}
