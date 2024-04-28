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

/* resources:
  https://learn.adafruit.com/multi-tasking-the-arduino-part-1
 */

//syntactic sugar
typedef const int GPIOpin;
typedef const int Digital_Input;
typedef const int Analog_Input;
typedef const int Digital_Output;

typedef unsigned long Time;


#define DO_LOG 1

#ifdef DO_LOG
#define LOG Serial.print
#define LOGln Serial.println
#else
#define LOG (void)
#define LOGln (void)
#endif

#include <stdio.h>
#include <string.h>

const Time BOOST_CONVERTER_STARTUP_ms = 500;
const unsigned long SERIAL_SPEED = 115200;
void setup_serial()
{
	Serial.begin(SERIAL_SPEED);
	LOGln("badge is up");
}


/* wait a random time between 5 and 20 minutes
   to write updated preferences back to non-volatile storage */
const Time MINIMUM_s_TO_UPDATE_PREFS = 300;
const Time MAXIMUM_s_TO_UPDATE_PREFS = 1200;
#include <Preferences.h>
Preferences prefs;

const uint8_t WIFI_PASSWORD_LENGTH = 20;
char WiFi_password[WIFI_PASSWORD_LENGTH+1] = {0};

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
  conf.seed = prefs.getULong64("seed") << 1
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


// functions on board:

// ======== Shitty Add-On v1.69bis (https://hackaday.com/2019/03/20/introducing-the-shitty-add-on-v1-69bis-standard/)
#include <Wire.h> // I2C
//
GPIOpin SAO_GPIO1 = D1;
GPIOpin SAO_GPIO2 = D0;
GPIOpin SAO_SDA = SDA;
GPIOpin SAO_SCL = SCL;
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


// ======== human input - pushbutton
Digital_Input pushbutton = D9;//(Vbutton)
unsigned pushbutton_debounced = 1;
void setup_pushbutton() {
  pinMode(pushbutton, INPUT);
}
void update_pushbutton() {
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
    if (millis() >= log_delay) LOG("pushbutton:");LOGln(pushbutton_debounced? "HIGH": "LOW");
  }
}

// IDEA: hold pushbutton down for 5s -> go into deep sleep, wake up on button press
// see deep-sleep sample at https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/
// DOWNER: only D0..D3 supported for wakeup; could use a SAO GPIOpin, though.


// ======== environmental/status input - lightdark sensor
Analog_Input lightdark_sensor = A3;//(Vdark)
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
    if (millis() >= log_delay) LOG("lightdark:");LOGln(lightdark_smoothed);
  }
}


// ======== environmental/status input - battery monitor
Analog_Input half_battery_voltage = A2;//(Vmeasure)
unsigned long battery_millivolts;
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
    if (millis() >= log_delay) LOG("battery:");LOG(battery_millivolts);LOGln("mV");
  }
}


