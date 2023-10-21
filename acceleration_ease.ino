const int throttlePin = A0; // Analog pin where throttle (e.g., potentiometer) is connected
const int motorPin = 3;     // Digital pin where motor driver PWM input is connected

int throttleValue = 0;      // Current reading from the throttle
int lastThrottleValue = 0;  // Last throttle reading, used for debounce

int motorSpeed = 0;         // Current speed sent to the motor
int targetSpeed = 0;        // Desired speed based on throttle position

// Adjusting 'accelerationFactor' changes the "sharpness" of the ease-in acceleration.
// Examples:
// - A value of 0.5 would give a very rapid response to throttle changes.
// - A value of 0.01 would provide a very gentle and slow response to throttle changes.
float accelerationFactor = 0.14;

// Speed difference threshold: If the difference between the target speed and the 
// current motor speed is below this value, motor speed is directly set to target speed.
// Examples:
// - A value of 5 would cause the motor to match the target speed sooner.
// - A value of 0 would always use the ease-in logic.
int speedDifferenceThreshold = 1;

// Debounce settings for the throttle:
// Examples:
// - If you find throttle readings are too jittery, increase 'debounceThreshold'.
// - If throttle responsiveness feels laggy, decrease 'debounceInterval'.
const int debounceThreshold = 15;
unsigned long lastThrottleChangeMillis = 0; // Timestamp of the last valid throttle change
const unsigned long debounceInterval = 50;  // Time interval for debounce

void setup() {
  pinMode(throttlePin, INPUT);
  pinMode(motorPin, OUTPUT);
  analogWrite(motorPin, motorSpeed);
  Serial.begin(9600);
}

void loop() {
  throttleValue = analogRead(throttlePin);

  // Debounce mechanism ensures that only significant changes in the throttle
  // position (beyond the 'debounceThreshold') or changes that persist 
  // beyond the 'debounceInterval' are taken into account.
  if (abs(throttleValue - lastThrottleValue) > debounceThreshold || 
      millis() - lastThrottleChangeMillis > debounceInterval) {
      
    lastThrottleValue = throttleValue; // Update last throttle value
    lastThrottleChangeMillis = millis(); // Update timestamp of last change

    // Convert throttle reading to a speed range of 0-255 for the motor
    targetSpeed = map(throttleValue, 0, 1023, 0, 255);

    int speedDifference = targetSpeed - motorSpeed;

    // Ease-in acceleration logic: adjust motor speed based on the difference 
    // between target and current speed and the acceleration factor.
    if (abs(speedDifference) <= speedDifferenceThreshold) {
      motorSpeed = targetSpeed;
    } else {
      motorSpeed += accelerationFactor * speedDifference;
    }

    // Ensure the motor speed stays within the allowable PWM range (0-255)
    if (motorSpeed < 45) {
      motorSpeed = 45;   // Example: This can be adjusted to set a different minimum speed for the motor.
    } else if (motorSpeed > 255) {
      motorSpeed = 255;  // Safety clamp to ensure PWM value doesn't exceed limits.
    }
  
    analogWrite(motorPin, motorSpeed); // Send speed to motor
    Serial.println(accelerationFactor * speedDifference);
  }

    delay(10);
}
