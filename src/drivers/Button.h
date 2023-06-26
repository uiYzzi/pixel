#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <Arduino.h>

class Button {
  public:
    Button(uint8_t pin,int long_press_duration=1000);
    void begin();
    bool isPressed();
    bool wasPressed();
    bool longPressDetected();
    void reset();

  private:
    uint8_t _pin;
    int _long_press_duration;
    volatile bool _isPressed;
    volatile bool _wasPressed;
    bool _interruptEnabled;
    volatile bool _longPressDetected;
    volatile unsigned long _pressStartTime;
    volatile unsigned long _lastInterruptTime;
    static void handleInterrupt(void* arg);
};

#endif
