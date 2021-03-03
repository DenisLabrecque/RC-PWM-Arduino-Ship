// Denis Labrecque, March 2021

#include <Servo.h> // Use of Servo.h disables Analog Write on pins 9 and 10 (PWM)

// Nano pins 11, 10, 9, 6, 5, and 3 are PWM.
const uint8_t PIN_PWM_RUDDER = 11; // TODO change
const uint8_t PIN_PWM_RADAR = 12; // TODO change
const uint8_t PIN_PWM_HELI_DOOR = 13; // TODO change, and at this point we may need to use the servo driver board

const unsigned short RADAR_SPEED = 200; // TODO adjust the radar to 15 rpm in servo microseconds
const unsigned short GARAGE_SPEED = 200; // TODO adjust garage door speeds in servo microseconds

Servo rudder_servo;
Servo radar_servo;
Servo heli_door_servo;

void setup_servos() {
  rudder_servo.attach(PIN_PWM_RUDDER);
  radar_servo.attach(PIN_PWM_RADAR);
}

void control_rudder(float rudder) {
  // I'm not sure how the rudder will be controlled yet, because of amperage draw.
  // Maybe have a separate power wire and have Arduino send PWM via Servo?
  // I suppose that's how it usually works through +5v anyway.
  // https://www.instructables.com/Arduino-How-to-Use-a-Servo-Motor-With-an-External-/
  short rudderDegrees = map(rudder, -1.0f, 1.0f, 0, 180);
  rudder_servo.write(rudderDegrees); // Position in degrees from 0 to 180, 90 being the midpoint
  Serial.print(" Rudder: ");
  Serial.print(rudderDegrees);
}

// Radar controlled by continuous rotation servo.
// Based on video footage, the S1850M radar rotates about once every 4 seconds.
// Occasionally, the radar is turned off while in transit to port.
void animate_radar(float primarySwitch, float secondarySwitch) {
  if(isToggled(secondarySwitch)) {
    Serial.print(" Radar toggled on");
    radar_servo.writeMicroseconds(PWM_MIDPOINT + RADAR_SPEED);
  }
  else
    Serial.print(" Radar toggled off");
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
        heli_door_servo.writeMicroseconds(PWM_MIDPOINT + GARAGE_SPEED);
      }
      else {
        Serial.print(" Heli door down");
        heli_door_servo.writeMicroseconds(PWM_MIDPOINT - GARAGE_SPEED);
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
