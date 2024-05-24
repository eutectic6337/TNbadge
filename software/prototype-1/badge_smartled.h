#include "controller.h"
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
  struct color_transition* tx;//array of n transitions
  unsigned n;
  unsigned slow_down;
  unsigned speed_up;
}
struct color_loop {
  struct color_sequence* s[];//s[i]==0 => last sequence in loop
  unsigned slow_down;
  unsigned speed_up;
}
struct color_step {
  Time next;
  signed delta;
  Time delta_t;
  Time started;
  struct color_transition* tx;
  struct color_sequence* seq;
  struct color_loop* loop;
};

struct brightness_transition {
  Time ms;
  fract8 target;// 0== off, -1== full on
};
struct brightness_sequence {
  struct brightness_transition* tx;//array of n transitions
  unsigned n;
  unsigned slow_down;
  unsigned speed_up;
}
struct brightness_loop {
  struct brightness_sequence* s[];//s[i]==0 => last sequence in loop
  unsigned slow_down;
  unsigned speed_up;
}
struct brightness_step {
  Time next;
  signed delta;
  Time delta_t;
  Time started;
  struct brightness_transition* tx;
  struct brightness_sequence* seq;
  struct brightness_loop* loop;
};

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
  {0,-1}, {10,-1},
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
  {100,CRGB::Red},
  {100,CRGB::Orange},
  {100,CRGB::Yellow},
  {100,CRGB::Green},
  {100,CRGB::Blue},
  {100,CRGB::Violet}};//cycle=600

#include "00-smartled_patterns.h"

const struct color_sequence rs = {
  rainbow, sizeof rainbow,
  1,1
};
const struct color_loop rl = {
  {&rs, 0},
  1,1
};

struct LED_state {
  CRGB color;
  struct color_step Rstep;
  struct color_step Gstep;
  struct color_step Bstep;

  unsigned brightness;
  struct brightness_step bright_step;
} city[NUM_SMART_LEDS];

