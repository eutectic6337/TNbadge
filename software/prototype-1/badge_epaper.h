#ifndef BADGE_EPAPER_H_
#define BADGE_EPAPER_H_

#ifdef ENABLE_EPAPER
// ======== epaper display (EPD)
//FIXME: add ESP32C3 hardware SPI support to GxEPD2
//#define USE_HSPI_FOR_EPD
#define ENABLE_GxEPD2_GFX 0
//#include <GFX.h> // uncomment to use class GFX of library GFX_Root instead of Adafruit_GFX
// Note: if you use this with ENABLE_GxEPD2_GFX 1:
//       uncomment it in GxEPD2_GFX.h too, or add #include <GFX.h> before any #include <GxEPD2_GFX.h>
#include <GxEPD2_3C.h>

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

// see https://www.reddit.com/r/arduino/comments/zllu7f/eink_weather_monitor/j06ckx6/
// see https://github.com/flamerten/EinkWeatherMonitor#eink-screen
// see https://github.com/MHEtLive/MH-ET-LIVE-E-Papers

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

#endif//BADGE_EPAPER_H_
