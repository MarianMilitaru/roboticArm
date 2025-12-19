#define DIRPIN 10
#define STEPPIN 9
#define ENABLEPIN 8
#define ANGLEPIN A0

void setup() {

  // Starting serial
  Serial.begin(115200);

  // Pin definitions
  pinMode (DIRPIN, OUTPUT);
  pinMode (STEPPIN, OUTPUT);
  pinMode (ANGLEPIN, INPUT);
  pinMode (ENABLEPIN, OUTPUT);
}

void loop() {
  digitalWrite(STEPPIN, HIGH);
  delayMicroseconds(500);
  digitalWrite(STEPPIN, LOW);
  delayMicroseconds(500);
}
