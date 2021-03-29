// Denis Labrecque, March 2021

#include <Servo.h> // Use of Servo.h disables Analog Write on pins 9 and 10 (PWM)

// Nano pins 11, 10, 9, 6, 5, and 3 are PWM.
const byte PIN_PWM_RUDDER = A0;
const byte OUT_RADAR = A1;
const byte OUT_STABILIZER1 = A2;
const byte OUT_STABILIZER2 = A3;

const unsigned short RADAR_SPEED = 60; // Adjust the radar to 15 rpm in servo microseconds

Servo rudder_servo;
Servo radar_servo;
Servo stabilizer1;
Servo stabilizer2;

void setup_servos() {
  rudder_servo.attach(PIN_PWM_RUDDER);
  radar_servo.attach(OUT_RADAR);
  stabilizer1.attach(OUT_STABILIZER1);
  stabilizer2.attach(OUT_STABILIZER2);
}

void control_rudder(short microseconds) {
  rudder_servo.writeMicroseconds(microseconds);
  stabilizer1.writeMicroseconds(microseconds);
  stabilizer2.writeMicroseconds(microseconds);
}

void control_rudder(float rudder) {
  // Rudder connected to external power source but Arduino pin
  short rudderDegrees = map(rudder, -1.0f, 1.0f, 0, 180);
  rudder_servo.write(rudderDegrees); // Position in degrees from 0 to 180, 90 being the midpoint
  stabilizer1.write(rudderDegrees);
  stabilizer2.write(rudderDegrees);
  Serial.print(" Rudder: ");
  Serial.print(rudderDegrees);
}

void animate_radar(short microseconds) {
  if(microseconds > PWM_MIDPOINT) {
    if(radar_servo.attached() == false)
      radar_servo.attach(OUT_RADAR);
    
    radar_servo.writeMicroseconds(PWM_MIDPOINT + RADAR_SPEED);
  }
  else if(radar_servo.attached())
    radar_servo.detach();
}

// Radar controlled by continuous rotation servo.
// Based on video footage, the S1850M radar rotates about once every 4 seconds.
// Occasionally, the radar is turned off while in transit to port.
void animate_radar(float primarySwitch, float secondarySwitch) {
  if(isToggled(primarySwitch)) {
    radar_servo.writeMicroseconds(PWM_MIDPOINT + RADAR_SPEED);
    Serial.print(" Radar on ");
  }
  else {
    radar_servo.writeMicroseconds(PWM_MIDPOINT);
    Serial.print(" Radar off ");
  }
}

// The forbin has three garage doors, one for the helicopter deck, and two side bay doors for rigid inflatable boats.
// Here's, let's pretend there's only the helicopter deck door manipulated by a continuous rotation servo.
// Vertical moves the door up or down, and horizontal changes control between doors.
void animate_doors(float primarySwitch, float vertical, float horizontal) {
  if(isToggled(primarySwitch)) {
    Serial.print(" Primary: servos");
    
    if(abs(vertical) > SWITCH_TOLERANCE) {
      // TODO: some sort of stopping code that prevents going past limits (like counting time)
      if(sign(vertical) == 1) {
        Serial.print(" Heli door up");
        //heli_door_servo.writeMicroseconds(PWM_MIDPOINT + GARAGE_SPEED);
      }
      else {
        Serial.print(" Heli door down");
        //heli_door_servo.writeMicroseconds(PWM_MIDPOINT - GARAGE_SPEED);
      }
    }

    // TODO this will require some debounce and real control code, but wanted it in serial monitor just for fun
    if(abs(horizontal) > SWITCH_TOLERANCE) {
      if(sign(horizontal) == -1)
        Serial.print(" Next door");
      else
        Serial.print(" Previous door");
    }
  }
}
