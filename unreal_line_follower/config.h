#ifndef CONFIG_H
#define CONFIG_H

#define PWMA 15
#define AIN1 2
#define AIN2 4

#define PWMB 19
#define BIN1 5
#define BIN2 18

#define BUTTON 21

const int SENSOR_PINS[8] = {32, 33, 25, 26, 27, 14, 12, 13};
#define NUM_SENSORS 8

const char* WIFI_SSID = "Xadi";
const char* WIFI_PASSWORD = "12345678";

#define EEPROM_SIZE 64
#define ADDR_KP 0
#define ADDR_KI 4
#define ADDR_KD 8
#define ADDR_BASE_SPEED 12
#define ADDR_MAX_SPEED 16
#define EEPROM_MAGIC 20
#define MAGIC_VALUE 0xAB

#define DEFAULT_KP 15.0
#define DEFAULT_KI 0.0
#define DEFAULT_KD 5.0
#define DEFAULT_BASE_SPEED 80
#define DEFAULT_MAX_SPEED 150

#define LOOP_DELAY 5
#define START_DELAY 1000
#define INTEGRAL_LIMIT 100

#define PWM_FREQ 5000
#define PWM_RESOLUTION 8

extern float KP;
extern float KI;
extern float KD;
extern int BASE_SPEED;
extern int MAX_SPEED;

extern float lastError;
extern float integral;

// Functions
float PID(float error);
void resetPID();

void initMotors();
void setMotorLeft(int speed);
void setMotorRight(int speed);
void stopMotors();

void initSensors();
float readPosition();

void initWiFi();
void handleWebServer();
void loadParameters();
void saveParameters();

#endif