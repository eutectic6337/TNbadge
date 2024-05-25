#ifndef BADGE_BATTERY_H_
#define BADGE_BATTERY_H_

// ======== environmental/status input - battery monitor
unsigned long battery_millivolts;
#ifdef ENABLE_BATTERYMONITOR
void setup_battery_monitor() {
  pinMode(half_battery_voltage, INPUT);
}
void update_battery_monitor() {
  const Time BATTERY_READING_ms = 50;
  const unsigned BATTERY_SMOOTH = 8;
  static Time wait_for_read;

  static unsigned raw[BATTERY_SMOOTH] = {0};
  static unsigned long sum = 0;
  static unsigned samples = 0;

  const Time LOG_DELAY_ms = 3210;
  static Time log_delay;

  /* How It Works:
     wait at least `BATTERY_READING_ms`
     take a fresh reading (push the rest along, with the oldest falling off the end)
     update `battery_millivolts` with the new rolling average
   */
  if (millis() >= wait_for_read) {
    sum -= raw[BATTERY_SMOOTH-1];
    if (samples < BATTERY_SMOOTH) {
      samples++;
    }
    for (int i = 1; i < samples; i++) {
      raw[i] = raw[i-1];
    }
    raw[0] = analogReadMilliVolts(half_battery_voltage);
    sum += raw[0];
    battery_millivolts = sum/samples *2;
    if (millis() >= log_delay) {
      LOG("battery:");LOG(battery_millivolts);LOGln("mV");
      log_delay = millis() + LOG_DELAY_ms;
    }
  }
}
#else
#define setup_battery_monitor() ((void)0)
#define update_battery_monitor() ((void)0)
#endif

#endif//BADGE_BATTERY_H_
