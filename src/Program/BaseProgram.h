#ifndef BASE_PROGRAM_H
#define BASE_PROGRAM_H
class BaseProgram {
public:
    virtual void update(float bright) = 0;
    virtual void begin() = 0;
};
#endif // BASE_PROGRAM_H