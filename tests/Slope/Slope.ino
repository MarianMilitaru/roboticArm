#define DIRPIN 10
#define STEPPIN 9
#define ENABLEPIN 8
#define ANGLEPIN A0

float previousAngle = 0.0f;

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

float fmapf(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop() {
  float currentAngle = fmapf(analogRead(ANGLEPIN), 0.0f, 4095.0f, 0.0f, 360.0f);
  //Serial.println(currentAngle - previousAngle);
  if (abs(currentAngle - previousAngle) > 10) {
    Serial.println(currentAngle);
    previousAngle = currentAngle;
  }
  digitalWrite(STEPPIN, HIGH);
  delayMicroseconds(1000);
  digitalWrite(STEPPIN, LOW);
  delayMicroseconds(1000);
}
