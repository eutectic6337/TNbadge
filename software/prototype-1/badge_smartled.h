#ifndef BADGE_SMARTLED_H_
#define BADGE_SMARTLED_H_

#ifdef ENABLE_SMARTLEDS
// ======== blinky outputs - city smartLEDs
//#include <SPI.h>
//FIXME: add ESP32C3 hardware SPI support to FastLED
//#define FASTLED_USE_GLOBAL_BRIGHTNESS 1
#include <FastLED.h>


enum LED_city_index {
  LED_Memphis, LED_Clarkesville, LED_Nashville, LED_Chattanooga, LED_Knoxville,
  NUM_SMART_LEDS
};
CRGB leds[NUM_SMART_LEDS];

/*
each LED has nominal colour and brightness
colour and brightness each follow sequences

sequence is array of transitions, speed-adjusted
transition is (linear) ramp of steps from current to target over time
(for color transition, use separate steps for R,G,B)

probably be best implemented using templates,
but I'm insufficiently comfortable with that C++ feature yet, so I'll use
copy-paste for now
*/

struct brightness_transition {int ms;int target;};
struct color_transition {int ms;CRGB target;};

#define NELEM(a) ((sizeof (a))/((sizeof (a)[0])))
#define CITY_BRIGHT(n,s,v) \
const double brightness_sequence_speed_##n s;\
const struct brightness_transition brightness_tx_##n [] = v;
#define CITY_BRIGHT_INIT(n) {brightness_sequence_speed_##n,brightness_tx_##n,NELEM(brightness_tx_##n)}

#define CITY_COLOR(n,s,v) \
const double color_sequence_speed_##n s;\
const struct color_transition color_tx_##n [] = v;
#define CITY_COLOR_INIT(n) {color_sequence_speed_##n,color_tx_##n,NELEM(color_tx_##n)}

#define SQUARE_(a,b,r,t) {0,(a)},{(r)*(t)/100,(a)},{0,(b)},{(100-(r))*(t)/100,(b)}
#define SQUARE(a,b,t) SQUARE_(a,b,50,t)

#define TRIANGLE_(a,b,r,t) {(r)*(t)/100,(a)},{(100-(r))*(t)/100,(b)}
#define TRIANGLE(a,b,t) TRIANGLE_(a,b,50,t)

#define SAWTOOTH(a,b,t) TRIANGLE_(a,b,100,t)
#define REVERSE_SAWTOOTH(a,b,t) TRIANGLE_(a,b,0,t)

#define RAINBOW(t) \
  {(t)/6,CRGB::Red},\
  {(t)/6,CRGB::Orange},\
  {(t)/6,CRGB::Yellow},\
  {(t)/6,CRGB::Green},\
  {(t)/6,CRGB::Blue},\
  {(t)/6,CRGB::Violet}

#include "00-smartled_patterns.h"

struct brightness_sequence {
  double speed;
  struct brightness_transition* tx;
  int n;
};
struct color_sequence {
  double speed;
  struct color_transition* tx;
  int n;
};

struct brightness_sequence city_brightness[] = {
  CITY_BRIGHT_INIT(Memphis),
  CITY_BRIGHT_INIT(Clarkesville),
  CITY_BRIGHT_INIT(Nashville),
  CITY_BRIGHT_INIT(Chattanooga),
  CITY_BRIGHT_INIT(Knoxville)
};
struct color_sequence city_color[] = {
  CITY_COLOR_INIT(Memphis),
  CITY_COLOR_INIT(Clarkesville),
  CITY_COLOR_INIT(Nashville),
  CITY_COLOR_INIT(Chattanooga),
  CITY_COLOR_INIT(Knoxville)
};

struct color_step {
  Time next;
  int delta;
  Time delta_t;
  Time started;
  struct color_transition* tx;
  struct color_sequence* seq;
};
struct brightness_step {
  Time next;
  int delta;
  Time delta_t;
  Time started;
  struct brightness_transition* tx;
  struct brightness_sequence* seq;
};

