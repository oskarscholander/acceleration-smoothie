const int throttlePin = A0; // Analog pin where throttle (e.g., potentiometer) is connected
const int motorPin = 3;     // Digital pin where motor driver PWM input is connected

int throttleValue = 0;      // Current reading from the throttle
int lastThrottleValue = 0;  // Last throttle reading, used for debounce

int motorSpeed = 0;         // Current speed sent to the motor
int targetSpeed = 0;        // Desired speed based on throttle position

// PID parameters
float Kp = 0.9;  // Proportional gain
float Ki = 0.05; // Integral gain
float Kd = 0.1;  // Derivative gain

float previousError = 0;
float integral = 0;

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

  if (abs(throttleValue - lastThrottleValue) > debounceThreshold || 
      millis() - lastThrottleChangeMillis > debounceInterval) {

    lastThrottleValue = throttleValue;
    lastThrottleChangeMillis = millis();

    targetSpeed = map(throttleValue, 0, 1023, 0, 255);

    // PID calculations
    float error = targetSpeed - motorSpeed;
    integral += error;
    float derivative = error - previousError;
    float output = Kp * error + Ki * integral + Kd * derivative;

    motorSpeed += output;

    // Clamp motorSpeed to ensure it's within the allowable PWM range (0-255)
    motorSpeed = constrain(motorSpeed, 45, 255);
    analogWrite(motorPin, motorSpeed);

    previousError = error; // Update the previous error

    // Debugging output
    Serial.println(motorSpeed);
  }

  delay(10);
}
