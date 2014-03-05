
/*
* Demo for RF remote switch receiver.
 * For details, see NewRemoteReceiver.h!
 *
 * Connect the transmitter to digital pin 11, and the receiver to digital pin 2.
 *
 * When run, this sketch waits for a valid code from a new-style the receiver,
 * decodes it, and retransmits it after 5 seconds.
 */

#include <NewRemoteReceiver.h>
#include <NewRemoteTransmitter.h>
#include <JeeLib.h>

static byte top, sendLen, dest, quiet; //stack[RF12_MAXDATA+4], value, 
static unsigned long stack[5], value;

void setup() {
  
  Serial.begin(9600);
    
  Serial.println("Welcome. This sketch receives and retransmits homeeasy codes");
  pinMode(A2, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  // See example ShowReceivedCode for info on this
  NewRemoteReceiver::init(1, 2, retransmitter);
}

void loop() {
    if (Serial.available())
    handleInput(Serial.read());
}

void retransmitter(NewRemoteCode receivedCode) {
  // Disable the receiver; otherwise it might pick up the retransmit as well.
  NewRemoteReceiver::disable();
  
  printReceivedCode(receivedCode);

  // Need interrupts for delay()
  interrupts();

  // Wait 50ms seconds (arbitrary) before sending.
  //delay(50);

  // Create a new transmitter with the received address and period, use digital pin 11 as output pin

  NewRemoteTransmitter transmitter(receivedCode.address, 7, receivedCode.period);

  Serial.println("Re-transmitting received code");
  
  if (receivedCode.switchType == NewRemoteCode::dim) {
    // Dimmer signal received
    transmitter.sendDim(receivedCode.unit, receivedCode.dimLevel);
  } else {
    // On/Off signal received
    bool isOn = receivedCode.switchType == NewRemoteCode::on || receivedCode.switchType == NewRemoteCode::dim;

    if (receivedCode.groupBit) {
      // Send to the group
      transmitter.sendGroup(isOn);
    } else {
      // Send to a single unit
      transmitter.sendUnit(receivedCode.unit, isOn);
    }
  }

  NewRemoteReceiver::enable();
}

static void kakuSend(unsigned long addr, byte unit, byte dim, byte on) {
  // Disable the receiver; otherwise it might pick up the retransmit as well.
  NewRemoteReceiver::disable();
  
  // Need interrupts for delay()
  interrupts();

  NewRemoteTransmitter transmitter(addr, A2, 256);
  
  printSendingCode(addr, unit, dim, on);
  
  switch (on) {
    case 0:
      transmitter.sendUnit(unit, false);
      break;    
    case 1:
       transmitter.sendUnit(unit, true);
       break;
    case 2:
      transmitter.sendDim(unit, dim);
      break;
    case 3:
      transmitter.sendGroup(false);
      break;
    case 4:
      transmitter.sendGroup(true);
      break;

  }
  
  NewRemoteReceiver::enable();  
  
}

void printSendingCode(unsigned long addr, byte unit, byte dim, byte on) {
  // Print the received code.
  Serial.print("Sending: Addr ");
  Serial.print(addr);
  
//  if (receivedCode.groupBit) {
//    Serial.print(" group");
//  } else {
    Serial.print(" unit ");
    Serial.print(unit);
//  }
  
  switch (on) {
    case 0:
      Serial.print(" off");
      break;
    case 1:
      Serial.print(" on");
      break;
    case 2:
      Serial.print(" dim level ");
      Serial.print(dim);
      break;
    case 3:
      Serial.print(" group off");
      break;
    case 4:
      Serial.print(" group on");
      break;
 //   case NewRemoteCode::on_with_dim:
   //   Serial.print(" on with dim level ");
     // Serial.print(receivedCode.dimLevel);
     // break;
  }
  
  Serial.print(", period: ");
  Serial.print("256");
  Serial.println("us.");
}

void printReceivedCode(NewRemoteCode receivedCode) {
  // Print the received code.
  Serial.print("Addr ");
  Serial.print(receivedCode.address);
  
  if (receivedCode.groupBit) {
    Serial.print(" group");
  } else {
    Serial.print(" unit ");
    Serial.print(receivedCode.unit);
  }
  
  switch (receivedCode.switchType) {
    case NewRemoteCode::off:
      Serial.print(" off");
      break;
    case NewRemoteCode::on:
      Serial.print(" on");
      break;
    case NewRemoteCode::dim:
      Serial.print(" dim level ");
      Serial.print(receivedCode.dimLevel);
      break;
  }
  
  Serial.print(", period: ");
  Serial.print(receivedCode.period);
  Serial.println("us.");
}

const char helpText1[] PROGMEM = 
  "\n"
  "Available commands:" "\n"
  "  <nn> i     - set node ID (standard node ids are 1..30)" "\n"
  "  <n> b      - set MHz band (4 = 433, 8 = 868, 9 = 915)" "\n"
  "  <nnn> g    - set network group (RFM12 only allows 212, 0 = any)" "\n"
  "  <n> c      - set collect mode (advanced, normally 0)" "\n"
  "  t          - broadcast max-size test packet, request ack" "\n"
  "  ...,<nn> a - send data packet to node <nn>, request ack" "\n"
  "  ...,<nn> s - send data packet to node <nn>, no ack" "\n"
  "  <n> l      - turn activity LED on PB1 on or off" "\n"
  "  <n> q      - set quiet mode (1 = don't report bad packets)" "\n"
  "  <n> x      - set reporting format (0 = decimal, 1 = hex)" "\n"
  "  123 z      - total power down, needs a reset to start up again" "\n"
  "Remote control commands:" "\n"
  "  <hchi>,<hclo>,<addr>,<cmd> f     - FS20 command (868 MHz)" "\n"
  "  <addr>,<dev>,<dim(0-15)>,<0=off,1=on,2=dim,3=groupoff,4=groupon> k              - KAKU command (433 MHz)" "\n"
;

static void showString (PGM_P s) {
  for (;;) {
    char c = pgm_read_byte(s++);
    if (c == 0)
      break;
    if (c == '\n')
      Serial.print('\r');
    Serial.print(c);
  }
}

static void showHelp () {
  showString(helpText1);
//  Serial.println("Current configuration:");
//  rf12_config();
}

static void handleInput (char c) {
  if ('0' <= c && c <= '9')
    value = 10 * value + c - '0';
  else if (c == ',') {
    if (top < sizeof stack)
      stack[top++] = value;
    value = 0;
  } else if ('a' <= c && c <='z') {
    Serial.print("> ");
    for (byte i = 0; i < top; ++i) {
      Serial.print((int) stack[i]);
      Serial.print(',');
    }
    Serial.print((int) value);
    Serial.println(c);
    switch (c) {
      default:
 //       showHelp();
        break;
      case 'i': // set node id
   //     config.nodeId = (config.nodeId & 0xE0) + (value & 0x1F);
     //   saveConfig();
        break;
      case 'b': // set band: 4 = 433, 8 = 868, 9 = 915
        if (value)
   //       config.nodeId = (bandToFreq(value) << 6) + (config.nodeId & 0x3F);
    //    saveConfig();
        break;
      case 'g': // set network group
   //     config.group = value;
     //   saveConfig();
        break;
      case 'c': // set collect mode (off = 0, on = 1)
     //   if (value)
    //      config.nodeId |= COLLECT;
    //    else
    //      config.nodeId &= ~COLLECT;
      //  saveConfig();
        break;
      case 't': // broadcast a maximum size test packet, request an ack
   //     cmd = 'a';
     //   sendLen = RF12_MAXDATA;
     //   dest = 0;
     //   for (byte i = 0; i < RF12_MAXDATA; ++i)
    //      testbuf[i] = i + testCounter;
    //    Serial.print("test ");
    //    Serial.println((int) testCounter); // first byte in test buffer
    //    ++testCounter;
    //    break;
      case 'a': // send packet to node ID N, request an ack
      case 's': // send packet to node ID N, no ack
  //      cmd = c;
   //     sendLen = top;
     //   dest = value;
       // memcpy(testbuf, stack, top);
        break;
      case 'l': // turn activity LED on or off
   //     activityLed(value);
        break;
      case 'f': // send FS20 command: <hchi>,<hclo>,<addr>,<cmd>f
  //      rf12_initialize(0, RF12_868MHZ);
    //    activityLed(1);
      //  fs20cmd(256 * stack[0] + stack[1], stack[2], value);
     //   activityLed(0);
    //    rf12_config(0); // restore normal packet listening mode
        break;
      case 'k': // send KAKU command: <addr>,<dev>,<dim>,<on>k
          
      //  rf12_initialize(0, RF12_433MHZ);
       // activityLed(1);
          kakuSend(stack[0], stack[1], stack[2], value);
    
    //    activityLed(0);
    //    rf12_config(0); // restore normal packet listening mode
        break;
      case 'd': // dump all log markers
   //     if (df_present())
    //      df_dump();
        break;
      case 'r': // replay from specified seqnum/time marker
     //   if (df_present()) {
     //     word seqnum = (stack[0] << 8) || stack[1];
       //   long asof = (stack[2] << 8) || stack[3];
         // asof = (asof << 16) | ((stack[4] << 8) || value);
        //  df_replay(seqnum, asof);
     //   }
        break;
      case 'e': // erase specified 4Kb block
  //      if (df_present() && stack[0] == 123) {
    //      word block = (stack[1] << 8) | value;
      //    df_erase(block);
    //    }
        break;
      case 'w': // wipe entire flash memory
     //   if (df_present() && stack[0] == 12 && value == 34) {
       //   df_wipe();
        //  Serial.println("erased");
      //  }
        break;
      case 'q': // turn quiet mode on or off (don't report bad packets)
    //    quiet = value;
        break;
      case 'z': // put the ATmega in ultra-low power mode (reset needed)
   //     if (value == 123) {
     //     delay(10);
       //   rf12_sleep(RF12_SLEEP);
       //   cli();
       ///   Sleepy::powerDown();
       // }
        break;
      case 'x': // set reporting mode to hex (1) or decimal (0)
//        useHex = value;
        break;
      case 'v': //display the interpreter version
  //      displayVersion(1);
        break;
    }
    value = top = 0;
    memset(stack, 0, sizeof stack);
  } else if (c == '>') {
    // special case, send to specific band and group, and don't echo cmd
    // input: band,group,node,header,data...
    stack[top++] = value;
 //   rf12_initialize(stack[2], bandToFreq(stack[0]), stack[1]);
 //   rf12_sendNow(stack[3], stack + 4, top - 4);
 //   rf12_sendWait(2);
 //   rf12_config(0); // restore original band, etc
    value = top = 0;
    memset(stack, 0, sizeof stack);
  } else if (' ' < c && c < 'A')
    showHelp();
}