struct LED_state {
  CRGB color;
  struct color_step Rstep;
  struct color_step Gstep;
  struct color_step Bstep;

  int brightness;
  struct brightness_step bright_step;
} city[NUM_SMART_LEDS];

void setup_city_smartLEDs() {
  pinMode(smart_LED_data, OUTPUT);
  pinMode(smart_LED_clock, OUTPUT);
//  SPI.begin();
//  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  FastLED.addLeds<APA102, smart_LED_data, smart_LED_clock, BGR>(leds, NUM_SMART_LEDS);  // BGR ordering is typical
//	FastLED.setBrightness(100);
//  FastLED.setMaxPowerInMilliWatts(500);
}

int any_LED_changed = 0;
void loop_city(int i) {
  const Time LOG_DELAY_ms = 2000;
  static Time log_delay;
  // ======== CUSTOMIZE HERE ========

  Time now = millis();

  //Brightness
  if (now >= city[i].bright_step.next) {
    any_LED_changed = 1;
    city[i].brightness += city[i].bright_step.delta;
    city[i].bright_step.next += city[i].bright_step.delta_t;
  }
  if (now > city[i].bright_step.started + city[i].bright_step.tx->ms) { 
    struct brightness_sequence* s = city[i].bright_step.seq;
    struct brightness_transition* t = city[i].bright_step.tx + 1;
    if (t >= s->tx + s->n) {// at end of sequence
      t = s->tx;
    }
    city[i].bright_step.started = now;
    int d = t->target - city[i].brightness;
    if (t->ms == 0) {
      city[i].bright_step.delta = 0;
      city[i].bright_step.delta_t = t->ms;
    }
    else {
      int abs_d = abs(d);
      if (abs_d < t->ms) {
        if (d < 0) {
          city[i].bright_step.delta = -1;
        }
        else if (d > 0) {
          city[i].bright_step.delta = 1;
        }
        city[i].bright_step.delta_t = t->ms / abs_d;
      }
      else {
        city[i].bright_step.delta_t = 1;
        city[i].bright_step.delta = d / t->ms;
      }
    }
    if (s->speed) { city[i].bright_step.delta_t *= s->speed; }

    city[i].bright_step.tx = t;
    city[i].bright_step.next = city[i].bright_step.started + city[i].bright_step.delta;
  }

  //Red
  if (now >= city[i].Rstep.next) {
    any_LED_changed = 1;
    city[i].color += CRGB(city[i].Rstep.delta, 0, 0);
    city[i].Rstep.next += city[i].Rstep.delta_t;
  }
  if (now > city[i].Rstep.started + city[i].Rstep.tx->ms) {
    struct color_sequence* s = city[i].Rstep.seq;
    struct color_transition* t = city[i].Rstep.tx + 1;
    if (t >= s->tx + s->n) {// at end of sequence
      t = s->tx;
    }
    city[i].Rstep.started = now;
    int d = t->target.red - city[i].color.red;
    if (t->ms == 0) {
      city[i].Rstep.delta = d;
      city[i].Rstep.delta_t = 0;
    }
    else if (d == 0) {
      city[i].Rstep.delta = 0;
      city[i].Rstep.delta_t = t->ms;
    }
    else {
      int abs_d = abs(d);
      if (abs_d < t->ms) {
        if (d < 0) {
          city[i].Rstep.delta = -1;
        }
        else if (d > 0) {
          city[i].Rstep.delta = 1;
        }
        city[i].Rstep.delta_t = t->ms / abs_d;
      }
      else {
        city[i].Rstep.delta_t = 1;
        city[i].Rstep.delta = d / t->ms;
      }
    }
    if (s->speed) { city[i].Rstep.delta_t *= s->speed; }

    city[i].Rstep.tx = t;
    city[i].Rstep.next = city[i].Rstep.started + city[i].Rstep.delta;
  }

  //Green
  if (now >= city[i].Gstep.next) {
    any_LED_changed = 1;
    city[i].color += CRGB(0, city[i].Gstep.delta, 0);
    city[i].Gstep.next += city[i].Gstep.delta_t;
  }
  if (now > city[i].Gstep.started + city[i].Gstep.tx->ms) {
    struct color_sequence* s = city[i].Gstep.seq;
    struct color_transition* t = city[i].Gstep.tx + 1;
    if (t >= s->tx + s->n) {// at end of sequence
      t = s->tx;
    }
    city[i].Gstep.started = now;
    int d = t->target.green - city[i].color.green;
    if (t->ms == 0) {
      city[i].Gstep.delta = d;
      city[i].Gstep.delta_t = 0;
    }
    else if (d == 0) {
      city[i].Gstep.delta = 0;
      city[i].Gstep.delta_t = t->ms;
    }
    else {
      int abs_d = abs(d);
      if (abs_d < t->ms) {
        if (d < 0) {
          city[i].Gstep.delta = -1;
        }
        else if (d > 0) {
          city[i].Gstep.delta = 1;
        }
        city[i].Gstep.delta_t = t->ms / abs_d;
      }
      else {
        city[i].Gstep.delta_t = 1;
        city[i].Gstep.delta = d / t->ms;
      }
    }
    if (s->speed) { city[i].Gstep.delta_t *= s->speed; }

    city[i].Gstep.tx = t;
    city[i].Gstep.next = city[i].Gstep.started + city[i].Gstep.delta;
  }

  //Blue
  if (now >= city[i].Bstep.next) {
    any_LED_changed = 1;
    city[i].color += CRGB(0, 0, city[i].Bstep.delta);
    city[i].Bstep.next += city[i].Bstep.delta_t;
  }
  if (now > city[i].Bstep.started + city[i].Bstep.tx->ms) {
    struct color_sequence* s = city[i].Bstep.seq;
    struct color_transition* t = city[i].Bstep.tx + 1;
    if (t >= s->tx + s->n) {// at end of sequence
      t = s->tx;
    }
    city[i].Bstep.started = now;
    int d = t->target.blue - city[i].color.blue;
    if (t->ms == 0) {
      city[i].Bstep.delta = d;
      city[i].Bstep.delta_t = 0;
    }
    else if (d == 0) {
      city[i].Bstep.delta = 0;
      city[i].Bstep.delta_t = t->ms;
    }
    else {
      int abs_d = abs(d);
      if (abs_d < t->ms) {
        if (d < 0) {
          city[i].Bstep.delta = -1;
        }
        else if (d > 0) {
          city[i].Bstep.delta = 1;
        }
        city[i].Bstep.delta_t = t->ms / abs_d;
      }
      else {
        city[i].Bstep.delta_t = 1;
        city[i].Bstep.delta = d / t->ms;
      }
    }
    if (s->speed) { city[i].Bstep.delta_t *= s->speed; }

    city[i].Bstep.tx = t;
    city[i].Bstep.next = city[i].Bstep.started + city[i].Bstep.delta;
  }

  //MAKE IT SO
  leds[i] = city[i].color.nscale8(city[i].brightness*256/100);

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

  const Time XLED_DELAY_ms = 200;
  static Time xled_delay = 0;

void loop_city_smartLEDs() {
  if (millis() >= xled_delay) {
    for (int i = LED_Memphis; i <= LED_Knoxville; i++) {
      //loop_city(i);
      leds[i].r = random(10,200);
      leds[i].g = random(10,200);
      leds[i].b = random(10,200);
    }
    any_LED_changed = 1;
    xled_delay = millis() + XLED_DELAY_ms;
  }
  if (any_LED_changed) {
    any_LED_changed = 0;
    FastLED.show();
  }
}

#else
#define setup_city_smartLEDs() ((void)0)
#define loop_city_smartLEDs() ((void)0)
#endif

#endif//BADGE_SMARTLED_H_
