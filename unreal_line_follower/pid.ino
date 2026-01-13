#include "config.h"

float lastError = 0;
float integral = 0;

float PID(float error) {
  float P = error;
  
  integral += error;
  if(integral > INTEGRAL_LIMIT) integral = INTEGRAL_LIMIT;
  if(integral < -INTEGRAL_LIMIT) integral = -INTEGRAL_LIMIT;
  float I = integral;
  
  float D = error - lastError;
  lastError = error;
  
  // Используем глобальные переменные KP, KI, KD
  float correction = (KP * P) + (KI * I) + (KD * D);
  Serial.println("kod v pid");
  
  return correction;
}

void resetPID() {
  lastError = 0;
  integral = 0;
}