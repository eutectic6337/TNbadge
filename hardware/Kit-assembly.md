# TNbadge Kit Assembly Guide

You've just been handed a Ziplock bag with a partially-assembled blinky-badge kit inside.
What now?

## What's in the bag?

(almost-) Full kit, from left:

- 30cm flexible LED, nominally pink, that you can use as the single LED

- 1000mAh Li-ion cell, will power this CPU (driving LEDs but without radio) for 24 hours continuous

- baggie containing boost converter, light sensor, and 3x Shitty-Add-On connectors

- USB C to USB C cable, for charging the battery and programming the CPU

- epaper display, and radio antenna with sticky back, in a box

- lanyard to clip onto holes in the top of the badge

missing:

- 3d-printed battery box to clip onto the back of the badge, into the 4 slots (supplied by @Lucien)

*Photo goes here*

## But I don't want a 30cm flexible pink LED

If you'd prefer a "normal" LED rather than the 30cm flexible one, I can toss in one or two red/green/yellow on request.
(I have hundreds and there's no way I'll use them all before I die, even though I plan to live forever.)

## If I were to go full monkey and do YOLO science with my board, will there be replacements available to purchase?

(thanks, @r0nk)

You can always make your own - the Kicad files
(and description of what I had to do to use JLCPCB's Colorful Silkscreen feature),
and also Gerber plots and Bill-Of-Materials including URLs for supplier pages, are all up here.

## OK, what do I need to do?

Get together parts and tools:

### Relevant parts from the packaged kit

(leave the epaper display alone for now ...)

bottom: the PCB, front side down as we'll start with through-hole parts on the back

top right: the little Zip-lock baggie with (from right)
- 3 Shitty Add On connectors
- 1 Light Dependent Resistor
- 1 boost converter module, with 3-pin header

top left: an assortment of Light Emitting Diodes, including the 30cm soft-flexible one that's in the kit as originally packed, and several "normal" ones which I can hand out if you want just a single spot rather than a pink glowy string

*Photo goes here*

### Tools you'll need

- a decent quality, ideally temperature controlled, soldering iron with a fairly fine tip;  
this is my Pinecil which I like very much and is very cheap ($26) if you don't have a suitable iron

*Photo goes here*

- long-nose or needle-nose pliers;  
I bought this pair over 35 years ago in a tool shop in Australia, and I've just recently found some almost identical on Amazon for almost exactly the same number of dollars as I paid

*Photo goes here*

- micro side-cutters or flush nippers;  
I broke the ones I bought with the above pliers, but got this pair from Uncle Jeff

*Photo goes here*

- rosin-flux cored electronics solder, either lead-free or with lead in;  
I don't see much need for lead-free for hobbyist use, as the amount of lead you'd absorb is so low as to be unmeasurable;  
the two rolls here are 1.0mm and 0.5mm diameter, which is about the extremes of useful sizes. If you get lead-inclusive, 63% tin/37% lead is the best alloy for usability. **DO NOT USE anything that mentions "plumbing" or acid.**

*Photo goes here*

- Not shown: a CR2032 coin cell for determining LED polarity.  
I've been playing with these things for more than 40 years now and still don't remember which of the short or long leg, or the flat or round side of the body, is the Anode and which is the Cathode, so I just ask the device.

### Step 1: light sensor

(LDR or light dependent resistor)

Bend its legs at a right angle, about 3mm (1/8") away from its body.

This component doesn't care which way around it is connected to the circuit so which way you choose to bend the legs is irrelevant.

*Photo goes here*

Position the LDR with its sensor face toward the top (longest) side of the badge and insert its legs through the two holes between the pushbutton switch and the nearest battery-box mounting slot.

*Photo goes here*

Push the LDR legs all the way in. Turn the board back-face down and lay it on your work surface - the battery connector will hold it an appropriate distance off the surface for the LDR to rest in the right position ready for soldering. You may want to place a weight on the board to hold it flat - I've used my side-cutters. 

*Photo goes here*

Solder the two legs to the board

- apply hot iron to both leg-of-component and pad-on-board simultaneously,

- apply solder to the heated pad until it melts and flows,

- remove non-melted solder,

- wait a few more seconds for molten solder to go around whole joint and make concave fillets between surfaces,

- remove iron;

- repeat on other leg

Trim the legs close to the board (at about the point where the fillet of solder has dropped to zero thickness)

*Photo goes here*

### Step 2: boost converter module

Put the long end of the 3-pin header into the boost converter module from the low-profile side.

*Photo goes here*

(See *Photo goes here*
 for how this module will sit on the badge)

Flip the board to front-down position again

Solder the 3 pins onto the module, using the same technique you used for the LDR.

*Photo goes here*

(If the pins are not exactly at a right angle to the module board, it's OK.)

Insert the pins into the main badge PCB with the module oriented as indicated by the footprint around the pads;
then flip the badge PCB (holding the module in place) so you can access the end of the pins. 

*Photo goes here*

Solder the pins to the badge.

*Photo goes here*

### Step 3: decision time

Which of the two locations you choose for the epaper display will determine

- which SAO connectors can reasonably be populated, and

- your options for installing the single LED.

*Photo goes here*

In short:

**epaper upper left** =>

- max 2 SAO connectors;

- "normal" LED needs 2 offset bends in its legs to peek out from under the epaper,
or soft flexible LED needs to be inserted from the back;

- and you obscure some of the wonderful art

**epaper lower right** =>

- can install 3 SAO connectors (or even 4, if you're OK with a SAO possibly obscuring the epaper);

- trivial LED install;

- and you see all of @heartsh8ped's artwork

#### epaper to the left

Mounting epaper on the left means two SAO connectors on the right.

*Photo goes here*

#### epaper to the right

Alternatively, the epaper display can be mounted at the lower right of the badge, allowing installation of 3 SAO connectors.

*Photo goes here*

### Step 4: single LED

#### Step 4: single LED on the front

First, verify with a CR2032 coin cell (or, *trust me, bro*) that the long leg of the LED,
which is attached to the small part inside the transparent plastic body, is the positive side or Anode.

*Photo goes here*

Bend the legs so the body of the LED will sit almost at the battery box mounting slot when the legs go through the pads,
Anode toward the top of the badge.

*Photo goes here*

Invert the board ready for soldering (note, longer leg pokes out of hole labeled "+")

*Photo goes here*

Solder and snip, just like the LDR.

#### soft-flexible LED strip on the front

The pin on one end of this has a tiny hole in it - this is the positive end or Anode -
and can also be verified with the old reliable CR2032

*Photo goes here*

insert that pin through the top LED-mounting hole from the front of the badge and again carefully flip the board
(using your needle-nose pliers or tweezers to hold the pin, stopping it from falling out)

*Photo goes here*

insert the other pin and solder

*Photo goes here*

### Step 5: SAO connectors

Note the key sticking out toward the top of the badge, matching the marking on the silkscreen;
get this the wrong way around and best case you SAO won't work, worst case it will release the magic smoke.

*Photo goes here*

Take care when inverting the board to solder these, as they'll just fall straight out if not supported all the way.
Solder 1 pin on each connector first

*Photo goes here*

and then a second (I recommend start with opposite corners) after verifying that all pins poke through the board

*Photo goes here*

Then do the rest of the pins

### Step 6: epaper

#### to the left

The soldering for this is basically a bigger version of what we did on the boost converter module - 8 pins instead of 3.

*Photo goes here*

When mounting the epaper to the top left of the badge,
insert the short side of the 8-pin header into the holes on the epaper board from behind.

*Photo goes here*

Solder one end pin, check that everything is still lined up OK, and solder the other end.

*Photo goes here*

The joints should all be smooth all the way around each pin, and smooth onto the board, preferably concave.
The second image shows that these are marginal - do better than me.

*Photo goes here*

Final step: insert into the badge from the front, and solder behind (end pins first as before).

*Photo goes here*

In the "after" image above, all except the rightmost pin have the shape you're looking for. The rightmost solder joins the PCB at an acute angle and is probably not making good electrical contact, so I re-melted it and applied fresh solder (and more importantly, flux) to the junction of pin and board and left the heat on for a couple seconds longer. It's not great, but it'll do.

*Photo goes here*

#### epaper to the right

In this case, we need to insert the pins into the epaper board from its front

*Photo goes here*

solder them

*Photo goes here*

insert into badge PCB from behind

*Photo goes here*

and solder

*Photo goes here*

These long pins poking out the front are perhaps less needing of a trim than if they poked out the back.
But if you do this assembly and want them trimmed, grab me and my Industrial Cutters.

Oh, and you'll want to peel off the film from the front of the epaper, using the red tab off its top left corner.


### Step 7: battery and battery box

*Photo goes here*

### Step 8: WiFi antenna

Plugging the antenna into the CPU module is ... not fun.

I finally managed it by holding the badge in my left hand thus,
with index finger holding the back of the (teeny) plug in place
over the (tiny) socket as I wriggled it by the cable held in my right hand.
When it looked and felt like it was in the right spot, gentle pressure from left index finger snapped it in place.

*Photo goes here*

### Step 9: trim pins

You'll notice that the pins from the epaper poke through quite a distance here.

If you have a really good (very hard jaws) pair of side-cutters you can use them to cut off these pins.
If you use nice but not-very-hard-jawed cutters, you'll get this
(as I did elsewhere, thus needing to buy my new Hakko cutters).

*Photo goes here*

### Step 10: lanyard

Attach the lanyard to the holes in the top corners of the badge, and it's done.

*Photo goes here*


*Photo goes here*


