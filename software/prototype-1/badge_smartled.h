#ifndef BADGE_SMARTLED_H_
#define BADGE_SMARTLED_H_

#ifdef ENABLE_SMARTLEDS
// ======== blinky outputs - city smartLEDs
#include <SPI.h>
//FIXME: add ESP32C3 hardware SPI support to FastLED
#define FASTLED_USE_GLOBAL_BRIGHTNESS 1
#include <FastLED.h>
//
Digital_Output smart_LED_data = MOSI;
Digital_Output smart_LED_clock = SCK;

enum LED_city_index {
  LED_Memphis, LED_Clarkesville, LED_Nashville, LED_Chattanooga, LED_Knoxville,
  NUM_SMART_LEDS
};
CRGB leds[NUM_SMART_LEDS];
void setup_city_smartLEDs() {
  pinMode(smart_LED_data, OUTPUT);
  pinMode(smart_LED_clock, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  FastLED.addLeds<APA102, smart_LED_data, smart_LED_clock, BGR>(leds, NUM_SMART_LEDS);  // BGR ordering is typical
	FastLED.setBrightness(100);
  FastLED.setMaxPowerInMilliWatts(500);
}
struct {
  Time wait_until;
  CRGB new_LED;
  Time time_step;
  fract8 step;
} city[NUM_SMART_LEDS];
int any_LED_changed = 0;
void update_city(int i) {
  const Time LOG_DELAY_ms = 2000;
  static Time log_delay;
  // ======== CUSTOMIZE HERE ========

  /* How It Works:
     a Finite State Machine for each LED has *n* states
     each fade state waits for each step in its respective processes
   */

  if (millis() >= city[i].wait_until) {
    any_LED_changed = 1;
    city[i].wait_until = millis() + city[i].time_step;
    leds[i] = leds[i].lerp8(city[i].new_LED, city[i].step);
    if (city[i].step == 255) {
      /* set new transition for this city */

      switch (i) {
      case LED_Memphis:
        if (millis() >= log_delay) {
          LOGln("Memphis");
          log_delay = millis() + LOG_DELAY_ms;
        }
        break;
      case LED_Clarkesville:
        if (millis() >= log_delay) {
          LOGln("Clarkesville");
          log_delay = millis() + LOG_DELAY_ms;
        }
        break;
      case LED_Nashville:
        if (millis() >= log_delay) {
          LOGln("Nashville");
          log_delay = millis() + LOG_DELAY_ms;
        }
        break;
      case LED_Chattanooga:
        if (millis() >= log_delay) {
          LOGln("Chattanooga");
          log_delay = millis() + LOG_DELAY_ms;
        }
        break;
      case LED_Knoxville:
        if (millis() >= log_delay) {
          LOGln("Knoxville");
          log_delay = millis() + LOG_DELAY_ms;
        }
        break;
      default:
        if (millis() >= log_delay) {
          LOGln("I have no idea");
          log_delay = millis() + LOG_DELAY_ms;
        }
      }
    }
    city[i].step++;
  }
}
void update_city_smartLEDs() {
  for(int i = LED_Memphis; i <= LED_Knoxville; i++) {
    update_city(i);
  }
  if (any_LED_changed) {
    any_LED_changed = 0;
    FastLED.show();
  }
}

/*
each LED starts at time 0, value 0,0,0
a fade goes from old[r,g,b] to new[r,g,b] over time t

*/
struct LED_fade { // from current to target over time
  unsigned time; // units might be seconds, milliseconds, ...
  union {
    CRGB c;
    struct LED_fade* other;
  };
};

struct LED_state {
  unsigned ticks_to_next_change;
};

// examples // when step[n].time==0, no more steps
const LED_fade white_90pc_flash[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {9,.c=CRGB::Black}, {0}}; //cycle=100
const LED_fade white_50pc_flash[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {99,.c=CRGB::Black}, {0}}; //cycle=200
const LED_fade white_10pc_flash[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {899,.c=CRGB::Black}, {0}}; //cycle=1000
const LED_fade white_1pc_flash[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {9899,.c=CRGB::Black}, {0}}; //cycle=10000
const LED_fade white_double_strobe[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {99,.c=CRGB::Black},
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {9699,.c=CRGB::Black}, {0}}; //cycle=10000
const LED_fade rainbow[] = {
  {100,.c=CRGB::Red},
  {100,.c=CRGB::Orange},
  {100,.c=CRGB::Yellow},
  {100,.c=CRGB::Green},
  {100,.c=CRGB::Blue},
  {100,.c=CRGB::Violet}, {0}};//cycle=600
const LED_fade red_blue_sawtooth[] = {
  {1,.c=CRGB::Black}, {999,.c=CRGB::Red},
  {1,.c=CRGB::Black}, {999,.c=CRGB::Blue}, {0}};//cycle=2000
#else
#define setup_city_smartLEDs() ((void)0)
#define update_city_smartLEDs() ((void)0)
#endif

#ifdef ENABLE_SMARTLEDS
//======== FIXME beyond here ========
void fadeall() { for(int i = 0; i < NUM_SMART_LEDS; i++) { leds[i].nscale8(250); } }

void loop2() { 
	static uint8_t hue = 0;
	Serial.print("x");
	// First slide the led in one direction
	for(int i = 0; i < NUM_SMART_LEDS; i++) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}
	Serial.print("x");

	// Now go in the other direction.  
	for(int i = (NUM_SMART_LEDS)-1; i >= 0; i--) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}
}
#endif

#endif//BADGE_SMARTLED_H_
