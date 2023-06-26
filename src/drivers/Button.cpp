#include "Button.h"

Button::Button(uint8_t pin,int long_press_duration) : _pin(pin),_long_press_duration(long_press_duration) ,_isPressed(false),
                                           _wasPressed(false), _interruptEnabled(false),
                                           _longPressDetected(false) {}

void Button::begin() {
  pinMode(_pin, INPUT_PULLUP);

  // Enable interrupt on falling edge
  attachInterruptArg(digitalPinToInterrupt(_pin), &Button::handleInterrupt, this, FALLING);

  _interruptEnabled = true;
}

bool Button::isPressed() {
  return _isPressed;
}

bool Button::wasPressed() {
  if (_wasPressed) {
    _wasPressed = false;
    return true;
  }
  else {
    return false;
  }
}

bool Button::longPressDetected() {
  if (_longPressDetected) {
    _wasPressed = false;
    _longPressDetected = false;
    return true;
  } else {
    return false;
  }
}

void Button::reset() {
  _wasPressed = false;
  _longPressDetected = false;
}

void Button::handleInterrupt(void* arg) {
  Button* button = (Button*) arg;

  // Ignore any interrupts within 10ms of the last one
  unsigned long now = millis();
  if (now - button->_lastInterruptTime < 10) {
    return;
  }
  button->_lastInterruptTime = now;

  // Update button state
  bool isButtonPressed = digitalRead(button->_pin) == LOW;

  if (isButtonPressed) {
    button->_isPressed = true;
    button->_wasPressed = true;
    button->_pressStartTime = now;
  } else {
    button->_isPressed = false;
    unsigned long pressDuration = now - button->_pressStartTime;
    
    // Detect long press
    if (pressDuration >= button->_long_press_duration) {
      button->_longPressDetected = true;
    }
  }
}
