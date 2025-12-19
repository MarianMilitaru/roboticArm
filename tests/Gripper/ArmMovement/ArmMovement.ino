// --- Pin Definitions ---
#define DIRPIN 10
#define STEPPIN 9
#define ENABLEPIN 8
#define ANGLEPIN A0

// --- Constants ---
constexpr float ADC_TO_DEG = 72.0f / 4095.0f; // 0-360 deg
const float ANGLE_TOLERANCE = 1.25f; // max allowed error

// --- State ---
float targetAngle = 120.0f;
float currentAngle;
float previousAngle = 0.0f;

// --- Stall logic ---
#define K 0.15
float accumulatedMovement = 0.0f;
bool isStalled = false;
bool shouldResetTimer = true;
unsigned long lastStallCheckTimer = 0;

// --- Max/Min delay ---
const float MAX_DELAY = 30.0f;
const float MIN_DELAY = 500.0f;

// --- Functions ---
float readSmoothAngle (uint8_t);

void setup() {

  // Starting serial
  Serial.begin(115200);

  // Pin definitions
  pinMode(DIRPIN, OUTPUT);
  pinMode(STEPPIN, OUTPUT);
  pinMode(ANGLEPIN, INPUT);
  pinMode(ENABLEPIN, OUTPUT);
  digitalWrite(ENABLEPIN, LOW); // Enable motor
  analogReadResolution(12);
}

void loop() {
  unsigned long currentTime = millis();
  
  // --- INPUTS & SENSOR READING ---
  currentAngle = readSmoothAngle(ANGLEPIN);
  if (Serial.available() > 0) {
    targetAngle = Serial.parseFloat();
    while(Serial.available()) Serial.read(); 

    // --- Reset stall logic on new command ---
    isStalled = false;
    accumulatedMovement = 0;
    shouldResetTimer = true;
    lastStallCheckTimer = 0;
  }

  // --- COMPUTE ERROR ---
  float error = targetAngle - currentAngle;

  // --- CONTROL LOGIC ---
  if ((abs(error) > ANGLE_TOLERANCE) && !isStalled) {

    previousAngle = currentAngle;
    
    // --- Motor Control ---
    int stepDelay = map(constrain(abs(error), 0, 20), 0, 20, MIN_DELAY, MAX_DELAY);
    digitalWrite(DIRPIN, (error > 0) ? HIGH : LOW);
    digitalWrite(STEPPIN, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(STEPPIN, LOW);
    delayMicroseconds(stepDelay);



    // --- STALL DETECTION ---
    currentAngle = readSmoothAngle(ANGLEPIN);
    float da = abs(currentAngle - previousAngle); // Check change
    accumulatedMovement += (K * da);
    if (shouldResetTimer) {
      shouldResetTimer = false;
      lastStallCheckTimer = currentTime;
      accumulatedMovement = 0;
    }
    if ((currentTime - lastStallCheckTimer) >= 200.0f) {
      Serial.println(accumulatedMovement);
      if (accumulatedMovement >= 8.0f) isStalled = true;
      lastStallCheckTimer = currentTime;
      accumulatedMovement = 0;
    }



  } else shouldResetTimer = true;
}

float readSmoothAngle (uint8_t pin) {
  return ((analogRead(pin) + analogRead(pin) + analogRead(pin) + analogRead(pin) + analogRead(pin)) * ADC_TO_DEG);
}