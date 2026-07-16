#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h> // Use of Servo.h disables Analog Write on pins 9 and 10 (PWM)

// Nano pins 11, 10, 9, 6, 5, and 3 are PWM.
const byte PIN_PWM_RUDDER = 6;
const byte PIN_PWM_STAB_L = 10;
const byte PIN_PWM_STAB_R = 9;
// Cytron motor pins
const byte PIN_PWM_MOTOR1 = 3; // Digital PWM pin (~)
const byte PIN_PWM_MOTOR2 = 5; // Digital PWM pin (~)
const byte DIR_MOTOR1 = 2; // Digital I/O pin
const byte DIR_MOTOR2 = 4; // Digital I/O pin
// nRF24 pins
const byte PIN_CE = A1; // yellow
const byte PIN_CSN = 8; // orange

unsigned long last_reception_millis; // Keep track of when we last got radio reception
Servo rudder_servo;
Servo stab_l_servo;
Servo stab_r_servo;
RF24 radio(PIN_CE, PIN_CSN, 4000000); // CE, CSN, set SPI speed to 4MHz instead of default 10MHz
const byte address[6] = "00001";

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  int throttle = 0;
  int rudder = 0;
  int aux = 0;
  bool horn = false;
  bool lights = false;
};
Data_Package data; // Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  last_reception_millis = 0;

  // Cytron/motors
  pinMode(PIN_PWM_MOTOR1, OUTPUT);
  pinMode(PIN_PWM_MOTOR2, OUTPUT);
  pinMode(DIR_MOTOR1, OUTPUT);
  pinMode(DIR_MOTOR2, OUTPUT);

  // Servos
  rudder_servo.attach(PIN_PWM_RUDDER);
  stab_l_servo.attach(PIN_PWM_STAB_L);
  stab_r_servo.attach(PIN_PWM_STAB_R);

  // nRF24
  start_receiver();

  Serial.print("CRC length: ");
  Serial.println(radio.getCRCLength());
  Serial.print("Data rate: ");
  Serial.println(radio.getDataRate());
  Serial.print("Packet size: ");
  Serial.println(sizeof(Data_Package));
}

void loop()
{
  // Check whether there is data to be received
  if (radio.available())
  {
    Serial.print("Millis: ");
    Serial.println(millis() - last_reception_millis);

    last_reception_millis = millis();
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
  }

  // Something may be wrong, we should be receiving, purge the system
  if (millis() - last_reception_millis > 500)
  {
    radio.flush_rx();
  }

  if (millis() - last_reception_millis > 2000)
  {
    reinitialize_receiver();
    last_reception_millis = millis();
  }

  // Normal case, everything running smoothly
  if (millis() - last_reception_millis < 500) {
    // Servos - position in degrees from 0 to 180, 90 being the midpoint
    short rudderDegrees = map(data.rudder, -100, 100, (180 - 30), (0 + 30)); // Removes 30 degrees to keep linkages safe; mapped "upside-down" for correct direction
    rudder_servo.write(rudderDegrees);
    short stabDegrees = map(data.rudder, -100, 100, (0 + 40), (180 - 40));
    stab_r_servo.write(stabDegrees);
    stab_l_servo.write(stabDegrees);

    // Drive the motors
    // Rotational direction
    if (data.throttle > 0) {
      digitalWrite(DIR_MOTOR1, HIGH);
      digitalWrite(DIR_MOTOR2, HIGH);
    }
    else {
      digitalWrite(DIR_MOTOR1, LOW);
      digitalWrite(DIR_MOTOR2, LOW);
    }
    // Speed
    analogWrite(PIN_PWM_MOTOR1, convert_100_100_to_0_255(data.throttle)); // 0 - 255
    analogWrite(PIN_PWM_MOTOR2, convert_100_100_to_0_255(data.throttle)); // 0 - 255
  }
  // Bad case, cut off power to avoid ramming things
  else
  {
    // Servos
    rudder_servo.write(90);
    stab_l_servo.write(90);
    stab_r_servo.write(90);
    // Motors
    analogWrite(PIN_PWM_MOTOR1, 0);
    analogWrite(PIN_PWM_MOTOR2, 0);
  }

  // Serial.print("Throttle: ");
  // Serial.print(data.throttle);
  // Serial.print(" Rudder: ");
  // Serial.println(data.rudder);
}

// Change radio data (-100 to 100) to range 0 - 255 (motors).
int convert_100_100_to_0_255(int valueSigned) {
  long speed = map(abs(valueSigned), 0, 100, 0, 170); // NOTE: the motors were fast anyway, so removed power from 255
  // Current motors just make noise below a certain speed, so don't send a signal below that speed
  if (speed < 12)
    return 0;
  return speed;
}

short convert_signed_direction_to_HIGH_LOW(int signedDirection) {
  if (signedDirection >= 0)
    return HIGH;
  return LOW;
}

void reinitialize_receiver()
{
  start_receiver();
  last_reception_millis = millis(); // prevent repeated resets
  Serial.println("Radio reinitialized");
}

// Starts or restarts the nRF24 module
void start_receiver()
{
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setPayloadSize(sizeof(Data_Package));
  radio.startListening();
}