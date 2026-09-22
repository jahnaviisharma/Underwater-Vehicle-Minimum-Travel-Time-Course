#include <Arduino.h>

/*
 * Underwater Vehicle - Minimum Travel Time
 * -----------------------------------------
 * Course: Thermofluid Laboratory
 * Author: Jahnavi Sharma
 *
 * ESP32-based dual-motor propulsion control using
 * two analog joysticks and an L298N motor driver.
 *
 * Features:
 * - Automatic joystick center calibration
 * - Moving-average input smoothing
 * - Configurable joystick dead zone
 * - Bidirectional motor control
 * - PWM-based speed control
 * - Independent left/right motor control
 * - Serial monitoring for debugging
 */

// ============================================================
// Joystick Inputs
// ============================================================

const int JOY_LEFT_Y_PIN  = 34;   // Left joystick VRy
const int JOY_RIGHT_Y_PIN = 35;   // Right joystick VRy


// ============================================================
// L298N Motor Driver Pins
// ============================================================

// Left motor
const int L_MOTOR_EN_PIN = 27;    // ENA
const int L_MOTOR_IN1    = 18;    // IN1
const int L_MOTOR_IN2    = 19;    // IN2

// Right motor
const int R_MOTOR_EN_PIN = 14;    // ENB
const int R_MOTOR_IN3    = 25;    // IN3
const int R_MOTOR_IN4    = 26;    // IN4


// ============================================================
// PWM Configuration
// ============================================================

const int PWM_FREQ = 5000;        // PWM frequency in Hz
const int PWM_RES  = 8;            // 8-bit resolution: 0-255


// ============================================================
// Joystick Calibration
// ============================================================

int leftJoyCenter  = 2048;
int rightJoyCenter = 2048;


// ============================================================
// ADC and Motor Control Parameters
// ============================================================

const int ADC_MIN = 0;
const int ADC_MAX = 4095;

const int DEADZONE = 350;

// Minimum PWM required to overcome motor friction
const int MIN_MOTOR_SPEED = 30;

const int MOTOR_MIN = -255;
const int MOTOR_MAX = 255;


// ============================================================
// Input Smoothing
// ============================================================

const int SAMPLES = 5;

// Moving-average buffers
int leftReadings[SAMPLES];
int rightReadings[SAMPLES];

// Separate running totals for each joystick
long leftTotal  = 0;
long rightTotal = 0;

int readIndex = 0;


// ============================================================
// Read Smoothed Joystick Value
// ============================================================

int readJoystickSmoothed(
  int pin,
  int readings[],
  long &total
) {
  // Remove the oldest reading
  total -= readings[readIndex];

  // Read the latest joystick value
  readings[readIndex] = analogRead(pin);

  // Add the new reading
  total += readings[readIndex];

  // Return moving average
  return total / SAMPLES;
}


// ============================================================
// Initialize Smoothing Buffers
// ============================================================

void initializeSmoothingBuffers() {

  leftTotal = 0;
  rightTotal = 0;

  for (int i = 0; i < SAMPLES; i++) {

    leftReadings[i] = leftJoyCenter;
    rightReadings[i] = rightJoyCenter;

    leftTotal += leftReadings[i];
    rightTotal += rightReadings[i];
  }
}


// ============================================================
// Calibrate Joystick Centers
// ============================================================

void calibrateJoysticks() {

  Serial.println();
  Serial.println("Calibrating joysticks...");
  Serial.println("Keep both joysticks in the NEUTRAL position.");
  
  delay(2000);

  long leftSum = 0;
  long rightSum = 0;

  const int calibrationSamples = 50;

  for (int i = 0; i < calibrationSamples; i++) {

    leftSum += analogRead(JOY_LEFT_Y_PIN);
    rightSum += analogRead(JOY_RIGHT_Y_PIN);

    delay(20);
  }

  leftJoyCenter =
    leftSum / calibrationSamples;

  rightJoyCenter =
    rightSum / calibrationSamples;

  Serial.print("Left joystick center: ");
  Serial.println(leftJoyCenter);

  Serial.print("Right joystick center: ");
  Serial.println(rightJoyCenter);

  Serial.println("Joystick calibration complete.");
  Serial.println();
}


// ============================================================
// Convert Joystick ADC Reading to Motor Speed
// ============================================================

int mapADCToSpeed(int adcValue, int center) {

  // ----------------------------------------------------------
  // Neutral / Dead Zone
  // ----------------------------------------------------------

  if (
    adcValue > center - DEADZONE &&
    adcValue < center + DEADZONE
  ) {
    return 0;
  }


  // ----------------------------------------------------------
  // Forward Direction
  // ----------------------------------------------------------

  if (adcValue >= center + DEADZONE) {

    int speed = map(
      adcValue,
      center + DEADZONE,
      ADC_MAX,
      MIN_MOTOR_SPEED,
      MOTOR_MAX
    );

    return constrain(
      speed,
      MIN_MOTOR_SPEED,
      MOTOR_MAX
    );
  }


  // ----------------------------------------------------------
  // Reverse Direction
  // ----------------------------------------------------------

  if (adcValue <= center - DEADZONE) {

    int speed = map(
      adcValue,
      ADC_MIN,
      center - DEADZONE,
      MOTOR_MIN,
      -MIN_MOTOR_SPEED
    );

    return constrain(
      speed,
      MOTOR_MIN,
      -MIN_MOTOR_SPEED
    );
  }

  return 0;
}


