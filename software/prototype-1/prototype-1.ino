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
#include <Arduino.h>
#include <Preferences.h>


//syntactic sugar
typedef const int GPIOpin;
typedef const int Digital_Input;
typedef const int Analog_Input;
typedef const int Digital_Output;

typedef unsigned long Time;

const Time BOOST_CONVERTER_STARTUP_ms = 500;
const unsigned long SERIAL_SPEED = 57600;

#include "XIAO-ESP32C3-pinout.h"
// functions on board:

// ======== Shitty Add-On v1.69bis (https://hackaday.com/2019/03/20/introducing-the-shitty-add-on-v1-69bis-standard/)
#include <Wire.h> // I2C
//
GPIOpin SAO_GPIO1 = pD1;
GPIOpin SAO_GPIO2 = pD0;
GPIOpin SAO_SDA = pSDA;
GPIOpin SAO_SCL = pSCL;
void setup_SAOs()
{
  // ======== CUSTOMIZE HERE ========
  pinMode(SAO_GPIO1, INPUT);
  pinMode(SAO_GPIO2, INPUT);
  pinMode(SAO_SDA, OUTPUT);
  pinMode(SAO_SCL, OUTPUT);
}
void update_SAOs()
{
  // ======== CUSTOMIZE HERE ========
}

// ======== human input - pushbutton
Digital_Input pushbutton = pD9;//(Vbutton)
unsigned pushbutton_debounced = 1;
void setup_pushbutton() {
  pinMode(pushbutton, INPUT);
}
void update_pushbutton() {
  const Time PUSHBUTTON_DEBOUNCE_ms = 20;
  static Time debounce_until;
  static unsigned prev = 0;

  /* How It Works:
     after pushbutton has been in same position for at least `PUSHBUTTON_DEBOUNCE_ms`
     set `pushbutton_debounced` to that value
   */
  if (digitalRead(pushbutton) != prev) {
    prev = !prev;
    debounce_until = millis() + PUSHBUTTON_DEBOUNCE_ms;
  }
  else if (millis() >= debounce_until) {
    pushbutton_debounced = prev;
  }
}

// IDEA: hold pushbutton down for 5s -> go into deep sleep, wake up on button press
// see deep-sleep sample at https://wiki.seeedstudio.com/XIAO_ESP32C3_Getting_Started/
// DOWNER: only D0..D3 supported for wakeup; could use a SAO GPIOpin, though.

// ======== environmental/status input - lightdark sensor
Analog_Input lightdark_sensor = pA3;//(Vdark)
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
  }
}

// ======== environmental/status input - battery monitor
Analog_Input half_battery_voltage = pA2;//(Vmeasure)
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
  }
}

// ======== blinky outputs - single LED
Digital_Output level_shifter_OE = pD7;
Digital_Output single_LED = pD6;
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

// ======== blinky outputs - city smartLEDs
#include <SPI.h>
//FIXME: add ESP32C3 hardware SPI support to FastLED
#include <FastLED.h>
//
enum LED_city_indices {
  LED_Memphis, LED_Clarkesville, LED_Nashville, LED_Chattanooga, LED_Knoxville,
  NUM_SMART_LEDS
};
Digital_Output smart_LED_data = pMOSI;
Digital_Output smart_LED_clock = pSCK;
CRGB leds[NUM_SMART_LEDS];
void setup_city_smartLEDs() {
  pinMode(smart_LED_data, OUTPUT);
  pinMode(smart_LED_clock, OUTPUT);
  FastLED.addLeds<APA102, smart_LED_data, smart_LED_clock, BGR>(leds, NUM_SMART_LEDS);  // BGR ordering is typical
	FastLED.setBrightness(25);
}
void update_city_smartLEDs() {
  // ======== CUSTOMIZE HERE ========

  /* How It Works:
     a Finite State Machine for each LED has *n* states
     each fade state waits for each step in its respective processes
   */

}


/*
each LED starts at time 0, value 0,0,0
a fade goes from old[r,g,b] to new[r,g,b] over time t

*/
struct LED_fade { // from current to target over time
  unsigned time; // units might be seconds, milliseconds, ...
  CRGB target;
};

