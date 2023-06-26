#include "Buzzer.h"

Buzzer::Buzzer(uint8_t pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void Buzzer::beep(unsigned int frequency, unsigned long duration) {
  tone(frequency);
  delay(duration);
  noTone();
}

void Buzzer::tone(unsigned int frequency) {
  ::tone(_pin, frequency);
}

void Buzzer::noTone() {
  ::noTone(_pin);
}
