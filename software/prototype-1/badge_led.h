#ifndef BADGE_LED_H_
#define BADGE_LED_H_

#ifdef ENABLE_LED
// ======== blinky outputs - single LED
void setup_single_LED() {
  pinMode(single_LED, OUTPUT);
}
void loop_single_LED() {
  // ======== CUSTOMIZE HERE ========
  const Time LED_BRIGHTEN_ms = 50; // per ramp-up step
  const unsigned LED_FULL = 200;
  const Time LED_FULL_ms = 100;
  const Time LED_DIMMING_ms = 40;
  const unsigned LED_DARK = 50; // per ramp-down step
  const Time LED_DARK_ms = 200;

  static unsigned fadeValue;
  static Time next_action;
  static enum {brightening, full, dimming, dark} state;

  /* How It Works:
     a Finite State Machine for a pulsing LED has 4 states
     'full' and 'dark' each just wait until their time is up
     'brightening' and 'dimming' each wait for each step in their respective processes
   */
  switch (state) {
  case brightening:
    if (millis() >= next_action) {
      next_action = millis() + LED_BRIGHTEN_ms;
      if (fadeValue < LED_FULL) {
        fadeValue++;
        analogWrite(single_LED, fadeValue);
        break;
      }
      state = full;
    }
    break;

  case full:
    if (millis() >= next_action) {
      next_action = millis() + LED_FULL_ms;
      state = dimming;
    }
    break;

  case dimming:
    if (millis() >= next_action) {
      next_action = millis() + LED_DIMMING_ms;
      if (fadeValue > LED_DARK) {
        fadeValue--;
        analogWrite(single_LED, fadeValue);
        break;
      }
      state = dark;
    }
    break;

  case dark:
    if (millis() >= next_action) {
      next_action = millis() + LED_DARK_ms;
      state = brightening;
    }
    break;
  }
}
#else
#define setup_single_LED() ((void)0)
#define loop_single_LED() ((void)0)
#endif

#endif//BADGE_LED_H_
