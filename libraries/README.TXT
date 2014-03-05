433MHz radio libraries for Arduino

This package contains four libraries for use with Arduino:

 - RemoteSensor: send and receive data of a weather station with remote 
   sensors.
 - RemoteSwitch and NewRemoteSwitch: send and receive data of radio controlled
   power switches.
 - InterruptChain: chains Arduino interrupts; allows for receiving both sensor
   and switch controls at the same time.
   
The libraries are well documented with helpful examples to get you started.

Latest source and wiki: https://bitbucket.org/fuzzillogic/433mhzforarduino


QUICKSTART:

 * Copy the directories InterruptChain, NewRemoteSwitch, RemoteSwitch and
   RemoteSensor to the "libraries" directory in your Arduino sketchbook
   directory. See http://arduino.cc/en/Guide/Libraries for detailed
   instructions.
 * Start the Arduino IDE.
 * From the File menu, select Examples -> InterruptChain ->
   ReceiveRemoteAndSensor.
 * Connect a 433MHz receiver to digital pin 2 on the Arduino.
 * Run the sketch.
 * Enabled the serial monitor at 115200 baud.
 * Press buttons on your remote(s), and/or wait for your thermo/hygro sensor
   to transmit.
 * You should see the received data in the serial monitor.