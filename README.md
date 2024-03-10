# TNbadge

A blinky-light badge in the approximate shape of Tennessee

[Join the Tennessee Cybersecurity Discord](https://discord.gg/tninfosec) and
[participate in discussion about this badge](https://discord.com/channels/781280955667185686/1010244036612862144).

## Features

### human-accessible output
- 5 of addressable RGB LED (APA102) at the approximate locations of the top 5 population centres
(west to east: Memphis, Clarksville, Nashville, Chattanooga, Knoxville)  
on-chip SPI (SCLK from pin 9 (D8/GPIO8), MOSI from pin 11 (D10/GPIO10))

- 1 of user-populated LED-footprint, MOSFET-switched 5V supply with current-limiting resistor  
from pin 7 (D6/GPIO21)

- 1 of 212x104 pixel epaper display (grey/black, or grey/black/red), populated at either of 2 locations
(one almost entirely on-board; one almost entirely off-board and mounted from behind to not obscure Chattanooga's LED)  
on-chip SPI, /CS from pin 8 (D7/GPIO20), DATA/CMD from pin 2 (D1/GPIO3), /RESET from pin 7 (D6/GPIO21), BUSY to pin 1 (D0/GPIO2)


### human-accessible input
- 1 of LDR, darkness-proportional voltage at ADC  
to pin 3 (A2/D2/GPIO4)

- 1 of pushbutton digital input (D9/GPIO9)


### electronic
- 1 .. 3 of SAO V.1.69bis master connector, populated at any of 4 locations not obstructed by epaper  
on-chip I2C (SCL on pin 6 (D5/GPIO7), SDA on pin 5 (D4/GPIO6)), GPIO1 on pin 2 (D1/GPIO3), GPIO2 on pin 1 (D0/GPIO2)

- USB-C for power and programming

- 1000mAh LiPo battery

- battery voltage /2 at ADC  
to pin 4 (A3/D3/GPIO5)

- on-chip Bluetooth Low Energy / WiFi

- power ON/OFF switch

- epaper display / LED enable switch  
(to maximize epaper lifetime by disconnecting power other than when required for update,
and encourage this by having LED power mutually exclusive with epaper power)

## design considerations
- minimal components on front allowing maximum space for silkscreen art

- maximal GND-connected fill on both top and bottom for even background under soldermask (and to reduce electrical noise)

- flat empty space on back for LiPo battery to rest snug against PCB

- maximum flexibility for user customization, both physical and logical

- components chosen (and footprints customized) for hand-solderability where possible

## software
still to come

ideally, watches for pushbutton held down at boot to signal display update requested;
after display is updated, user flips epaper/LED switch for blinky goodness

possible behaviour-change triggers:
- ambient light level

- dropping battery voltage

- proximity of other badges, which specific badges they are, and *their* interaction history

- external access over BLE/WiFi by not-a-badge
