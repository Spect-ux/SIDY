// Motor A (left wheel)
const int PWMA = 25;
const int AIN1 = 26;
const int AIN2 = 27;

// Motor B (right wheel)
const int BIN1 = 32;
const int BIN2 = 33;
const int PWMB = 18;

// Driver standby (must be HIGH for the motors to run)
const int STBY = 4;

// Onboard LED (GPIO2 on most ESP32 dev boards)
const int LED_PIN = 2;

// PWM settings
const int PWM_FREQ = 5000;  // Hz
const int PWM_RES  = 8;     // 8-bit: 0-255

// Slow main speed (0-255). TT motors often stall below ~60-80.
// Raise in steps of 10 if the wheels just hum or twitch.
const int Main = 70;

// Extra speed added to the outer wheel on curves. The inner wheel stays at
// Main so it never drops below the stall speed.
const int CURVE_BOOST = 30;

// Per-wheel trim to correct drifting when driving "straight".
// If the robot pulls left, try TRIM_RIGHT = 5 (or the reverse).
const int TRIM_LEFT  = 0;
const int TRIM_RIGHT = 0;

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  ledcAttach(PWMA, PWM_FREQ, PWM_RES);
  ledcAttach(PWMB, PWM_FREQ, PWM_RES);

  digitalWrite(STBY, HIGH);  // enable the driver
}

// Signed speed: -255..255. Sign = direction, magnitude = PWM.
void setMotor(int pwmPin, int in1, int in2, int speed) {
  speed = constrain(speed, -255, 255);
  if (speed > 0) {
    digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  } else if (speed < 0) {
    digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
  } else {
    digitalWrite(in1, LOW);  digitalWrite(in2, LOW);
  }
  ledcWrite(pwmPin, abs(speed));
}

// Set each wheel independently. Positive = forward, negative = reverse.
// Assumes A = left wheel, B = right wheel (swap if yours is the other way).
void drive(int left, int right) {
  // Apply trim only to the wheel that's actually moving
  if (left  > 0) left  += TRIM_LEFT;   else if (left  < 0) left  -= TRIM_LEFT;
  if (right > 0) right += TRIM_RIGHT;  else if (right < 0) right -= TRIM_RIGHT;

  setMotor(PWMA, AIN1, AIN2, left);
  setMotor(PWMB, BIN1, BIN2, right);
}

// --- Movements ---
void forward()    { drive(Main, Main); }
void backward()   { drive(-Main, -Main); }

// Gentle curves: outer wheel faster, inner wheel at Main
void curveLeft()  { drive(Main, Main + CURVE_BOOST); }
void curveRight() { drive(Main + CURVE_BOOST, Main); }

// Sharp turns: one wheel stops, the other drives
void pivotLeft()  { drive(0, Main); }
void pivotRight() { drive(Main, 0); }

// Spin on the spot: wheels run in opposite directions
void spinLeft()   { drive(-Main, Main); }
void spinRight()  { drive(Main, -Main); }

void stopAll()    { drive(0, 0); }

void loop() {
  // LED solid on while moving, off while stopped
  digitalWrite(LED_PIN, HIGH);

  forward();     delay(2000);
  curveRight();  delay(1500);
  forward();     delay(1000);
  spinLeft();    delay(800);   // tune this delay for a 90 degree turn

  stopAll();
  digitalWrite(LED_PIN, LOW);
  delay(2000);

  // Reverse: LED blinks while motors run
  backward();
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_PIN, HIGH); delay(250);
    digitalWrite(LED_PIN, LOW);  delay(250);
  }

  stopAll();
  delay(2000);
}