Digital_Output level_shifter_OE = D7;
#ifdef ENABLE_LED
// ======== blinky outputs - single LED
Digital_Output single_LED = D6;
void setup_single_LED() {
  pinMode(single_LED, OUTPUT);
}
void update_single_LED() {
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
#define update_single_LED() ((void)0)
#endif


#ifdef ENABLE_SMARTLEDS
// ======== blinky outputs - city smartLEDs
#include <SPI.h>
//FIXME: add ESP32C3 hardware SPI support to FastLED
#define FASTLED_USE_GLOBAL_BRIGHTNESS 1
#include <FastLED.h>
//
Digital_Output smart_LED_data = MOSI;
Digital_Output smart_LED_clock = SCK;

enum LED_city_index {
  LED_Memphis, LED_Clarkesville, LED_Nashville, LED_Chattanooga, LED_Knoxville,
  NUM_SMART_LEDS
};
CRGB leds[NUM_SMART_LEDS];
void setup_city_smartLEDs() {
  pinMode(smart_LED_data, OUTPUT);
  pinMode(smart_LED_clock, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  FastLED.addLeds<APA102, smart_LED_data, smart_LED_clock, BGR>(leds, NUM_SMART_LEDS);  // BGR ordering is typical
	FastLED.setBrightness(100);
  FastLED.setMaxPowerInMilliWatts(500);
}
struct {
  Time wait_until;
  CRGB new_LED;
  Time time_step;
  fract8 step;
} city[NUM_SMART_LEDS];
int any_LED_changed = 0;
void update_city(int i) {
  const Time LOG_DELAY_ms = 2000;
  static Time log_delay;
  // ======== CUSTOMIZE HERE ========

  /* How It Works:
     a Finite State Machine for each LED has *n* states
     each fade state waits for each step in its respective processes
   */

  if (millis() >= city[i].wait_until) {
    any_LED_changed = 1;
    city[i].wait_until = millis() + city[i].time_step;
    leds[i] = leds[i].lerp8(city[i].new_LED, city[i].step);
    if (city[i].step == 255) {
      /* set new transition for this city */

      switch (i) {
      case LED_Memphis: if (millis() >= log_delay) LOGln("Memphis"); break;
      case LED_Clarkesville: if (millis() >= log_delay) LOGln("Clarkesville"); break;
      case LED_Nashville: if (millis() >= log_delay) LOGln("Nashville"); break;
      case LED_Chattanooga: if (millis() >= log_delay) LOGln("Chattanooga"); break;
      case LED_Knoxville: if (millis() >= log_delay) LOGln("Knoxville"); break;
      default: if (millis() >= log_delay) LOGln("I have no idea");
      }
    }
    city[i].step++;
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

/*
each LED starts at time 0, value 0,0,0
a fade goes from old[r,g,b] to new[r,g,b] over time t

*/
struct LED_fade { // from current to target over time
  unsigned time; // units might be seconds, milliseconds, ...
  union {
    CRGB c;
    struct LED_fade* other;
  };
};

struct LED_state {
  unsigned ticks_to_next_change;
};

// examples // when step[n].time==0, no more steps
const LED_fade white_90pc_flash[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {9,.c=CRGB::Black}, {0}}; //cycle=100
const LED_fade white_50pc_flash[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {99,.c=CRGB::Black}, {0}}; //cycle=200
const LED_fade white_10pc_flash[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {899,.c=CRGB::Black}, {0}}; //cycle=1000
const LED_fade white_1pc_flash[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {9899,.c=CRGB::Black}, {0}}; //cycle=10000
const LED_fade white_double_strobe[] = {
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {99,.c=CRGB::Black},
  {1,.c=CRGB::White}, {99,.c=CRGB::White},
  {1,.c=CRGB::Black}, {9699,.c=CRGB::Black}, {0}}; //cycle=10000
const LED_fade rainbow[] = {
  {100,.c=CRGB::Red},
  {100,.c=CRGB::Orange},
  {100,.c=CRGB::Yellow},
  {100,.c=CRGB::Green},
  {100,.c=CRGB::Blue},
  {100,.c=CRGB::Violet}, {0}};//cycle=600
const LED_fade red_blue_sawtooth[] = {
  {1,.c=CRGB::Black}, {999,.c=CRGB::Red},
  {1,.c=CRGB::Black}, {999,.c=CRGB::Blue}, {0}};//cycle=2000
#else
#define setup_city_smartLEDs() ((void)0)
#define update_city_smartLEDs() ((void)0)
#endif


#ifdef ENABLE_EPAPER
// ======== epaper display (EPD)
//FIXME: add ESP32C3 hardware SPI support to GxEPD2
//#define USE_HSPI_FOR_EPD
#define ENABLE_GxEPD2_GFX 0
//#include <GFX.h> // uncomment to use class GFX of library GFX_Root instead of Adafruit_GFX
// Note: if you use this with ENABLE_GxEPD2_GFX 1:
//       uncomment it in GxEPD2_GFX.h too, or add #include <GFX.h> before any #include <GxEPD2_GFX.h>
#include <GxEPD2_3C.h>
//
/* Note: I/O pins are shared with other hardware, but
        EPD is powered only when other hardware is unpowered
        thanks to slide-switch SW3
 */
Digital_Output EPD_SDI = MOSI;
Digital_Output EPD_SCLK = SCK;
Digital_Output EPD_cs = D3;//(shared with Vdark)
Digital_Output EPD_data_cmd = D1;//(shared with SAO GPIO1)
Digital_Output EPD_Reset = D6;//(shared with single LED)
Digital_Input EPD_Busy = D0;//(shared with SAO GPIO2)
void update_epaper_display();
void setup_epaper_display() {
  /* After customizing your epaper display and downloading updated firmware,
     turn off the badge.
     Then slide the "enable epaper update" switch away from "blink LEDs",
     press-and-hold the pushbutton, and turn on the badge.
     Once the display has updated, release the pushbutton
     and slide the "enable epaper update" switch back to "blink LEDs".
   */
  pinMode(pushbutton, INPUT);
  /* "enable epaper update" switches pullup on EPD_Reset line to 3.3V */
  pinMode(EPD_Reset, INPUT);
  if (digitalRead(pushbutton) == 0 && digitalRead(EPD_Reset) == 1) update_epaper_display();

  /* wait for pushbutton release and epaper-disable */
  pinMode(pushbutton, INPUT);
  pinMode(EPD_Reset, INPUT);
  while (digitalRead(pushbutton) == 0 || digitalRead(EPD_Reset) == 1) /* do nothing */;
  // pushbutton has been released
  // "enable epaper update" switch is in "blink LEDs" position
  // boost converter has battery power connected to its input

  delay(BOOST_CONVERTER_STARTUP_ms);
  pinMode(level_shifter_OE, OUTPUT);
  digitalWrite(level_shifter_OE, HIGH);
}
#else
#define setup_epaper_display() ((void)0)
#endif

// ======== on-chip radio: BLE and/or WiFi
uint64_t MAC;


#ifdef ENABLE_BLUETOOTH
//284050
//284228
#include "SimpleBLE.h"
SimpleBLE ble;
void setup_simpleBLE()
{
  ble.begin("ESP32 SimpleBLE");
}
void update_simpleBLE()
{
    String out = "BLE32 name: ";
    out += String(millis() / 1000);
    Serial.println(out);
    ble.begin(out);
}
#else
#define setup_simpleBLE() ((void)0)
#define update_simpleBLE() ((void)0)
#endif


#ifdef ENABLE_BLUETOOTH
// IDEA: BLE uses so much flash it's probably not worth it
//284228
//1002926

//284050
//1002746
#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>
#include <BLEServer.h>
// https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf
#define SERVICE_UUID        "6050bcfe-a8f8-4ad6-961d-5af97ac37e09"
#define CHARACTERISTIC_UUID "cab5c6ff-31a7-4b2a-aada-4bae02ca1ca7"
void setup_Bluetooth()
{
  // ======== CUSTOMIZE Bluetooth HERE ========
  // IDEA: https://en.wikipedia.org/wiki/Bluetooth_Low_Energy_beacon


  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}
void update_Bluetooth()
{
  // ======== CUSTOMIZE Bluetooth HERE ========
}
#else
#define setup_Bluetooth() ((void)0)
#define update_Bluetooth() ((void)0)
#endif


#ifdef ENABLE_WIFI
//284050
//732912
#include <WiFi.h>
const Time WIFI_STABILITY_ms = 100;
#define SSIDprefix "TNbadge"
char SSID[(sizeof SSIDprefix)+(sizeof MAC)*2] = SSIDprefix;
const char WiFi_password_alphabet[] =
  "abcdefghijklmnoppqrstuvwxyz[];,./"
  "ABCDEFGHIJKKLMNOPQRSTUVWXYZ{}:<>?"
  "`1234567890-="
  "~!@#$%^&*()_+"
  "\\|'\"";

// IDEA: probably no need to even connect to AP, let alone server 
#if 0
WiFiServer server(80);
#endif

/* IDEA:
   find out what friends are nearby through
   - broadcasting our own SSID,
   - scanning for SSIDs,
   - filtering by known SSID prefix,
   - sorting by RSSID
 */

/* IDEA:
   use hardware crypto engine

   #include "esp_secure_cert_crypto.h"
   #include "esp_secure_cert_read.h"
   #include "rom/aes.h"
   #include "rom/digital_signature.h"
   #include "rom/hmac.h"
   #include "rom/sha.h"

   (also in the SDK ...)
   #include "sodium.h" //https://doc.libsodium.org/
 */

 /* possibly useful magic numbers for API

 may want to tweak Tx power
 .setTxPower()
    WIFI_POWER_19_5dBm
    WIFI_POWER_19dBm
    WIFI_POWER_18_5dBm
    WIFI_POWER_17dBm
    WIFI_POWER_15dBm
    WIFI_POWER_13dBm
    WIFI_POWER_11dBm
    WIFI_POWER_8_5dBm
    WIFI_POWER_7dBm
    WIFI_POWER_5dBm
    WIFI_POWER_2dBm
    WIFI_POWER_MINUS_1dBm

sadly, doesn't look like we can use other than WPA2 or OPEN on soft-AP
.setMinSecurity()
    WIFI_AUTH_WPA3_PSK
    WIFI_AUTH_WPA2_WPA3_PSK
    WIFI_AUTH_WPA2_PSK

    WIFI_AUTH_OPEN
    WIFI_AUTH_WEP
    WIFI_AUTH_WPA_PSK
    WIFI_AUTH_WPA_WPA2_PSK
    WIFI_AUTH_ENTERPRISE
    WIFI_AUTH_WPA2_ENTERPRISE
    WIFI_AUTH_WAPI_PSK
    WIFI_AUTH_WPA3_ENT_192
    WIFI_AUTH_MAX

 */
void setup_WiFi()
{
  // ======== CUSTOMIZE WiFi HERE ========
  sprintf(SSID + (sizeof SSIDprefix) - 1, "%llX", MAC);
  LOG("SSID:");LOGln(SSID);
  // generate a completely random password, if not already pulled from prefs
  if (WiFi_password[0] == 0) {
    for (int i = 0; i < WIFI_PASSWORD_LENGTH; i++) {
      WiFi_password[i] = WiFi_password_alphabet[random((sizeof WiFi_password_alphabet)-1)];
    }
    WiFi_password[(sizeof WiFi_password_alphabet)-1] =0;
  }
  LOG("WiFi password:");LOGln(WiFi_password);

  // Set to station+AP mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_AP_STA);
  WiFi.disconnect();
  delay(WIFI_STABILITY_ms);

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(SSID, WiFi_password)) {
    LOGln("Soft AP creation failed.");
    return;
  }
  IPAddress myIP = WiFi.softAPIP();
  LOG("AP IP address:");LOGln(myIP);
#if 0
  server.begin();
#endif
}
void update_WiFi()
{
  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks(/*async=*/true);
  //(, bool show_hidden = false, bool passive = false, uint32_t max_ms_per_chan = 300, uint8_t channel = 0, const char * ssid=nullptr, const uint8_t * bssid=nullptr)
  if (n == WIFI_SCAN_RUNNING || n == WIFI_SCAN_FAILED) return;

  unsigned friends = 0;
  int32_t max_signal, min_signal;
  for (int i = 0; i < n; ++i) {
    // look at SSID and RSSI for each network found
    const char* s = WiFi.SSID(i).c_str();
    if (strstr(s, SSIDprefix) != s) continue;// not one of ours

    friends++;
    int32_t r = WiFi.RSSI(i);
    if (friends == 1 || r > max_signal) max_signal = r;
    if (friends == 1 || r < min_signal) min_signal = r;

    //FIXME: do something with encryptionType; maybe disown OPEN/WEP/WPA ?
    switch (WiFi.encryptionType(i))
    {
    case WIFI_AUTH_OPEN:
        Serial.print("open");
        break;
    case WIFI_AUTH_WEP:
        Serial.print("WEP");
        break;
    case WIFI_AUTH_WPA_PSK:
        Serial.print("WPA");
        break;
    case WIFI_AUTH_WPA2_PSK:
        Serial.print("WPA2");
        break;
    case WIFI_AUTH_WPA_WPA2_PSK:
        Serial.print("WPA+WPA2");
        break;
    case WIFI_AUTH_WPA2_ENTERPRISE:
        Serial.print("WPA2-EAP");
        break;
    case WIFI_AUTH_WPA3_PSK:
        Serial.print("WPA3");
        break;
    case WIFI_AUTH_WPA2_WPA3_PSK:
        Serial.print("WPA2+WPA3");
        break;
    case WIFI_AUTH_WAPI_PSK:
        Serial.print("WAPI");
        break;
    default:
        Serial.print("unknown");
    }
  }
  //FIXME: make result available to rest of program,
  //       c.f. pushbutton_debounced, lightdark_smoothed

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();
}
#else
#define setup_WiFi() ((void)0)
#define update_WiFi() ((void)0)
#endif

void setup_radio() {
  MAC = ESP.getEfuseMac();
  setup_simpleBLE();
  setup_Bluetooth();
  setup_WiFi();
}
void update_radio() {
  update_simpleBLE();
  update_Bluetooth();
  update_WiFi();
}


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


#ifdef ENABLE_SMARTLEDS
//======== FIXME beyond here ========
void fadeall() { for(int i = 0; i < NUM_SMART_LEDS; i++) { leds[i].nscale8(250); } }

void loop2() { 
	static uint8_t hue = 0;
	Serial.print("x");
	// First slide the led in one direction
	for(int i = 0; i < NUM_SMART_LEDS; i++) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}
	Serial.print("x");

	// Now go in the other direction.  
	for(int i = (NUM_SMART_LEDS)-1; i >= 0; i--) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		delay(10);
	}
}
#endif


#ifdef ENABLE_EPAPER
// NOTE for use with Waveshare ESP32 Driver Board:
// **** also need to select the constructor with the parameters for this board in GxEPD2_display_selection_new_style.h ****
//
// The Wavehare ESP32 Driver Board uses uncommon SPI pins for the FPC connector. It uses HSPI pins, but SCK and MOSI are swapped.
// To use HW SPI with the ESP32 Driver Board, HW SPI pins need be re-mapped in any case. Can be done using either HSPI or VSPI.
// Other SPI clients can either be connected to the same SPI bus as the e-paper, or to the other HW SPI bus, or through SW SPI.
// The logical configuration would be to use the e-paper connection on HSPI with re-mapped pins, and use VSPI for other SPI clients.
// VSPI with standard VSPI pins is used by the global SPI instance of the Arduino IDE ESP32 package.

#define MAX_DISPLAY_BUFFER_SIZE 131072ul // e.g. half of available ram
#define MAX_HEIGHT_3C(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
//#define USE_HSPI_FOR_EPD
// 3-color e-papers
GxEPD2_3C<GxEPD2_213c,     MAX_HEIGHT_3C(GxEPD2_213c)>     display(GxEPD2_213c(EPD_cs, EPD_data_cmd, EPD_Reset, EPD_Busy));     // GDEW0213Z16 104x212, UC8151 (IL0373)
//GxEPD2_3C<GxEPD2_213_Z19c, MAX_HEIGHT_3C(GxEPD2_213_Z19c)> display(GxEPD2_213_Z19c(EPD_cs, EPD_data_cmd, EPD_Reset, EPD_Busy)); // GDEH0213Z19 104x212, UC8151D

#include <pgmspace.h>
const unsigned char bitmap_black[2762] PROGMEM = {
  // size 2762 came from sample code; actual bitmap has only 2756 bytes
  // I'm not sure why the difference
  #include "black.h"
};
const unsigned char bitmap_red[2762] PROGMEM = {
  #include "red.h"
};



#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
SPIClass hspi(HSPI);
#endif

void update_epaper_display()
{
  pinMode(EPD_SDI, OUTPUT);
  pinMode(EPD_SCLK, OUTPUT);
  pinMode(EPD_cs, OUTPUT);
  pinMode(EPD_data_cmd, OUTPUT);
  pinMode(EPD_Reset, OUTPUT);
  pinMode(EPD_Busy, INPUT);

  SPI.begin();
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  // FIXME

#if defined(ESP32) && defined(USE_HSPI_FOR_EPD)
  hspi.begin(13, 12, 14, 15); // remap hspi for EPD (swap pins)
  display.epd2.selectSPI(hspi, SPISettings(4000000, MSBFIRST, SPI_MODE0));
#elif (defined(ARDUINO_ARCH_ESP32) && defined(ARDUINO_LOLIN_S2_MINI))
  // SPI.begin(sck, miso, mosi, ss); // preset for remapped pins
  SPI.begin(18, -1, 16, 33); // my LOLIN ESP32 S2 mini connection
#endif
  //display.init(115200); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  //display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  //display.init(115200, true, 10, false, SPIn, SPISettings(4000000, MSBFIRST, SPI_MODE0)); // extended init method with SPI channel and/or settings selection
  // first update should be full refresh

  // most e-papers have width < height (portrait) as native orientation, especially the small ones
  // in GxEPD2 rotation 0 is used for native orientation (most TFT libraries use 0 fix for portrait orientation)
  // set rotation to 1 (rotate right 90 degrees) to have enough space on small displays (landscape)
  display.setRotation(1);
  display.setFullWindow();

  display.firstPage();
  display.fillScreen(GxEPD_WHITE);
  display.drawInvertedBitmap(0, 0, bitmap_black, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_BLACK);
  display.drawBitmap(0, 0, bitmap_red, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_RED);
  display.nextPage();

  display.powerOff();
  display.end();
}
#else
#define update_epaper_display() ((void)0)
#endif

