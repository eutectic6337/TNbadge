#ifndef BADGE_PUSHBUTTON_H_
#define BADGE_PUSHBUTTON_H_

// ======== human input - pushbutton
unsigned pushbutton_debounced = 1;
void setup_pushbutton() {
  pinMode(pushbutton, INPUT);
}
void loop_pushbutton() {
  const Time PUSHBUTTON_DEBOUNCE_ms = 20;
  static Time debounce_until;
  static unsigned prev = 0;

  const Time LOG_DELAY_ms = 2000;
  static Time log_delay;

  /* How It Works:
     after pushbutton has been in same position for at least `PUSHBUTTON_DEBOUNCE_ms`
     set `pushbutton_debounced` to that value
   */
  if (digitalRead(pushbutton) != prev) {
    prev = !prev;
    debounce_until = millis() + PUSHBUTTON_DEBOUNCE_ms;
    log_delay = millis() + LOG_DELAY_ms;
  }
  else if (millis() >= debounce_until) {
    pushbutton_debounced = prev;
    if (millis() >= log_delay) {
      LOG("pushbutton:");LOGln(pushbutton_debounced? "HIGH": "LOW");
      log_delay = millis() + LOG_DELAY_ms;
    }
  }
}

#endif//BADGE_PUSHBUTTON_H_
