#include <Arduino.h>
#include <Wire.h>
#include "ESP32_Definitions.h"
#include "CMD_Definitions.h"

// Calculating angle scaling for faster computing
constexpr float SCALE = 120.0f / 4095.0f;

uint8_t currentCommand;
uint8_t status;
uint8_t direction;
float currentAngle;
float targetAngle;

void receiveEvent (int);
void requestEvent (void);
float smoothSignal(float);
void moveMotor(float tolerance = 1.0f, uint8_t maxCorrections = 3);

void setup() {

  // Board settings
  analogReadResolution(12);
  pinMode(DIRPIN, OUTPUT);
  pinMode(STEPPIN, OUTPUT);
  pinMode(ENABLEPIN, OUTPUT);

  // Pins initialization
  digitalWrite(ENABLEPIN, LOW);

  // Serial initialization
  Serial.begin(115200);
  
  // I2C initialization
  Wire.setClock(400000);
  Wire.begin(ADD1);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // Motor status initialization
  status = READY;
}

/*!
* @brief This function is called when master sends data
* @param bytes Holds the number of bytes going to be recieved
*/
void receiveEvent(int bytes) {

  if (bytes < 1) return; // return if there is not one byte sent

  currentCommand = Wire.read(); // first byte = command

  switch (currentCommand) {
    case ANGLE: {
      // 5 bytes sent in total
      // byte 1: command
      // byte 2-5: angle
      if(bytes >= 1 + sizeof(float)) {
        Wire.readBytes((uint8_t*)&targetAngle, sizeof(targetAngle));
      }
      status = NOTREADY;
      currentCommand = 0b00000000;
      break;
    }
    case DIR: {
      // 2 bytes sent in total
      // byte 1: command
      // byte 2: direction
      if (bytes >= 1 + sizeof(uint8_t)) {
        Wire.readBytes((uint8_t*)&direction, sizeof(direction));
      }
      currentCommand = 0b00000000;
      break;
    }
    default: break;
  }
}

void requestEvent(void) {
  switch (currentCommand) {
    case REQANGLE: {
      currentAngle = smoothSignal(ANGLEPIN);
      Wire.write((uint8_t*)&currentAngle, sizeof(currentAngle));
      currentCommand = 0b00000000;
      break;
    }
    case REQSTATUS: {
      Wire.write((uint8_t*)&status, sizeof(status));
      currentCommand = 0b00000000;
      break;
    }
    default: break;
  }
}


void loop() {
  float error = targetAngle - currentAngle;
  if (abs(error) < 2.0f) {
    moveMotor();
  }
} 

float smoothSignal (float pin) {
  return ((analogRead(pin) + analogRead(pin) + analogRead(pin)) * SCALE);
}

void moveMotor(float tolerance, uint8_t maxCorrections) {

  // Stop counter
  uint8_t corrections = 0;
  int8_t signChange = 0;

  // Read angle and compute error
  float error = smoothSignal(ANGLEPIN) - targetAngle;

  while (abs(error) > tolerance) {

    // Compute stopping condition
    if ((error > 0 && signChange < 0) || (error < 0 && signChange > 0)) corrections++;
    signChange = (error > 0) - (error < 0);

    // Stop closed loop after some steps
    if (corrections >= maxCorrections) break;
    
    // Choose rotation direction depending on the error
    digitalWrite(DIRPIN, error < 0 ? LOW : HIGH);

    // Move motor
    digitalWrite(STEPPIN, HIGH);
    delayMicroseconds(600);
    digitalWrite(STEPPIN, LOW);
    delayMicroseconds(600);

    // Update feedback
    error = smoothSignal(ANGLEPIN) - targetAngle;
  }
  status = READY;
}