struct LED_state {
  unsigned ticks_to_next_change;
};

// examples // when step[n].time==0, no more steps
const LED_fade white_90pc_flash[] = {
  {1,CRGB::White}, {99,CRGB::White},
  {1,CRGB::Black}, {9,CRGB::Black}, {0}}; //cycle=100
const LED_fade white_50pc_flash[] = {
  {1,CRGB::White}, {99,CRGB::White},
  {1,CRGB::Black}, {99,CRGB::Black}, {0}}; //cycle=200
const LED_fade white_10pc_flash[] = {
  {1,CRGB::White}, {99,CRGB::White},
  {1,CRGB::Black}, {899,CRGB::Black}, {0}}; //cycle=1000
const LED_fade white_1pc_flash[] = {
  {1,CRGB::White}, {99,CRGB::White},
  {1,CRGB::Black}, {9899,CRGB::Black}, {0}}; //cycle=10000
const LED_fade white_double_strobe[] = {
  {1,CRGB::White}, {99,CRGB::White},
  {1,CRGB::Black}, {99,CRGB::Black},
  {1,CRGB::White}, {99,CRGB::White},
  {1,CRGB::Black}, {9699,CRGB::Black}, {0}}; //cycle=10000
const LED_fade rainbow[] = {
  {100,CRGB::Red},
  {100,CRGB::Orange},
  {100,CRGB::Yellow},
  {100,CRGB::Green},
  {100,CRGB::Blue},
  {100,CRGB::Violet}, {0}};//cycle=600
const LED_fade red_blue_sawtooth[] = {
  {1,CRGB::Black}, {999,CRGB::Red},
  {1,CRGB::Black}, {999,CRGB::Blue}, {0}};//cycle=2000

// ======== epaper display (EPD)
//FIXME: add ESP32C3 hardware SPI support to GxEPD2
#define USE_HSPI_FOR_EPD
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
Digital_Output EPD_SDI = pMOSI;
Digital_Output EPD_SCLK = pSCK;
Digital_Output EPD_cs = pD3;//(shared with Vdark)
Digital_Output EPD_data_cmd = pD1;//(shared with SAO GPIO1)
Digital_Output EPD_Reset = pD6;//(shared with single LED)
Digital_Input EPD_Busy = pD0;//(shared with SAO GPIO2)
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
  pinMode(smart_LED_data, OUTPUT);
  pinMode(smart_LED_clock, OUTPUT);
  FastLED.addLeds<APA102, smart_LED_data, smart_LED_clock, BGR>(leds, NUM_SMART_LEDS);  // BGR orde
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

// ======== on-chip radio: BLE and/or WiFi
#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>
#include <BLEServer.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <WiFiMulti.h>
//
void setup_radio() {
  // ======== CUSTOMIZE HERE ========
}
void update_radio() {
  // ======== CUSTOMIZE HERE ========
}


void setup_serial()
{
	Serial.begin(SERIAL_SPEED);
	Serial.println("badge is up");
}


void setup() {
  setup_epaper_display();

  setup_serial();

  setup_battery_monitor();
  setup_lightdark_sensor();
  setup_pushbutton();

  setup_single_LED();
  setup_city_smartLEDs();

  setup_SAOs();
}

// the loop function runs over and over again forever
void loop() {
  update_battery_monitor();
  update_lightdark_sensor();
  update_pushbutton();

  update_single_LED();
  update_city_smartLEDs();

  update_SAOs();
}



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
		FastLED.show(); 
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
//GxEPD2_3C<GxEPD2_213_Z98c, MAX_HEIGHT_3C(GxEPD2_213_Z98c)> display(GxEPD2_213_Z98c(EPD_cs, EPD_data_cmd, EPD_Reset, EPD_Busy)); // GDEY0213Z98 122x250, SSD1680

