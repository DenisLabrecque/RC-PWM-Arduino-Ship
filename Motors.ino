// Denis Labrecque, March 2021

// Nano pins 11, 10, 9, 6, 5, and 3 are PWM.
const int PIN_PWM_MOTOR1 = A3; // TODO change
const uint8_t DIR_MOTOR1 = A1; // TODO change
const int PIN_PWM_MOTOR2 = A4; // TODO change
const uint8_t DIR_MOTOR2 = A2; // TODO change

const float DEADZONE = 0.05f; // Percent positive and negative throttle that is nonreactive

unsigned long now;                        // timing variables to update data at a regular interval                  
unsigned long rc_update;
const int channels = 6;                   // specify the number of receiver channels
float RC_in[channels];                    // an array to store the calibrated input from receiver

void setup() {
    setup_pwmRead();
    setup_motors();
    setup_servos();
    Serial.begin(9600);
}


void loop() {    
    now = millis();
    
    if(RC_avail() || now - rc_update > 25)   // if RC data is available or 25ms has passed since last update (adjust to be equal or greater than the frame rate of receiver) {
      rc_update = now;                           
    //print_RCpwm();                        // uncommment to print raw data from receiver to serial

    control(RC_decode(1), RC_decode(2), RC_decode(4), RC_decode(3), RC_decode(5), RC_decode(6));
}


// Since all the inputs are passed in, here is an easy place to delegate all control responsibility.
void control(float throttle, float rudder, float vertical, float horizontal, float switch1, float switch2) {
  mix_motors(throttle, rudder);
  control_rudder(rudder);
  animate_radar(switch2);
}


// Essential for operation.
// https://tutorial.cytron.io/2015/04/05/using-mdd10a-arduino-uno/
void mix_motors(float throttle, float rudder) {
  // Throttles get controlled via MDD10A board
  // Control motor direction
  short forwards;
  if(DEADZONE < throttle)
    forwards = 1;
  else if(throttle < -DEADZONE)
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
  
  Serial.println();
  Serial.print("Speed1: ");
  Serial.print(speed1 * forwards1);
  Serial.print(" Speed2: ");
  Serial.print(speed2 * forwards2);
  digitalWrite(DIR_MOTOR1, forwards1);
  digitalWrite(DIR_MOTOR2, forwards2);
  analogWrite(PIN_PWM_MOTOR1, speed1);
  analogWrite(PIN_PWM_MOTOR2, speed2);
}


void setup_motors() {
  pinMode(PIN_PWM_MOTOR1, OUTPUT);
  pinMode(DIR_MOTOR1, OUTPUT);
  pinMode(PIN_PWM_MOTOR2, OUTPUT);
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
