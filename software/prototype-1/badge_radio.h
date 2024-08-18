#ifndef BADGE_RADIO_H_
#define BADGE_RADIO_H_

// ======== on-chip radio: WiFi and/or BLE
uint64_t MAC;
#include "badge_wifi.h"
#include "badge_bluetooth.h"

void setup_radio() {
  MAC = ESP.getEfuseMac();
  setup_simpleBLE();
  setup_Bluetooth();
  setup_WiFi();
}
void loop_radio() {
  loop_simpleBLE();
  loop_Bluetooth();
  loop_WiFi();
}

#endif//BADGE_RADIO_H_
