// Denis Labrecque, March 2021

#include <Servo.h> // Use of Servo.h disables Analog Write on pins 9 and 10 (PWM)

// Nano pins 11, 10, 9, 6, 5, and 3 are PWM.
const uint8_t PIN_PWM_RUDDER = 11; // TODO change
const uint8_t PIN_PWM_RADAR = 12; // TODO change

Servo rudder_servo;
Servo radar_servo;

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
void animate_radar(float switch2) {
  if(switch2)
    radar_servo.writeMicroseconds(1700); // TODO adjust speed to 15 rpm
}
