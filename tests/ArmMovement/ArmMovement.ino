#define DIRPIN 10
#define STEPPIN 9
#define ENABLEPIN 8
#define ANGLEPIN A0

constexpr float SCALE = 120.0f / 4095.0f;

float previousAngle = 0.0f;

float smoothSignal (float pin) {
  return ((analogRead(pin) + analogRead(pin) + analogRead(pin)) * SCALE);
}

void setup() {

  // Starting serial
  Serial.begin(115200);

  // Pin definitions
  pinMode (DIRPIN, OUTPUT);
  pinMode (STEPPIN, OUTPUT);
  pinMode (ANGLEPIN, INPUT);
  pinMode (ENABLEPIN, OUTPUT);
  analogReadResolution(12);
  digitalWrite(DIRPIN, LOW);    // counterclockwise
}

void loop() {
  // put your main code here, to run repeatedly:
  float currentAngle = smoothSignal(ANGLEPIN);
  Serial.println(currentAngle);
  if (currentAngle <= 30) {
    digitalWrite(DIRPIN, HIGH);
  } else if (currentAngle >= 320) {
    digitalWrite(DIRPIN, LOW);
  }
  digitalWrite(STEPPIN, HIGH);
  delayMicroseconds(300);
  digitalWrite(STEPPIN, LOW);
  delayMicroseconds(300);

}
