/**
 * Demo for RF remote switch receiver.
 * For details, see NewRemoteReceiver.h!
 *
 * Connect the transmitter to digital pin 11.
 *
 * This sketch demonstrates the use of the NewRemoteTransmitter class.
 *
 * This sketch allows you to specify a dim level for a dim-compatible receiver,
 * like the acm-100.
 *
 * When run, open the serial console at 115200 baud. Enter a number between 0 and
 * 15, and hit Send. The receiver will turn on the light, and dims to specified dim
 * level. 0 turns off the light.
 * The actual lower bound is set on your dimmer. Consult dimmer manual to change.
 *
 * NOTE: the actual receivers have the address and group numbers in this example
 * are only for demonstration! If you want to duplicate an existing remote, please
 * try the "retransmitter"-example instead.
 * 
 * To use this actual example, you'd need to "learn" the used code in the receivers
 */

#include <NewRemoteTransmitter.h>

// Create a transmitter on address 123, using digital pin 11 to transmit, 
// with a period duration of 260ms (default), repeating the transmitted
// code 2^3=8 times.
NewRemoteTransmitter transmitter(123, 11, 260, 3);

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available()) {
    // Read the requested level from serial
    int level = Serial.parseInt();

    if (level == 0) {
      Serial.println("Switching off light");
      transmitter.sendUnit(1, false);
    } 
    else if (level >= 1 && level <= 15) {
      Serial.print("Setting dim level to ");
      Serial.println(level);
      transmitter.sendDim(1, level);
    } 
    else {
      Serial.println("Invalid input. dim level must be between 0 and 15!");
    }
  }
}
