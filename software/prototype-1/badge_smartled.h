#ifndef BADGE_SMARTLED_H_
#define BADGE_SMARTLED_H_

#ifdef ENABLE_SMARTLEDS
// ======== blinky outputs - city smartLEDs
#include <SPI.h>
//FIXME: add ESP32C3 hardware SPI support to FastLED
#define FASTLED_USE_GLOBAL_BRIGHTNESS 1
#include <FastLED.h>


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

/*
each LED has nominal colour and brightness
colour and brightness each follow loops

loop is array of sequences which restarts after the last executes
sequence is array of transitions, speed-adjusted
transition is (linear) ramp of steps from current to target over time
(for color transition, use separate steps for R,G,B)

loop (and components therof) would probably be best implemented using templates,
but I'm insufficiently comfortable with that C++ feature yet, so I'll use
copy-paste for now
*/
struct color_transition {
  Time ms;
  CRGB target;
};
struct color_sequence {
  unsigned slow_down;
  unsigned speed_up;
  struct color_transition* tx;
  unsigned n;
}
struct color_step {
  Time next;
  signed delta;
  Time delta_t;
  Time started;
  struct color_sequence* tx;
};
struct color_loop {
  unsigned slow_down;
  unsigned speed_up;
  struct color_sequence* s[];
}

struct brightness_transition {
  Time ms;
  fract8 target;// 0== off, -1== full on
};
struct brightness_sequence {
  unsigned slow_down;
  unsigned speed_up;
  struct brightness_transition* tx;
  unsigned n;
}
struct brightness_step {
  Time next;
  signed delta;
  Time delta_t;
  Time started;
  struct brightness_sequence* tx;
};
struct brightness_loop {
  unsigned slow_down;
  unsigned speed_up;
  struct brightness_sequence* s[];
}

const struct brightness_transition flash_99pc[] = {
  {0,-1}, {990,-1},
  {0,0}, {10,0}}; //cycle=1000
const struct brightness_transition flash_90pc[] = {
  {0,-1}, {900,-1},
  {0,0}, {100,0}}; //cycle=1000
const struct brightness_transition flash_50pc[] = {
  {0,-1}, {500,-1},
  {0,0}, {500,0}}; //cycle=1000
const struct brightness_transition flash_10pc[] = {
  {0,-1}, {100,-1},
  {0,0}, {900,0}}; //cycle=1000
const struct brightness_transition flash_1pc[] = {
  {0,-1}, 10,-1},
  {0,0}, {990,0}}; //cycle=1000
const struct brightness_transition double_strobe[] = {
  {0,-1}, {10,-1},
  {0,0}, {10,0},
  {0,-1}, {10,-1},
  {0,0}, {970,0}}; //cycle=1000
const struct brightness_transition sawtooth[] = {
  {0,0}, {1000,-1}};//cycle=1000
const struct brightness_transition reverse_sawtooth[] = {
  {0,-1}, {1000,0}};//cycle=1000

const struct color_transition rainbow[] = {
  {100,.c=CRGB::Red},
  {100,.c=CRGB::Orange},
  {100,.c=CRGB::Yellow},
  {100,.c=CRGB::Green},
  {100,.c=CRGB::Blue},
  {100,.c=CRGB::Violet}};//cycle=600


struct LED_state {
  struct color_loop cloop;
  CRGB color;
  struct color_step Rstep, Gstep, Bstep;

  struct brightness_loop bloop;
  unsigned brightness;
  struct brightness_step bstep;
} city[NUM_SMART_LEDS];

int any_LED_changed = 0;
void update_city(int i) {
  const Time LOG_DELAY_ms = 2000;
  static Time log_delay;
  // ======== CUSTOMIZE HERE ========


  if (millis() > s.next) {
    v += s.delta; // and MAKE IT SO
    s.next += s.delta_t;
  }
  if (millis() > s.started + s.tx->ms) {
    s.started = millis();
    struct transition* n = s.tx->next;
    if (!n) { ?? }
    if (!n->ms) { ?? }
    signed d = n->target - v;
    if (d == 0) {
      s.delta = 0;
      s.delta_t = n->ms;
    }
    else {
      value abs_d = abs(d);
      if (abs_d < n->ms) {
        if (d < 0) {
          s.delta = -1;
        }
        else if (d > 0) {
          s.delta = 1;
        }
        s.delta_t = n->ms / abs_d;
      }
      else {
        s.delta_t = 1;
        s.delta = d / n->ms;
      }
    }
    s.tx = n;
    s.next = s.started + s.delta;
  }



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