// 3-color
#include "bitmaps/Bitmaps3c104x212.h" // 2.13" b/w/r
//#include "bitmaps/Bitmaps3c128x250.h" // 2.13" b/w/r


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

  // FIXME

  Serial.println();
  Serial.println("setup");
  delay(100);

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
  if (display.pages() > 1)
  {
    delay(100);
    Serial.print("pages = "); Serial.print(display.pages()); Serial.print(" page height = "); Serial.println(display.pageHeight());
    delay(1000);
  }
  // first update should be full refresh
  helloWorld();
  delay(1000);
  // partial refresh mode can be used to full screen,
  // effective if display panel hasFastPartialUpdate
  helloFullScreenPartialMode();
  delay(1000);
  //stripeTest(); return; // GDEH029Z13 issue
  helloArduino();
  delay(1000);
  helloEpaper();
  delay(1000);
  //helloValue(123.9, 1);
  //delay(1000);
  //drawGrid(); return;
  drawBitmaps();
  drawGraphics();
  //return;
  //drawCornerTest();
  //showBox(16, 16, 48, 32, false);
  //showBox(16, 56, 48, 32, true);
  display.powerOff();
  deepSleepTest();
  Serial.println("setup done");
  display.end();
}


// note for partial update window and setPartialWindow() method:
// partial update window size and position is on byte boundary in physical x direction
// the size is increased in setPartialWindow() if x or w are not multiple of 8 for even rotation, y or h for odd rotation
// see also comment in GxEPD2_BW.h, GxEPD2_3C.h or GxEPD2_GFX.h for method setPartialWindow()

const char HelloWorld[] = "Hello World!";
const char HelloArduino[] = "Hello Arduino!";
const char HelloEpaper[] = "Hello E-Paper!";

void helloWorld()
{
  //Serial.println("helloWorld");
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  // center bounding box by transposition of origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(HelloWorld);
  }
  while (display.nextPage());
  //Serial.println("helloWorld done");
}

void helloWorldForDummies()
{
  //Serial.println("helloWorld");
  const char text[] = "Hello World!";
  // most e-papers have width < height (portrait) as native orientation, especially the small ones
  // in GxEPD2 rotation 0 is used for native orientation (most TFT libraries use 0 fix for portrait orientation)
  // set rotation to 1 (rotate right 90 degrees) to have enough space on small displays (landscape)
  display.setRotation(1);
  // on e-papers black on white is more pleasant to read
  display.setTextColor(GxEPD_BLACK);
  // Adafruit_GFX has a handy method getTextBounds() to determine the boundary box for a text for the actual font
  int16_t tbx, tby; uint16_t tbw, tbh; // boundary box window
  display.getTextBounds(text, 0, 0, &tbx, &tby, &tbw, &tbh); // it works for origin 0, 0, fortunately (negative tby!)
  // center bounding box by transposition of origin:
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  // full window mode is the initial mode, set it anyway
  display.setFullWindow();
  // here we use paged drawing, even if the processor has enough RAM for full buffer
  // so this can be used with any supported processor board.
  // the cost in code overhead and execution time penalty is marginal
  // tell the graphics class to use paged drawing mode
  display.firstPage();
  do
  {
    // this part of code is executed multiple times, as many as needed,
    // in case of full buffer it is executed once
    // IMPORTANT: each iteration needs to draw the same, to avoid strange effects
    // use a copy of values that might change, don't read e.g. from analog or pins in the loop!
    display.fillScreen(GxEPD_WHITE); // set the background to white (fill the buffer with value for white)
    display.setCursor(x, y); // set the postition to start printing text
    display.print(text); // print some text
    // end of part executed multiple times
  }
  // tell the graphics class to transfer the buffer content (page) to the controller buffer
  // the graphics class will command the controller to refresh to the screen when the last page has been transferred
  // returns true if more pages need be drawn and transferred
  // returns false if the last page has been transferred and the screen refreshed for panels without fast partial update
  // returns false for panels with fast partial update when the controller buffer has been written once more, to make the differential buffers equal
  // (for full buffered with fast partial update the (full) buffer is just transferred again, and false returned)
  while (display.nextPage());
  //Serial.println("helloWorld done");
}

