#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button {
  public:
    Button(uint8_t pin, int long_press_duration=1000);
    void begin();
    bool isPressed();
    bool wasPressed();
    bool longPressDetected();
    void reset();
    void update(); // 新增一个函数声明
  private:
    enum State { // 新增一个枚举类型
      IDLE,
      DEBOUNCE,
      PRESSED,
      RELEASED,
      LONG_PRESSED
    };
    uint8_t _pin;
    int _long_press_duration;
    State _state; // 新增一个状态变量
    unsigned long _lastStateChange; // 新增一个时间变量
    bool _isPressed;
    bool _wasPressed;
    bool _longPressDetected;
};

#endif
