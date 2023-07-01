#include "Button.h"

Button::Button(uint8_t pin, int long_press_duration) : _pin(pin), _long_press_duration(long_press_duration), _state(IDLE), _lastStateChange(0), _isPressed(false), _wasPressed(false), _longPressDetected(false) {}

void Button::begin(){
  pinMode(_pin, INPUT_PULLUP);
}

bool Button::isPressed(){
  return _isPressed;
}

bool Button::wasPressed(){
  if (_wasPressed) {
    _wasPressed = false;
    return true;
  } else {
    return false;
  }
}

bool Button::longPressDetected(){
  if (_longPressDetected) {
    _wasPressed = false;
    _longPressDetected = false;
    return true;
  } else {
    return false;
  }
}

void Button::reset(){
  _wasPressed = false;
  _longPressDetected = false;
}

void Button::update(){ // 新增一个函数来更新状态机
  bool isButtonPressed = digitalRead(_pin) == LOW; // 读取按钮状态
  unsigned long now = millis(); // 获取当前时间
  switch (_state) { // 根据当前状态进行不同的处理
    case IDLE: // 空闲状态，等待按钮按下
      if (isButtonPressed) { // 如果按钮按下了
        _state = DEBOUNCE; // 切换到去抖动状态
        _lastStateChange = now; // 记录状态改变的时间
      }
      break;
    case DEBOUNCE: // 去抖动状态，等待一段时间后确认按钮是否仍然按下
      if (now - _lastStateChange >= 10) { // 如果已经过了10毫秒
        if (isButtonPressed) { // 如果按钮仍然按下
          _state = PRESSED; // 切换到按下状态
          _lastStateChange = now; // 记录状态改变的时间
          _isPressed = true; // 设置_isPressed为true
          _wasPressed = true; // 设置_wasPressed为true
        } else { // 如果按钮已经松开了
          _state = IDLE; // 切换回空闲状态
        }
      }
      break;
    case PRESSED: // 按下状态，等待按钮松开或者长按触发
      if (!isButtonPressed) { // 如果按钮松开了
        _state = RELEASED; // 切换到松开状态
        _lastStateChange = now; // 记录状态改变的时间
      } else if (now - _lastStateChange >= _long_press_duration) { // 如果已经达到长按的时间阈值
        _state = LONG_PRESSED; // 切换到长按状态
        _lastStateChange = now; // 记录状态改变的时间
        _longPressDetected = true; // 设置_longPressDetected为true
      }
      break;
    case RELEASED: // 松开状态，等待一段时间后确认按钮是否仍然松开
      if (now - _lastStateChange >= 10) { // 如果已经过了10毫秒
        if (!isButtonPressed) { // 如果按钮仍然松开
          _state = IDLE; // 切换回空闲状态
          _isPressed = false; // 设置_isPressed为false
        } else { // 如果按钮又被按下了
          _state = DEBOUNCE; // 切换到去抖动状态
          _lastStateChange = now; // 记录状态改变的时间
        }
      }
      break;
    case LONG_PRESSED: // 长按状态，等待按钮松开
      if (!isButtonPressed) { // 如果按钮松开了
        _state = RELEASED; // 切换到松开状态
        _lastStateChange = now; // 记录状态改变的时间
        _isPressed = false; // 设置_isPressed为false
      }
      break;
  }
}