void helloFullScreenPartialMode()
{
  //Serial.println("helloFullScreenPartialMode");
  const char fullscreen[] = "full screen update";
  const char fpm[] = "fast partial mode";
  const char spm[] = "slow partial mode";
  const char npm[] = "no partial mode";
  display.setPartialWindow(0, 0, display.width(), display.height());
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
  const char* updatemode;
  if (display.epd2.hasFastPartialUpdate)
  {
    updatemode = fpm;
  }
  else if (display.epd2.hasPartialUpdate)
  {
    updatemode = spm;
  }
  else
  {
    updatemode = npm;
  }
  // do this outside of the loop
  int16_t tbx, tby; uint16_t tbw, tbh;
  // center update text
  display.getTextBounds(fullscreen, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t utx = ((display.width() - tbw) / 2) - tbx;
  uint16_t uty = ((display.height() / 4) - tbh / 2) - tby;
  // center update mode
  display.getTextBounds(updatemode, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t umx = ((display.width() - tbw) / 2) - tbx;
  uint16_t umy = ((display.height() * 3 / 4) - tbh / 2) - tby;
  // center HelloWorld
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t hwx = ((display.width() - tbw) / 2) - tbx;
  uint16_t hwy = ((display.height() - tbh) / 2) - tby;
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(hwx, hwy);
    display.print(HelloWorld);
    display.setCursor(utx, uty);
    display.print(fullscreen);
    display.setCursor(umx, umy);
    display.print(updatemode);
  }
  while (display.nextPage());
  //Serial.println("helloFullScreenPartialMode done");
}

void helloArduino()
{
  //Serial.println("helloArduino");
  display.setRotation(1);
  display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  // align with centered HelloWorld
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  // height might be different
  display.getTextBounds(HelloArduino, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t y = ((display.height() / 4) - tbh / 2) - tby; // y is base line!
  // make the window big enough to cover (overwrite) descenders of previous text
  do
  {
    display.fillScreen(GxEPD_WHITE);
    //display.drawRect(x, y - tbh, tbw, tbh, GxEPD_BLACK);
    display.setCursor(x, y);
    display.print(HelloArduino);
  }
  while (display.nextPage());
  delay(1000);
  //Serial.println("helloArduino done");
}

void helloEpaper()
{
  //Serial.println("helloEpaper");
  display.setRotation(1);
  display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  // align with centered HelloWorld
  display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  // height might be different
  display.getTextBounds(HelloEpaper, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t y = ((display.height() * 3 / 4) - tbh / 2) - tby; // y is base line!
  // make the window big enough to cover (overwrite) descenders of previous text
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(HelloEpaper);
  }
  while (display.nextPage());
  //Serial.println("helloEpaper done");
}

// test partial window issue on GDEW0213Z19 and GDEH029Z13
void stripeTest()
{
  helloStripe(104);
  delay(2000);
  helloStripe(96);
}

const char HelloStripe[] = "Hello Stripe!";

void helloStripe(uint16_t pw_xe) // end of partial window in physcal x direction
{
  //Serial.print("HelloStripe("); Serial.print(pw_xe); Serial.println(")");
  display.setRotation(3);
  display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(HelloStripe, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  do
  {
    display.fillScreen(GxEPD_WHITE);
  }
  while (display.nextPage());
  //Serial.println("HelloStripe done");
}

#if defined(ESP8266) || defined(ESP32)
#include <StreamString.h>
#define PrintString StreamString
#else
class PrintString : public Print, public String
{
  public:
    size_t write(uint8_t data) override
    {
      return concat(char(data));
    };
};
#endif

void helloValue(double v, int digits)
{
  //Serial.println("helloValue");
  display.setRotation(1);
  display.setTextColor(display.epd2.hasColor ? GxEPD_RED : GxEPD_BLACK);
  PrintString valueString;
  valueString.print(v, digits);
  int16_t tbx, tby; uint16_t tbw, tbh;
  display.getTextBounds(valueString, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() * 3 / 4) - tbh / 2) - tby; // y is base line!
  // show what happens, if we use the bounding box for partial window
  uint16_t wx = (display.width() - tbw) / 2;
  uint16_t wy = ((display.height() * 3 / 4) - tbh / 2);
  display.setPartialWindow(wx, wy, tbw, tbh);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(valueString);
  }
  while (display.nextPage());
  delay(2000);
  // make the partial window big enough to cover the previous text
  uint16_t ww = tbw; // remember window width
  display.getTextBounds(HelloEpaper, 0, 0, &tbx, &tby, &tbw, &tbh);
  // adjust, because HelloEpaper was aligned, not centered (could calculate this to be precise)
  ww = max(ww, uint16_t(tbw + 12)); // 12 seems ok
  wx = (display.width() - tbw) / 2;
  // make the window big enough to cover (overwrite) descenders of previous text
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(valueString);
  }
  while (display.nextPage());
  //Serial.println("helloValue done");
}

