#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8, 4000000); // CE, CSN, set SPI speed to 4MHz instead of default 10MHz
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
  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // Initialize radio
  radio.begin();
  radio.openWritingPipe(address);
  // Setting PA level to HIGH was OK directly from Arduino + capacitor, and gave noticeably better range.
  // Setting PA level to MAX was not OK, though; servo was twitching even up close.
  radio.setPALevel(RF24_PA_HIGH);
  radio.setPayloadSize(sizeof(Data_Package));
  radio.setCRCLength(RF24_CRC_16);
  radio.stopListening();
}

void loop() {
  // Read
  int analogRudder = analogRead(A0);
  int analogThrottle = analogRead(A1);

  // Map
  data.rudder = mapPotentiometerDeadzone(analogRudder, 3);// map(analogRudder, 0, 1023, -105, 105);
  data.throttle = mapPotentiometerDeadzone(analogThrottle, 0); //map(analogThrottle, 0, 1023, -105, 105);

  // print out the value you read:
  Serial.print("Throttle: ");
  Serial.print(data.throttle);
  Serial.print(" Rudder: ");
  Serial.println(data.rudder);

  // Radio -- send
  radio.write(&data, sizeof(Data_Package));
}

int mapPotentiometerDeadzone(int analogValue, int deadzone) {
  int mapped = map(analogValue, 0, 1023, -100 - deadzone, 100 + deadzone);
  if (mapped >= 0) {
    if (mapped < deadzone)
      return 0;
    return mapped - deadzone;
  }
  else {
    if (mapped > -deadzone)
      return 0;
    return mapped + deadzone;
  }
}