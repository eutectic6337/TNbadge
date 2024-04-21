# TNbadge hardware

## Printed Circuit Board

- designed in Kicad

- imported into [EasyEDA Pro](https://pro.easyeda.com)

- multicolour PNG imported to "silkscreen" layer

- ordered from [JLC PCB](https://jlcpcb.com)

## component sources

- already in hand (through-hole LDRs)

- [LCSC](https://lcsc.com) (SMD resistors & capacitors)

- [Seeed Studio](https://seeedstudio.com) (CPU module)

- [AliExpress](https://aliexpress.com) (everything else)


# assembly and test

## front surface mount

### assembly

- stencil solder paste on front

- place 2x 100k resistor

- place 5x APA102 addressable RGB LED

- (optional) preheat to 100 degC

- apply hot air at 200+ degC to each component until solder flows to create concave surface

### test

- position board front-down on stable non-conductive surface

- starting at lower right corner, test continuity between each pair of pads from the set
{test points, CPU pads}

- continuity is expected between
-- TP1 CPU13 CPU\_GND CPU\_BAT-
-- TP2 CPU\_BAT+
-- TP3 CPU12
-- TP15 CPU8
-- TP18 CPU7
-- TP20 CPU1

- continuity between any pair of test points,
or between any pair of CPU pads,
or between any other test points and CPU pads
indicates a likely solder bridge

- continuity between any of TP5 through TP14 and TP4 or TP1
indicates a bridge between pads of one of the smart LEDs either side of the
involved TP5...TP14

### rectify

For each identified fault

- desolder identified likely culprit components

- flux and wick up solder from PCB

- re-test to verify fault is gone; if fault remains, dig deeper

- flux pads and reposition component

- reflow remaining solder

- add minimal solder if needed

- re-test to verify fault is gone

## back surface mount

### assembly

- stencil solder paste on front

- place 1x level shifter chip

- place 1x 100 ohm resistor

- place 6x 10k resistor

- place 7x 1uF capacitor

- place 2x slide switch

- place 1x battery connector

- (optional) preheat to 100 degC

- apply hot air at 200+ degC to each component until solder flows to create concave surface

### test

- position board front-down on stable non-conductive surface

- starting at lower right corner, test continuity between each pair of pads from the set
{test points, CPU pads}

- continuity is expected between

   - TP1 CPU13 CPU\_GND CPU\_BAT-

   - TP2 CPU\_BAT+

   - TP3 CPU12

   - TP15 CPU8

   - TP19 CPU7

   - TP20 CPU1

- continuity between any pair of test points,
or between any pair of CPU pads,
or between any other test points and CPU pads
indicates a likely solder bridge

### rectify

most likely location for bridge is level shifter chip

- flux and wick up excess solder from pins on one side

- drag-solder with small wedge tip iron to reflow remaining solder

- re-test to verify fault is gone; if fault remains, dig deeper

For each other identified fault

- desolder identified likely culprit components

- flux and wick up solder from PCB

- re-test to verify fault is gone; if fault remains, dig deeper

- flux pads and reposition component

- reflow remaining solder

- add minimal solder if needed

- re-test to verify fault is gone

