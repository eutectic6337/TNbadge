#ifndef BADGE_LIGHTDARK_H_
#define BADGE_LIGHTDARK_H_

// ======== environmental/status input - lightdark sensor
unsigned lightdark_smoothed;
void setup_lightdark_sensor() {
  pinMode(lightdark_sensor, INPUT);
}
void update_lightdark_sensor() {
  const Time LIGHTDARK_READING_ms = 20;
  const unsigned LIGHTDARK_SMOOTH = 16;
  static Time wait_for_read;

  static unsigned raw[LIGHTDARK_SMOOTH] = {0};
  static unsigned long sum = 0;
  static unsigned samples = 0;

  const Time LOG_DELAY_ms = 1234;
  static Time log_delay;

  /* How It Works:
     wait at least `LIGHTDARK_READING_ms`
     take a fresh reading (push the rest along, with the oldest falling off the end)
     update `lightdark_smoothed` with the new rolling average
   */
  if (millis() >= wait_for_read) {
    sum -= raw[LIGHTDARK_SMOOTH-1];
    if (samples < LIGHTDARK_SMOOTH) {
      samples++;
    }
    for (int i = 1; i < samples; i++) {
      raw[i] = raw[i-1];
    }
    raw[0] = analogRead(lightdark_sensor);
    sum += raw[0];
    lightdark_smoothed = sum/samples;
    if (millis() >= log_delay) {
      LOG("lightdark:");LOGln(lightdark_smoothed);
      log_delay = millis() + LOG_DELAY_ms;
    }
  }
}

#endif//BADGE_LIGHTDARK_H_
