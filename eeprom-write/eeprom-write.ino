
#include <EEPROM.h>

float iBat = 100; // Initial Battery Percent
int eeAddress = 0;

void setup() {
    EEPROM.put(eeAddress, iBat);
}

void loop() {
    // Empty
}
