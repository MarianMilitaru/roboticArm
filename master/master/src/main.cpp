#include <Arduino.h>
#include <Wire.h>
#include "PI_Definitions.h"
#include "CMD_Definitions.h"

byte ledState;

const uint8_t slave[4] = {0x01, 0x02, 0x03, 0x04};

struct Define{
  float angle;
  float targetAngle;
  uint8_t status;
};

Define* joint[10] = {nullptr};
arduino::MbedI2C Wire1(2, 3);

bool sendCommand (uint8_t, uint8_t);
bool reqData (uint8_t, uint8_t);
bool sendData (uint8_t, uint8_t, float);
bool sendData (uint8_t, uint8_t, uint8_t);

void setup() {

  // Allow the slaves to start first
  delay(2000);
  
  // Serial initialization
  Serial.begin(115200);
  while(!Serial) {
  }

  // I2C initialization
  Wire1.setClock(400000);
  Wire1.begin();

  // LED initialization
  pinMode(LED_BUILTIN, OUTPUT);

  // Joints initialization
  for (int i = 0; i < 4; i++) {
    joint[slave[i]] = new Define{0.0f, 0.0f, NOTREADY};
  }
}

void loop() {
  for (int i = 0; i < 3; i++) {
    while (!sendData(slave[i], ANGLE, joint[slave[i]]->targetAngle)) {
    }
    Serial.print("Angle sent: ");
    Serial.print(joint[slave[i]]->targetAngle);
    Serial.println("!");
    joint[slave[i]]->targetAngle = 360 - (random(0, 37) * 10);
  }
  delay(5000);

  for (int i = 0; i < 3; i++) {
    while (!reqData(slave[i], REQANGLE)) {
    }
    Serial.print("Slave[0x0");
    Serial.print(i+1);
    Serial.print("] angle: ");
    Serial.println(joint[slave[i]]->angle);
  }
  delay(500);
}

bool sendCommand (uint8_t address, uint8_t command) {
  Wire1.beginTransmission(address);
  Wire1.write(command);
  if (Wire1.endTransmission() == 0)
    return true;
  return false;
}

bool reqData (uint8_t address, uint8_t request) {
  if (sendCommand(address, request)) {
    switch (request) {
      case REQANGLE: {
        Wire1.requestFrom(address, (uint8_t)sizeof(float));
        if (Wire1.available() == sizeof(float)) {
          Wire1.readBytes((uint8_t*)&joint[address]->angle, sizeof(float));
        }
        break;
      }
      case REQSTATUS: {
        Wire1.requestFrom(address, (uint8_t)sizeof(uint8_t));
        if (Wire1.available() == sizeof(uint8_t)) {
          Wire1.readBytes((uint8_t*)&joint[address]->status, sizeof(uint8_t));
        }
        break;
      }
      case REQCHECK: {
        break;
      }
      default: break;
    }
    return true;
  } else {
    Serial.print("slave at ");
    Serial.print(address, HEX);
    Serial.println(" not responding!");
    return false;
  }
}

bool sendData (uint8_t address, uint8_t setting, float data) {
  switch (setting) {
    case ANGLE: {
      Wire1.beginTransmission(address);
      Wire1.write(setting);
      Wire1.write((uint8_t*)&data, sizeof(float));
      if ((Wire1.endTransmission() == 0)) {
        return true;
      }
      break;
    }
    default: break;
  }
  return false;
}

bool sendData (uint8_t address, uint8_t setting, uint8_t data) {
  switch (setting) {
  case DIR: {
      Wire1.beginTransmission(address);
      Wire1.write(setting);
      Wire1.write((uint8_t*)&data, sizeof(uint8_t));
      if ((Wire1.endTransmission() == 0)) {
        return true;
      }
      break;
    }
    default: break;
  }
  return false;
}