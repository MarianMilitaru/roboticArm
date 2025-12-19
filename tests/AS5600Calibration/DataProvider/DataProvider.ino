#include <cmath>

#define DIRPIN 10
#define STEPPIN 9
#define ENABLEPIN 8
#define ANGLEPIN A0
#define time 100/16

uint16_t numberOfSteps = 200;
uint16_t currentStep = 1;
float idealAngle = 0;
float readAngle = 0;
float startAngle;

// Calculating angle scaling for faster computing
constexpr float SCALE = 120.0f / 4095.0f;

float smoothSignal(float);
void moveMotor(float tolerance = 1.0f, uint8_t maxCorrections = 30);

void setup() {

  // Starting serial
  Serial.begin(115200);

  // Pin definitions
  pinMode (DIRPIN, OUTPUT);
  pinMode (STEPPIN, OUTPUT);
  pinMode (ANGLEPIN, INPUT);
  pinMode (ENABLEPIN, OUTPUT);
  analogReadResolution(12);

  // Microstep resolution
  numberOfSteps *= 16;

  moveMotor();
  digitalWrite(DIRPIN, HIGH);    // clockwise
}

float smoothSignal (float pin) {
  return ((analogRead(pin) + analogRead(pin) + analogRead(pin)) * SCALE);
}

void loop() {
  float currentAngle = smoothSignal(ANGLEPIN);
  idealAngle = 0.1125 * currentStep;
  float error = idealAngle - currentAngle;

  Serial.print(currentAngle);
  Serial.print("/");
  Serial.print(idealAngle);
  Serial.print("/");
  Serial.print(error);
  Serial.print("/");
  Serial.println(currentStep);
  
  digitalWrite(STEPPIN, HIGH);
  delayMicroseconds(time);
  digitalWrite(STEPPIN, LOW);
  delayMicroseconds(time);
  currentStep++;
  if (currentStep >= numberOfSteps) {currentStep = 0; idealAngle = 0;}
}

void moveMotor(float tolerance, uint8_t maxCorrections) {

  float targetAngle = 360.0f;

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
    //delayMicroseconds(time);
    digitalWrite(STEPPIN, LOW);
    //delayMicroseconds(time);

    // Update feedback
    error = smoothSignal(ANGLEPIN) - targetAngle;
    Serial.print(".");
  }
}
