#define DIRPIN 10
#define STEPPIN 9
#define ENABLEPIN 8
#define ANGLEPIN A0

// Calculating angle scaling for faster computing
constexpr float SCALE = 120.0f / 4095.0f;

float targetAngle = 0.0f;
float currentAngle;

void moveMotor(float tolerance = 1.0f, uint8_t maxCorrections = 3);
float smoothSignal(float);

void setup() {

  // Starting serial
  Serial.begin(115200);

  // Pin definitions
  pinMode (DIRPIN, OUTPUT);
  pinMode (STEPPIN, OUTPUT);
  pinMode (ANGLEPIN, INPUT);
  pinMode (ENABLEPIN, OUTPUT);

  digitalWrite(ENABLEPIN, LOW);
  analogReadResolution(12);
}

void loop() {
  targetAngle = 360 - (random(0, 35) * 10);
  Serial.print("Target angle: ");
  Serial.println(targetAngle);
  moveMotor();

  delay(3000);
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

