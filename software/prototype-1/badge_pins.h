#ifndef BADGE_PINS_H_
#define BADGE_PINS_H_

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

Digital_Output smart_LED_data = MOSI;
Digital_Output smart_LED_clock = SCK;

Digital_Output level_shifter_OE = D7;
Digital_Output single_LED = D6;

Digital_Input pushbutton = D9;//(Vbutton)

Analog_Input half_battery_voltage = A2;//(Vmeasure)

// E (1749) ADC: adc2_get_raw(752): adc unit not supported
// E (1754) ADC: ADC2 is no longer supported, please use ADC1. Search for errata on espressif website for more details. You can enable ADC_ONESHOT_FORCE_USE_ADC2_ON_C3 to force use ADC2
Analog_Input lightdark_sensor = D3;//(Vdark)

GPIOpin SAO_GPIO1 = D1;
GPIOpin SAO_GPIO2 = D0;
GPIOpin SAO_SDA = SDA;
GPIOpin SAO_SCL = SCL;

#endif//BADGE_PINS_H_
