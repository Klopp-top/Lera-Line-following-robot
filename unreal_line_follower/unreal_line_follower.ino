#include "config.h"

bool isRunning = false;
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);

  initMotors();
  initSensors();
  pinMode(BUTTON, INPUT);
  resetPID();
  ayat();

  // Инициализация WiFi и веб-сервера
  //initWiFi();

  delay(START_DELAY);
  Serial.println("Ready! Press button to start/stop");
}

void loop() {
  //handleWebServer();
  static bool lastButton = LOW;
  bool currentButton = digitalRead(BUTTON);

  if (currentButton == HIGH && lastButton == LOW) {
    delay(50);  // debounce
    isRunning = !isRunning;
    if (isRunning) {
      Serial.println(">>> STARTED <<<");
      resetPID();
    } else {
      Serial.println(">>> STOPPED <<<");
      stopMotors();
    }
  }
  lastButton = currentButton;

  if (isRunning) {

    int s0 = digitalRead(SENSOR_PINS[0]);
    int s1 = digitalRead(SENSOR_PINS[1]);
    int s2 = digitalRead(SENSOR_PINS[2]);
    int s3 = digitalRead(SENSOR_PINS[3]);
    int s4 = digitalRead(SENSOR_PINS[4]);
    int s5 = digitalRead(SENSOR_PINS[5]);
    int s6 = digitalRead(SENSOR_PINS[6]);
    int s7 = digitalRead(SENSOR_PINS[7]);

    // Линия по центру - едем прямо
    if (s3 == 1 || s4 == 1) {
      setMotorLeft(BASE_SPEED);
      setMotorRight(BASE_SPEED);
    }
    // Линия слева - поворот налево
    else if (s0 == 1 || s1 == 1 || s2 == 1) {
      setMotorLeft(30);
      setMotorRight(BASE_SPEED);
    }
    // Линия справа - поворот направо
    else if (s5 == 1 || s6 == 1 || s7 == 1) {
      setMotorLeft(BASE_SPEED);
      setMotorRight(30);
    }
    // Линия не видна - едем прямо
    else {
      setMotorLeft(BASE_SPEED);
      setMotorRight(BASE_SPEED);
    }
  }
}