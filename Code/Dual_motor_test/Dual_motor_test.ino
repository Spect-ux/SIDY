// ESP32 + TB6612FNG: two motors, slow speed test, status LED
// Written for Arduino-ESP32 core 3.x (see note below for 2.x)
// Core version: ledcAttach(pin, freq, res) is for Arduino-ESP32 core 3.x.
// On core 2.x, replace it with ledcSetup(0, PWM_FREQ, PWM_RES); ledcAttachPin(PWMA, 0); (channel 1 for PWMB), and call ledcWrite(channel, speed) instead of ledcWrite(pin, speed).
// Speed: TEST_SPEED is the only value you need to change. 80 out of 255 is slow, and a lower battery voltage will make it slower still.

// Motor A
const int PWMA = 25;
const int AIN1 = 26;
const int AIN2 = 27;

// Motor B
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

// Slow test speed (0-255). TT motors often stall below ~60-80.
// Raise in steps of 10 if the wheels just hum or twitch.
const int TEST_SPEED = 80;

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

void motorA(int speed, bool forward) {
  digitalWrite(AIN1, forward ? HIGH : LOW);
  digitalWrite(AIN2, forward ? LOW : HIGH);
  ledcWrite(PWMA, speed);
}

void motorB(int speed, bool forward) {
  digitalWrite(BIN1, forward ? HIGH : LOW);
  digitalWrite(BIN2, forward ? LOW : HIGH);
  ledcWrite(PWMB, speed);
}

void stopMotors() {
  ledcWrite(PWMA, 0);
  ledcWrite(PWMB, 0);
  digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW);
}

void loop() {
  // Forward: LED solid on while motors run
  digitalWrite(LED_PIN, HIGH);
  motorA(TEST_SPEED, true);
  motorB(TEST_SPEED, true);
  delay(2000);

  // Stop: LED off
  stopMotors();
  digitalWrite(LED_PIN, LOW);
  delay(1000);

  // Reverse: LED blinks while motors run
  motorA(TEST_SPEED, false);
  motorB(TEST_SPEED, false);
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_PIN, HIGH); delay(250);
    digitalWrite(LED_PIN, LOW);  delay(250);
  }

  stopMotors();
  delay(1000);
}