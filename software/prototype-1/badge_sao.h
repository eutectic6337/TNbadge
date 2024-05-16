#ifndef BADGE_SAO_H_
#define BADGE_SAO_H_

// ======== Shitty Add-On v1.69bis (https://hackaday.com/2019/03/20/introducing-the-shitty-add-on-v1-69bis-standard/)
#include <Wire.h> // I2C

void setup_SAOs()
{
  // ======== CUSTOMIZE HERE ========old
  pinMode(SAO_GPIO1, INPUT);
  pinMode(SAO_GPIO2, INPUT);
  pinMode(SAO_SDA, OUTPUT);
  pinMode(SAO_SCL, OUTPUT);
  Wire.begin();
}
void update_SAOs()
{
  // ======== CUSTOMIZE HERE ========
}

#endif//BADGE_SAO_H_