void deepSleepTest()
{
  //Serial.println("deepSleepTest");
  const char hibernating[] = "hibernating ...";
  const char wokeup[] = "woke up";
  const char from[] = "from deep sleep";
  const char again[] = "again";
  display.setRotation(1);
  display.setTextColor(GxEPD_BLACK);
  int16_t tbx, tby; uint16_t tbw, tbh;
  // center text
  display.getTextBounds(hibernating, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t x = ((display.width() - tbw) / 2) - tbx;
  uint16_t y = ((display.height() - tbh) / 2) - tby;
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(x, y);
    display.print(hibernating);
  }
  while (display.nextPage());
  display.hibernate();
  delay(5000);
  display.getTextBounds(wokeup, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t wx = (display.width() - tbw) / 2;
  uint16_t wy = ((display.height() / 3) - tbh / 2) - tby; // y is base line!
  display.getTextBounds(from, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t fx = (display.width() - tbw) / 2;
  uint16_t fy = ((display.height() * 2 / 3) - tbh / 2) - tby; // y is base line!
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(wx, wy);
    display.print(wokeup);
    display.setCursor(fx, fy);
    display.print(from);
  }
  while (display.nextPage());
  delay(5000);
  display.getTextBounds(hibernating, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t hx = (display.width() - tbw) / 2;
  uint16_t hy = ((display.height() / 3) - tbh / 2) - tby; // y is base line!
  display.getTextBounds(again, 0, 0, &tbx, &tby, &tbw, &tbh);
  uint16_t ax = (display.width() - tbw) / 2;
  uint16_t ay = ((display.height() * 2 / 3) - tbh / 2) - tby; // y is base line!
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(hx, hy);
    display.print(hibernating);
    display.setCursor(ax, ay);
    display.print(again);
  }
  while (display.nextPage());
  display.hibernate();
  //Serial.println("deepSleepTest done");
}

void showBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool partial)
{
  //Serial.println("showBox");
  display.setRotation(1);
  if (partial)
  {
    display.setPartialWindow(x, y, w, h);
  }
  else
  {
    display.setFullWindow();
  }
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(x, y, w, h, GxEPD_BLACK);
  }
  while (display.nextPage());
  //Serial.println("showBox done");
}

void drawCornerTest()
{
  display.setFullWindow();
  display.setTextColor(GxEPD_BLACK);
  for (uint16_t r = 0; r <= 4; r++)
  {
    display.setRotation(r);
    display.firstPage();
    do
    {
      display.fillScreen(GxEPD_WHITE);
      display.fillRect(0, 0, 8, 8, GxEPD_BLACK);
      display.fillRect(display.width() - 18, 0, 16, 16, GxEPD_BLACK);
      display.fillRect(display.width() - 25, display.height() - 25, 24, 24, GxEPD_BLACK);
      display.fillRect(0, display.height() - 33, 32, 32, GxEPD_BLACK);
      display.setCursor(display.width() / 2, display.height() / 2);
      display.print(display.getRotation());
    }
    while (display.nextPage());
    delay(2000);
  }
}

