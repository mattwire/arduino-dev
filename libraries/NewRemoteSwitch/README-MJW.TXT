Added lines:
9: #include "../PinChangeInt/PinChangeInt.h"
55: PCintPort::attachInterrupt(A1, interruptHandler, CHANGE);

This uses PinChangeInt library to activate interrupt on PIN A1 (data input pin)
