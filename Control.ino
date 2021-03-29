// Denis Labrecque, March 2021

// Nano pins 11, 10, 9, 6, 5, and 3 are PWM.
const byte PIN_PWM_MOTOR1 = 3; // Digital PWM pin (~)
const byte PIN_PWM_MOTOR2 = 5; // Digital PWM pin (~)
const byte DIR_MOTOR1 = A6; // Analog pin
const byte DIR_MOTOR2 = A7; // Analog pin

const float DEADZONE = 0.09f; // Percent positive and negative throttle that is nonreactive
const float SWITCH_TOLERANCE = 0.25; // Percent that a stick must reach to activate a function
const float PWM_MIDPOINT = 1500;

void setup() {
  setup_pwmRead();
  setup_motors();
  setup_servos();
  Serial.begin(9600);
}

int pwm_value;
void loop() {
  // RC_decode() gets received inputs in order as a channel number, starting at channel 1, channel 2, etc.
  //control(RC_decode(1), RC_decode(2), RC_decode(3), RC_decode(4), RC_decode(5), RC_decode(6));

  Serial.print("Throttle: ");
  Serial.print(RC_raw(1));
  Serial.print(" Rudder: ");
  Serial.print(RC_raw(2));
  Serial.print(" Gear: ");
  Serial.println(RC_raw(5));

  mix_motors(RC_decode(1), RC_decode(2));
  control_rudder(RC_raw(2));
  animate_radar(RC_raw(5));
}


// Since all the inputs are passed in, here is an easy place to delegate all control responsibility.
// All inputs are from -1.0f to 1.0f.
// Primary switch true: second stick controls doors.
// Primary switch false: second stick controls lights.
void control(float throttle, float aileron, float elevator, float rudder, float gear, float auxiliary) {
  print_received_values(throttle, aileron, elevator, rudder, gear, auxiliary);
  mix_motors(throttle, rudder);
  control_rudder(rudder);
  animate_radar(gear, auxiliary);
  //animate_lights(primarySwitch, horizontal);
}


// Use this to check that the channels are correct
void print_received_values(float throttle, float aileron, float elevator, float rudder, float gear, float auxiliary) {
  Serial.print("Throttle: ");
  Serial.print(throttle);
  Serial.print(" Aileron: ");
  Serial.print(aileron);
  Serial.print(" Elevator: ");
  Serial.print(elevator);
  Serial.print(" Rudder: ");
  Serial.print(rudder);
  Serial.print(" Gear: ");
  Serial.print(gear);
  Serial.print(" Auxiliary: ");
  Serial.println(auxiliary);
}


void test_throttle(short microseconds) {
  digitalWrite(DIR_MOTOR1, HIGH);
  digitalWrite(DIR_MOTOR2, HIGH);

  short proportional;
  if(microseconds < 1400 || microseconds > 1600) {
    proportional = map((long)microseconds, 1000, 2000, 0, 255);
  }
  else {
    proportional = 0;
  }

  analogWrite(PIN_PWM_MOTOR1, proportional);
  analogWrite(PIN_PWM_MOTOR2, proportional);
}


// Simple throttle that mixes only the throttle channel to go forwards
void test_throttle(float throttle) {
  Serial.print("Throttle: ");
  Serial.print(throttle);

  digitalWrite(DIR_MOTOR1, HIGH);
  float proportional = map(abs(throttle), 0.0f, 1.0f, 0, 255);

  Serial.print(" Proportional: ");
  Serial.println(proportional);
  
  analogWrite(PIN_PWM_MOTOR1, proportional);
}


// Essential for operation.
// https://tutorial.cytron.io/2015/04/05/using-mdd10a-arduino-uno/
void mix_motors(float throttle, float rudder) {
  // Throttles get controlled via MDD10A board
  // Control motor direction
  short forwards;
  if(DEADZONE < throttle || DEADZONE < rudder) // Second condition allows in-place turns
    forwards = 1;
  else if(throttle < -DEADZONE || rudder < -DEADZONE)
    forwards = -1;
  else
    forwards = 0;
  
  // Control motor speed from 0-255
  short speed1;
  short speed2;
  short forwards1 = forwards;
  short forwards2 = forwards;
  if(forwards == 0) {
    speed1 = 0;
    speed2 = 0;
  }
  else {
    // Speed is always positive
    throttle = abs(throttle);
    
    // Rudder and throttle are mixed together
    // Map from the deadzone to add full sensitivity back to the reduced range
    speed1 = map(throttle + rudder, DEADZONE, 1.0f, 0, 255);
    speed2 = map(throttle - rudder, DEADZONE, 1.0f, 0, 255);
    
    // Mixing rudder may cause negatives; if speed is negative, we need to reverse-turn thrust
    if(isNegative(speed1)) {
      forwards1 = -forwards1;
      speed1 = abs(speed1);
    }
    if(isNegative(speed2)) {
      forwards2 = -forwards2;
      speed2 = abs(speed2);
    }
  }
  // Clamp values
  speed1 = constrain(speed1, 0, 255);
  speed2 = constrain(speed2, 0, 255);

  // Direction
  if(forwards1 >= 0)
    digitalWrite(DIR_MOTOR1, HIGH);
  else
    digitalWrite(DIR_MOTOR1, LOW);
  if(forwards2 >= 0)
    digitalWrite(DIR_MOTOR2, HIGH);
  else
    digitalWrite(DIR_MOTOR2, LOW);

  // Speed
  analogWrite(PIN_PWM_MOTOR1, speed1);
  analogWrite(PIN_PWM_MOTOR2, speed2);
}


void setup_motors() {
  pinMode(PIN_PWM_MOTOR1, OUTPUT);
  pinMode(PIN_PWM_MOTOR2, OUTPUT);
  pinMode(DIR_MOTOR1, OUTPUT);
  pinMode(DIR_MOTOR2, OUTPUT);
}

// The Arduino function doesn't accept fractional input.
// This is a logical copy.
long map(float x, float in_min, float in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

bool isNegative(short value) {
  return value < 0;
}

bool isToggled(float value) {
  return value > 0;
}

short sign(float value) {
  if(value > 0)
    return 1;
  else
    return -1;
}