// ============================================================
// Drive a Motor
// ============================================================

void driveMotor(
  int motorSpeed,
  int in1,
  int in2,
  int enPin
) {

  // ----------------------------------------------------------
  // Stop Motor
  // ----------------------------------------------------------

  if (motorSpeed == 0) {

    // Disable motor output
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);

    ledcWrite(enPin, 0);

    return;
  }


  // ----------------------------------------------------------
  // Calculate PWM Duty Cycle
  // ----------------------------------------------------------

  int duty = abs(motorSpeed);

  duty = constrain(duty, 0, 255);


  // ----------------------------------------------------------
  // Forward Direction
  // ----------------------------------------------------------

  if (motorSpeed > 0) {

    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }


  // ----------------------------------------------------------
  // Reverse Direction
  // ----------------------------------------------------------

  else {

    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }


  // ----------------------------------------------------------
  // Apply PWM
  // ----------------------------------------------------------

  ledcWrite(enPin, duty);
}


// ============================================================
// Setup
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("====================================");
  Serial.println(" Underwater Vehicle Motor Control");
  Serial.println("====================================");


  // ----------------------------------------------------------
  // Configure Motor Pins
  // ----------------------------------------------------------

  pinMode(L_MOTOR_IN1, OUTPUT);
  pinMode(L_MOTOR_IN2, OUTPUT);

  pinMode(R_MOTOR_IN3, OUTPUT);
  pinMode(R_MOTOR_IN4, OUTPUT);

  pinMode(L_MOTOR_EN_PIN, OUTPUT);
  pinMode(R_MOTOR_EN_PIN, OUTPUT);


  // ----------------------------------------------------------
  // Force Motors to Stop During Startup
  // ----------------------------------------------------------

  digitalWrite(L_MOTOR_IN1, LOW);
  digitalWrite(L_MOTOR_IN2, LOW);

  digitalWrite(R_MOTOR_IN3, LOW);
  digitalWrite(R_MOTOR_IN4, LOW);


  // ----------------------------------------------------------
  // Configure PWM
  // ----------------------------------------------------------

  ledcAttach(
    L_MOTOR_EN_PIN,
    PWM_FREQ,
    PWM_RES
  );

  ledcAttach(
    R_MOTOR_EN_PIN,
    PWM_FREQ,
    PWM_RES
  );


  // Ensure motors start stopped
  ledcWrite(L_MOTOR_EN_PIN, 0);
  ledcWrite(R_MOTOR_EN_PIN, 0);


  delay(500);


  // ----------------------------------------------------------
  // Calibrate Joysticks
  // ----------------------------------------------------------

  calibrateJoysticks();


  // ----------------------------------------------------------
  // Initialize Smoothing Buffers
  // ----------------------------------------------------------

  initializeSmoothingBuffers();


  Serial.println("Dual joystick motor control ready.");
  Serial.println();
}


// ============================================================
// Main Control Loop
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // Move to Next Position in Moving-Average Buffer
  // ----------------------------------------------------------

  readIndex = (readIndex + 1) % SAMPLES;


  // ----------------------------------------------------------
  // Read and Smooth Joystick Inputs
  // ----------------------------------------------------------

  int rawLeftY = readJoystickSmoothed(
    JOY_LEFT_Y_PIN,
    leftReadings,
    leftTotal
  );

  int rawRightY = readJoystickSmoothed(
    JOY_RIGHT_Y_PIN,
    rightReadings,
    rightTotal
  );


  // ----------------------------------------------------------
  // Convert Joystick Inputs to Motor Commands
  // ----------------------------------------------------------

  int leftMotorSpeed = mapADCToSpeed(
    rawLeftY,
    leftJoyCenter
  );

  int rightMotorSpeed = mapADCToSpeed(
    rawRightY,
    rightJoyCenter
  );


  // ----------------------------------------------------------
  // Drive Motors
  // ----------------------------------------------------------

  driveMotor(
    leftMotorSpeed,
    L_MOTOR_IN1,
    L_MOTOR_IN2,
    L_MOTOR_EN_PIN
  );

  driveMotor(
    rightMotorSpeed,
    R_MOTOR_IN3,
    R_MOTOR_IN4,
    R_MOTOR_EN_PIN
  );


  // ----------------------------------------------------------
  // Serial Debug Output
  // Only print while motors are moving
  // ----------------------------------------------------------

  if (
    leftMotorSpeed != 0 ||
    rightMotorSpeed != 0
  ) {

    Serial.print("Left Joystick: ");
    Serial.print(rawLeftY);

    Serial.print(" | Left Motor: ");
    Serial.print(leftMotorSpeed);

    Serial.print(" || Right Joystick: ");
    Serial.print(rawRightY);

    Serial.print(" | Right Motor: ");
    Serial.println(rightMotorSpeed);
  }


  // ----------------------------------------------------------
  // Control Update Period
  // 20 ms ≈ 50 Hz
  // ----------------------------------------------------------

  delay(20);
}
