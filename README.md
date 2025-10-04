# TNbadge

A blinky-light badge in the approximate shape of Tennessee

[PCB front](hardware/i/bare_PCB-front.jpg) [PCB back](hardware/i/bare_PCB-back.jpg)

[Join the Tennessee Cybersecurity Discord](https://discord.gg/tninfosec) and
[participate in discussion about this badge](https://discord.com/channels/781280955667185686/1010244036612862144).

[User's Guide](users-guide.md)

## Features

### human-accessible output
- 5 of [2.0mm square addressable RGB LED (APA102)](https://www.aliexpress.us/item/2251832665711027.html)
at the approximate locations of the top 5 population centres
(west to east: Memphis, Clarksville, Nashville, Chattanooga, Knoxville)  
on-chip SPI (SCLK from pin 9 (D8/GPIO8), MOSI from pin 11 (D10/GPIO10))

- 1 of user-populated [LED](https://www.aliexpress.us/item/3256802159185271.html)-footprint,
[switched](https://www.aliexpress.us/item/2251832541429715.html)
[5V supply](https://www.aliexpress.us/item/3256801453688823.html)
with current-limiting resistor  
from pin 7 (D6/GPIO21)

- 1 of [212x104 pixel 2.13 inch epaper display (grey/black/red)](https://www.aliexpress.us/item/3256806569318405.html)
populated at either of 2 locations
(one almost entirely on-board; one almost entirely off-board and mounted from behind to not obscure Chattanooga's LED)  
on-chip SPI, /CS from pin 4 (D3/GPIO5), DATA/CMD from pin 2 (D1/GPIO3), /RESET from pin 7 (D6/GPIO21), BUSY to pin 1 (D0/GPIO2)


### human-accessible input
- 1 of LDR, darkness-proportional voltage at ADC  
to pin 3 (A3/D3/GPIO5)  
*unfortunately, the chip configuration in Espressif's Arduino chip-support package connects a non-functional ADC to this pin, so this can't work without a deeper dive* :(

- 1 of [pushbutton](https://www.aliexpress.us/item/2251832616067755.html)
digital input to pin 10 (D9/GPIO9)


### electronic
- 1 .. 3 of [SAO V.1.69bis master connector](https://www.aliexpress.us/item/2251832874579437.html)
populated at any of 4 locations not obstructed by epaper  
on-chip I2C (SCL on pin 6 (D5/GPIO7), SDA on pin 5 (D4/GPIO6)), GPIO1 on pin 2 (D1/GPIO3), GPIO2 on pin 1 (D0/GPIO2)

- [USB-C](https://www.aliexpress.us/item/2251832621183502.html)
for power and programming

- [1000mAh LiPo battery](https://www.aliexpress.us/item/3256803012248557.html)

- battery voltage /2 at ADC  
to pin 3 (A2/D2/GPIO4)

- on-chip Bluetooth Low Energy / WiFi

- [power ON/OFF switch](https://www.aliexpress.us/item/2255799844067525.html)

- [epaper display / LED enable switch](https://www.aliexpress.us/item/2255799844067525.html)
(to maximize epaper lifetime by disconnecting power other than when required for update,
and encourage this by having LED power mutually exclusive with epaper power)

## design considerations
- minimal components on front allowing maximum space for silkscreen art

- maximal GND-connected fill on both top and bottom for even background under soldermask (and to reduce electrical noise)

- flat empty space on back for LiPo battery to rest snug against PCB

- maximum flexibility for user customization, both physical and logical

- components chosen (and footprints customized) for hand-solderability where possible

## software
[prototype implementation](https://github.com/eutectic6337/TNbadge/tree/main/software/prototype-1)

ideally, watches for pushbutton held down at boot to signal display update requested;
after display is updated, user flips epaper/LED switch for blinky goodness

possible behaviour-change triggers:
- ambient light level

- dropping battery voltage

- proximity of other badges, which specific badges they are, and *their* interaction history

- external access over BLE/WiFi by not-a-badge

# production timeline
Board design is now locked down.

- First batch of badge kits
(SMD components soldered onto PCB, through-hole assembly to be completed by user)
distributed in April 2024 to software development volunteers

- Artwork has been added to board silkscreen

- epaper displays are now in hand

- remaining PCBs to be delivered mid June

- remaining CPU modules to be ordered mid June

- SMD assembly to start when PCBs arrive

- bagged kits distributed as soon as possible, but no later than
[DEF CON 32](https://defcon.org) in Las Vegas
