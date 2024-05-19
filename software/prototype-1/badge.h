#ifndef BADGE_H_
#define BADGE_H_

//syntactic sugar
typedef const int GPIOpin;
typedef const int Digital_Input;
typedef const int Analog_Input;
typedef const int Digital_Output;

typedef unsigned long Time;


#ifndef NDEBUG
#define LOG Serial.print
#define LOGln Serial.println
#else
#define LOG (void)
#define LOGln (void)
#endif

#include <stdio.h>
#include <string.h>

const Time BOOST_CONVERTER_STARTUP_ms = 500;
const unsigned long SERIAL_SPEED = 115200;
void setup_serial()
{
	Serial.begin(SERIAL_SPEED);
	LOGln("badge is up");
}

#endif//BADGE_H_