void drawGrid()
{
  uint16_t x, y;
  display.firstPage();
  do
  {
    x = 0;
    do
    {
      display.drawLine(x, 0, x, display.height() - 1, GxEPD_BLACK);
      x += 10;
    }
    while (x < display.width());
    y = 0;
    do
    {
      display.drawLine(0, y, display.width() - 1, y, GxEPD_BLACK);
      y += 10;
    }
    while (y < display.height());
    x = 0;
    do
    {
      display.fillCircle(x, display.height() / 2, 3, GxEPD_BLACK);
      x += 50;
    }
    while (x <= display.width());
    y = 0;
    do
    {
      display.fillCircle(display.width() / 2, y, 3, GxEPD_BLACK);
      y += 50;
    }
    while (y <= display.height());
  }
  while (display.nextPage());
}

void drawBitmaps()
{
  display.setRotation(0);
  display.setFullWindow();
#ifdef _GxBitmaps104x212_H_
  drawBitmaps104x212();
#endif
  // 3-color
#ifdef _GxBitmaps3c104x212_H_
  drawBitmaps3c104x212();
#endif
}

#ifdef _GxBitmaps104x212_H_
void drawBitmaps104x212()
{
  const unsigned char* bitmaps[] =
  {
    WS_Bitmap104x212, Bitmap104x212_1, Bitmap104x212_2, Bitmap104x212_3
  };
  if ((display.epd2.WIDTH == 104) && (display.epd2.HEIGHT == 212) && !display.epd2.hasColor)
  {
    for (uint16_t i = 0; i < sizeof(bitmaps) / sizeof(char*); i++)
    {
      display.firstPage();
      do
      {
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0, 0, bitmaps[i], 104, 212, GxEPD_BLACK);
      }
      while (display.nextPage());
      delay(2000);
    }
  }
}
#endif

struct bitmap_pair
{
  const unsigned char* black;
  const unsigned char* red;
};

#ifdef _GxBitmaps3c104x212_H_
void drawBitmaps3c104x212()
{
  bitmap_pair bitmap_pairs[] =
  {
    {Bitmap3c104x212_1_black, Bitmap3c104x212_1_red},
    {Bitmap3c104x212_2_black, Bitmap3c104x212_2_red},
    {WS_Bitmap3c104x212_black, WS_Bitmap3c104x212_red}
  };
  if (display.epd2.panel == GxEPD2::GDEW0213Z16)
  {
    for (uint16_t i = 0; i < sizeof(bitmap_pairs) / sizeof(bitmap_pair); i++)
    {
      display.firstPage();
      do
      {
        display.fillScreen(GxEPD_WHITE);
        display.drawInvertedBitmap(0, 0, bitmap_pairs[i].black, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_BLACK);
        if (bitmap_pairs[i].red == WS_Bitmap3c104x212_red)
        {
          display.drawInvertedBitmap(0, 0, bitmap_pairs[i].red, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_RED);
        }
        else display.drawBitmap(0, 0, bitmap_pairs[i].red, display.epd2.WIDTH, display.epd2.HEIGHT, GxEPD_RED);
      }
      while (display.nextPage());
      delay(2000);
    }
  }
}
#endif

void drawGraphics()
{
  display.setRotation(0);
  display.firstPage();
  do
  {
    display.drawRect(display.width() / 8, display.height() / 8, display.width() * 3 / 4, display.height() * 3 / 4, GxEPD_BLACK);
    display.drawLine(display.width() / 8, display.height() / 8, display.width() * 7 / 8, display.height() * 7 / 8, GxEPD_BLACK);
    display.drawLine(display.width() / 8, display.height() * 7 / 8, display.width() * 7 / 8, display.height() / 8, GxEPD_BLACK);
    display.drawCircle(display.width() / 2, display.height() / 2, display.height() / 4, GxEPD_BLACK);
    display.drawPixel(display.width() / 4, display.height() / 2 , GxEPD_BLACK);
    display.drawPixel(display.width() * 3 / 4, display.height() / 2 , GxEPD_BLACK);
  }
  while (display.nextPage());
}
