#include <Arduino.h>
extern "C" void fb_json_putchar(char c) {
    Serial.print(c);
}
