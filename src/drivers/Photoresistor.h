#ifndef PHOTORESISTOR_H
#define PHOTORESISTOR_H

#include <Arduino.h>

class Photoresistor {
  public:
    Photoresistor();
    float read();
};

#endif
