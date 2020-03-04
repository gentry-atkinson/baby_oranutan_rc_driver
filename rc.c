/************************************************
*Author: Gentry Atkinson
*Organization: Bobcat Robotics
*Date: 26 Feb 2020
*Description: this file is being written as a driver for the fairyweight robot
*    Uncle Traveling Matt but it should serve as a general purpose program for
*    tank-style drive with a 3-channel mixed controller.
************************************************/

#include <pololu/orangutan.h>
#include <arduino.h>

#define headlight_pin IO_D7

#define throttle_pin IO_B0
#define steering_pin IO_B1
#define ch3_pin IO_B2

void read_pins(int*, int*, int*, int);
int set_neutral_pulse();

int main()
{
  set_motors(0, 0);
  red_led(1);
  delay_ms(500);
  set_digital_output(headlight_pin, LOW);
  set_digital_input(throttle_pin, PULL_UP_ENABLED);
  set_digital_input(steering_pin, PULL_UP_ENABLED);
  set_digital_input(ch3_pin, PULL_UP_ENABLED);
  red_led(0);

  int throttle=0, steering=0, ch3=0;
  int neutralPulse = set_neutral_pulse();
  while(1)
  {
    read_pins(&throttle, &steering, &ch3, neutralPulse);

    set_digital_output(headlight_pin, HIGH);
    delay_ms(throttle*10);
    set_digital_output(headlight_pin, LOW);
    delay_ms(1000-throttle*10);

  }
}

int set_neutral_pulse(){
  int t_counter = 0;
  long int timeout = 10000000;
  while(is_digital_input_high(throttle_pin)&&timeout-- > 0);
  while(!is_digital_input_high(throttle_pin)&&timeout-- > 0);
  while(is_digital_input_high(throttle_pin)&&timeout-- > 0){
    t_counter += 1;
    delay_us(10);
  }
  return t_counter;
}

//receiver input is assumed to range from 1ms to 2ms
//outputs will be set from 0 to 100
void read_pins(int* throttle, int* steering, int* ch3, int neutralPulse){
  red_led(10);
  int t_counter = 0, s_counter = 0, ch3_counter = 0;
  long int timeout = 20000000;

  pulseIn(throttle_pin, HIGH);

  while(is_digital_input_high(throttle_pin)&&timeout-- > 0);
  while(!is_digital_input_high(throttle_pin)&&timeout-- > 0);
  while(is_digital_input_high(throttle_pin)&&timeout-- > 0){
    t_counter += 1;
    delay_us(10);
  }

  while(is_digital_input_high(steering_pin)&&timeout-- > 0);
  while(!is_digital_input_high(steering_pin)&&timeout-- > 0);
  while(is_digital_input_high(steering_pin)&&timeout-- > 0){
    s_counter += 1;
    delay_us(10);
  }

  while(is_digital_input_high(ch3_pin)&&timeout-- > 0);
  while(!is_digital_input_high(ch3_pin)&&timeout-- > 0);
  while(is_digital_input_high(ch3_pin)&&timeout-- > 0){
    ch3_counter += 1;
    delay_us(10);
  }

  if(t_counter < 0 || t_counter > 10000){
    *throttle = 50;
  }
  else{
    *throttle = ((t_counter-neutralPulse)/neutralPulse)*50 + 50;
  }

  if(s_counter < 1 || s_counter > 10000){
    *steering = 50;
  }
  else{
    *steering = ((s_counter-neutralPulse)/neutralPulse)*50 + 50;
  }

  if(ch3_counter < 1 || ch3_counter > 10000){
    *ch3 = 50;
  }
  else{
    *ch3 = ((ch3_counter-neutralPulse)/neutralPulse)*50 + 50;
  }

  if(*throttle>100) *throttle=100;
  else if(*throttle<0) *throttle=0;

  if(*steering>100) *steering=100;
  else if(*steering<0) *steering=0;

  if(*ch3>100) *ch3=100;
  else if(*ch3<0) *ch3=0;

  red_led(0);
  return;
}