void setup_city_smartLEDs() {
  pinMode(smart_LED_data, OUTPUT);
  pinMode(smart_LED_clock, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  FastLED.addLeds<APA102, smart_LED_data, smart_LED_clock, BGR>(leds, NUM_SMART_LEDS);  // BGR ordering is typical
	FastLED.setBrightness(100);
  FastLED.setMaxPowerInMilliWatts(500);
}

int any_LED_changed = 0;
void update_city(int i) {
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
    struct brightness_loop* l = city[i].bright_step.loop;
    struct brightness_sequence* s = city[i].bright_step.seq;
    struct brightness_transition* n = city[i].bright_step.tx + 1;
    if (n >= s.tx + n) {// at end of sequence
      s++;
      if (*s == 0) {// at end of loop
        s = l->s[0];
      }
      n = s->t[0];
    }
    city[i].bright_step.started = now;
    signed d = n->target - city[i].brightness;
    if (n->ms == 0) {
      city[i].bright_step.delta = d;
      city[i].bright_step.delta_t = 0;
    }
    else if (d == 0) {
      city[i].bright_step.delta = 0;
      city[i].bright_step.delta_t = n->ms;
    }
    else {
      value abs_d = abs(d);
      if (abs_d < n->ms) {
        if (d < 0) {
          city[i].bright_step.delta = -1;
        }
        else if (d > 0) {
          city[i].bright_step.delta = 1;
        }
        city[i].bright_step.delta_t = n->ms / abs_d;
      }
      else {
        city[i].bright_step.delta_t = 1;
        city[i].bright_step.delta = d / n->ms;
      }
    }
    if (s->slow_down) { city[i].bright_step.delta_t *= s->slow_down; }
    if (l->slow_down) { city[i].bright_step.delta_t *= l->slow_down; }
    if (s->speed_up) { city[i].bright_step.delta_t /= s->speed_up; }
    if (l->speed_up) { city[i].bright_step.delta_t /= l->speed_up; }

    city[i].bright_step.tx = n;
    city[i].bright_step.next = city[i].bright_step.started + city[i].bright_step.delta;
  }

  //Red
  if (now >= city[i].Rstep.next) {
    any_LED_changed = 1;
    city[i].color += CRGB(city[i].Rstep.delta, 0, 0);
    city[i].Rstep.next += city[i].Rstep.delta_t;
  }
  if (now > city[i].Rstep.started + city[i].Rstep.tx->ms) {
    struct color_loop* l = city[i].Rstep.loop;
    struct color_sequence* s = city[i].Rstep.seq;
    struct color_transition* n = city[i].Rstep.tx + 1;
    if (n >= s.tx + n) {// at end of sequence
      s++;
      if (*s == 0) {// at end of loop
        s = l->s[0];
      }
      n = s->t[0];
    }
    city[i].Rstep.started = now;
    signed d = n->target - city[i].color.red;
    if (n->ms == 0) {
      city[i].Rstep.delta = d;
      city[i].Rstep.delta_t = 0;
    }
    else if (d == 0) {
      city[i].Rstep.delta = 0;
      city[i].Rstep.delta_t = n->ms;
    }
    else {
      value abs_d = abs(d);
      if (abs_d < n->ms) {
        if (d < 0) {
          city[i].Rstep.delta = -1;
        }
        else if (d > 0) {
          city[i].Rstep.delta = 1;
        }
        city[i].Rstep.delta_t = n->ms / abs_d;
      }
      else {
        city[i].Rstep.delta_t = 1;
        city[i].Rstep.delta = d / n->ms;
      }
    }
    if (s->slow_down) { city[i].Rstep.delta_t *= s->slow_down; }
    if (l->slow_down) { city[i].Rstep.delta_t *= l->slow_down; }
    if (s->speed_up) { city[i].Rstep.delta_t /= s->speed_up; }
    if (l->speed_up) { city[i].Rstep.delta_t /= l->speed_up; }

    city[i].Rstep.tx = n;
    city[i].Rstep.next = city[i].Rstep.started + city[i].Rstep.delta;
  }

  //Green
  if (now >= city[i].Gstep.next) {
    any_LED_changed = 1;
    city[i].color += CRGB(0, city[i].Gstep.delta, 0);
    city[i].Gstep.next += city[i].Gstep.delta_t;
  }
  if (now > city[i].Gstep.started + city[i].Gstep.tx->ms) {
    struct color_loop* l = city[i].Gstep.loop;
    struct color_sequence* s = city[i].Gstep.seq;
    struct color_transition* n = city[i].Gstep.tx + 1;
    if (n >= s.tx + n) {// at end of sequence
      s++;
      if (*s == 0) {// at end of loop
        s = l->s[0];
      }
      n = s->t[0];
    }
    city[i].Gstep.started = now;
    signed d = n->target - city[i].color.green;
    if (n->ms == 0) {
      city[i].Gstep.delta = d;
      city[i].Gstep.delta_t = 0;
    }
    else if (d == 0) {
      city[i].Gstep.delta = 0;
      city[i].Gstep.delta_t = n->ms;
    }
    else {
      value abs_d = abs(d);
      if (abs_d < n->ms) {
        if (d < 0) {
          city[i].Gstep.delta = -1;
        }
        else if (d > 0) {
          city[i].Gstep.delta = 1;
        }
        city[i].Gstep.delta_t = n->ms / abs_d;
      }
      else {
        city[i].Gstep.delta_t = 1;
        city[i].Gstep.delta = d / n->ms;
      }
    }
    if (s->slow_down) { city[i].Gstep.delta_t *= s->slow_down; }
    if (l->slow_down) { city[i].Gstep.delta_t *= l->slow_down; }
    if (s->speed_up) { city[i].Gstep.delta_t /= s->speed_up; }
    if (l->speed_up) { city[i].Gstep.delta_t /= l->speed_up; }

    city[i].Gstep.tx = n;
    city[i].Gstep.next = city[i].Gstep.started + city[i].Gstep.delta;
  }

  //Blue
  if (now >= city[i].Bstep.next) {
    any_LED_changed = 1;
    city[i].color += CRGB(0, 0, city[i].Bstep.delta);
    city[i].Bstep.next += city[i].Bstep.delta_t;
  }
  if (now > city[i].Bstep.started + city[i].Bstep.tx->ms) {
    struct color_loop* l = city[i].Bstep.loop;
    struct color_sequence* s = city[i].Bstep.seq;
    struct color_transition* n = city[i].Bstep.tx + 1;
    if (n >= s.tx + n) {// at end of sequence
      s++;
      if (*s == 0) {// at end of loop
        s = l->s[0];
      }
      n = s->t[0];
    }
    city[i].Bstep.started = now;
    signed d = n->target - city[i].color.blue;
    if (n->ms == 0) {
      city[i].Bstep.delta = d;
      city[i].Bstep.delta_t = 0;
    }
    else if (d == 0) {
      city[i].Bstep.delta = 0;
      city[i].Bstep.delta_t = n->ms;
    }
    else {
      value abs_d = abs(d);
      if (abs_d < n->ms) {
        if (d < 0) {
          city[i].Bstep.delta = -1;
        }
        else if (d > 0) {
          city[i].Bstep.delta = 1;
        }
        city[i].Bstep.delta_t = n->ms / abs_d;
      }
      else {
        city[i].Bstep.delta_t = 1;
        city[i].Bstep.delta = d / n->ms;
      }
    }
    if (s->slow_down) { city[i].Bstep.delta_t *= s->slow_down; }
    if (l->slow_down) { city[i].Bstep.delta_t *= l->slow_down; }
    if (s->speed_up) { city[i].Bstep.delta_t /= s->speed_up; }
    if (l->speed_up) { city[i].Bstep.delta_t /= l->speed_up; }

    city[i].Bstep.tx = n;
    city[i].Bstep.next = city[i].Bstep.started + city[i].Bstep.delta;
  }

  //MAKE IT SO
  led[i] = city[i].color.nscale8(city[i].brightness);

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

#endif//BADGE_SMARTLED_H_
