#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
  public:
    Buzzer(uint8_t pin);
    void beep(unsigned int frequency, unsigned long duration);
    void tone(unsigned int frequency);
    void noTone();
  private:
    uint8_t _pin;
};

#endif /* BUZZER_H */
