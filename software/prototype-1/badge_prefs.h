#ifndef BADGE_PREFS_H_
#define BADGE_PREFS_H_

/* wait a random time between 5 and 20 minutes
   to write updated preferences back to non-volatile storage */
const Time MINIMUM_s_TO_UPDATE_PREFS = 300;
const Time MAXIMUM_s_TO_UPDATE_PREFS = 1200;
#include <Preferences.h>
Preferences prefs;

struct {
  uint64_t seed;// for random number generator
  char* pw = WiFi_password;
  Time next;
} conf;
#include "rom/rtc.h"
#include "esp_random.h"
void setup_prefs()
{
  prefs.begin("badge", /*readonly=*/true);
  conf.seed = (prefs.getULong64("seed") << 1)
    + 1
    + rtc_get_reset_reason(0);
  prefs.getString("pw", conf.pw, WIFI_PASSWORD_LENGTH);
  prefs.end();

  conf.seed += esp_random();
  randomSeed(conf.seed);
  uint32_t t = random(MINIMUM_s_TO_UPDATE_PREFS, MAXIMUM_s_TO_UPDATE_PREFS);
  conf.next = millis()/1000 + t;
  LOG("seed:");LOGln(conf.seed);
  LOG("seconds to first prefs write:");LOGln(t);
}
void update_prefs()
{
  if (millis()/1000 >= conf.next) {
    prefs.begin("badge", /*readonly=*/false);
    conf.seed += micros();
    prefs.putULong64("seed", conf.seed);
    prefs.putString("pw", conf.pw);
    prefs.end();

    uint32_t t = random(MINIMUM_s_TO_UPDATE_PREFS, MAXIMUM_s_TO_UPDATE_PREFS);
    conf.next = millis()/1000 + t;
    LOG("seconds to next prefs write:");LOGln(t);
  }
}

#endif//BADGE_PREFS_H_
