#include "config.h"

void initSensors() {
  for(int i = 0; i < NUM_SENSORS; i++) {
    pinMode(SENSOR_PINS[i], INPUT);
  }
}

float readPosition() {
  int sensorValues[NUM_SENSORS];
  float weightedSum = 0;
  int sum = 0;
  
  for(int i = 0; i < NUM_SENSORS; i++) {
    sensorValues[i] = digitalRead(SENSOR_PINS[i]);
    weightedSum += sensorValues[i] * i;
    sum += sensorValues[i];
  }
  
  if(sum == 0) {
    if(lastError > 0) return 7.0;
    else return 0.0;
  }
  
  return weightedSum / sum;
}