/*
  Tennessee Cybersecurity / dc615
  2024 blinky-badge
  (https://github.com/eutectic6337/TNbadge)
 */

//
// NOTES FOR BADGE WEARERS
//
// 1. each intended customization location is marked
//    "// ======== CUSTOMIZE HERE ========"
//
// 2. each feature uses a state machine,
//    initialized in setup_FEATURE(),
//    maintained in update_FEATURE()
//

/* remove leading // to enable; add // to disable feature */
//#define ENABLE_EPAPER
//#define ENABLE_SMARTLEDS
#define ENABLE_LED
//#define ENABLE_BLUETOOTH
//#define ENABLE_WIFI
#define ENABLE_BATTERYMONITOR
#define ENABLE_LIGHTDARK

/* resources:
  https://learn.adafruit.com/multi-tasking-the-arduino-part-1
 */

// define to disable progress logging to serial port
//#define NDEBUG
#include "badge.h"
#include "badge_pins.h"

// functions on board:
#include "badge_radio.h"
#include "badge_epaper.h"
#include "badge_smartled.h"
#include "badge_led.h"
#include "badge_pushbutton.h"
#include "badge_battery.h"
#include "badge_lightdark.h"
#include "badge_sao.h"

#include "badge_prefs.h"

// IDEA: hold pushbutton down for 5s -> go into deep sleep, wake up on button press
// see deep-sleep sample at https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/
// DOWNER: only D0..D3 supported for wakeup; could use a SAO GPIOpin, though.

void setup() {
  setup_serial();// so other code can produce diagnostics, or interact
  setup_prefs();
  /* odd-one-out:
     only modify EPD at powerup, and only if explicitly requested
     because EPD has limited lifetime and slow response
     so we generally treat it as a static display */
  setup_epaper_display();

  setup_battery_monitor();
  setup_lightdark_sensor();
  setup_pushbutton();

  setup_single_LED();
  setup_city_smartLEDs();

  setup_SAOs();
  setup_radio();
}

// the loop function runs over and over again forever
void loop() {
  update_prefs();

  update_battery_monitor();
  update_lightdark_sensor();
  update_pushbutton();

  update_single_LED();
  update_city_smartLEDs();

  update_SAOs();
  update_radio();
}
