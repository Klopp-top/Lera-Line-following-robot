#include "config.h"

void initMotors() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  
  ledcAttach(PWMA, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(PWMB, PWM_FREQ, PWM_RESOLUTION);
}

void initKnopka(){
  pinMode(BUTTON, INPUT_PULLUP);
}

void setMotorLeft(int speed) {
  digitalWrite(AIN1, speed > 0);
  digitalWrite(AIN2, speed < 0);
  ledcWrite(PWMA, abs(speed));
}

void setMotorRight(int speed) {
  digitalWrite(BIN1, speed > 0);
  digitalWrite(BIN2, speed < 0);
  ledcWrite(PWMB, abs(speed));
}

void stopMotors() {
  setMotorLeft(0);
  setMotorRight(0);
}