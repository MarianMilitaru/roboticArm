#include <Arduino.h>
#include <Wire.h>
#include "ESP32_Definitions.h"
#include "CMD_Definitions.h"

// Calculating angle scaling for faster computing
constexpr float SCALE = 120.0f / 4095.0f;

uint8_t currentCommand;
uint8_t status;
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
  Wire.begin(ADD2);
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
  if(Wire.available() == sizeof(uint8_t)) {
    currentCommand = Wire.read();
  }
  switch (currentCommand) {
  case ANGLE: {
    if(Wire.available() == sizeof(float)) {
      Wire.readBytes((uint8_t*)&targetAngle, sizeof(targetAngle));
    }
    break;
  }
  default:
    break;
  }
}

void requestEvent(void) {
  switch (currentCommand) {
    case REQANGLE: {
      Wire.write((uint8_t*)&currentAngle, sizeof(currentAngle));
      currentCommand = 0b00000000;
      break;
    }
    case REQSTATUS: break;
    default: break;
  }
}


void loop() {

  // Doing the requested computation in the loop 
  // so the response to the master can be almost 
  // instant when interogated
  switch (currentCommand) {
    case ESTOP: {
      digitalWrite(ENABLEPIN, HIGH);
      break;
    }
    case FULLSTEP: break;
    case HALFSTEP: break;
    case QUARTEDSTEP: break;
    case EIGHTHSTEP: break;
    case SIXTEENTHSTEP: break;
    case REQANGLE: {
      currentAngle = smoothSignal(ANGLEPIN);
      break;
    }
    case ANGLE: {
      moveMotor();
      currentCommand = 0b00000000;
      break;
    }
    default:
      break;
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
}