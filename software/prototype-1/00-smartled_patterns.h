// Each of the badge's 5 cities must have a CITY_BRIGHT() and a CITY_COLOR() invocation:
//   CITY_BRIGHT(cityname,
//               brightness-speed-factor,
//               {comma-separated-sequence-of-brightness-transitions}
//               );
//   CITY_COLOR(cityname,
//              color-speed-factor,
//              {comma-separated-sequence-of-color-transitions}
//             );

// Each brightness transition is
//    { milliseconds-to-completion, target-brightness-percentage }

// Each color transition is
//    { milliseconds-to-completion, target-color }
// (color names matching those in the HTML spec are provided as CRGB::colorname)

// The speed factor determines the speed at which the sequence of transitions runs
// versus its explicit timings

// Some helper macros are provided to simplify defining common brightness waveforms
//    SQUARE(), TRIANGLE(), SAWTOOTH()
// and color sequences
//    RAINBOW()

CITY_BRIGHT(Memphis,1.1,{TRIANGLE(10,80,1000), TRIANGLE(10,80,800), SAWTOOTH(10,80,600)});
CITY_BRIGHT(Clarkesville,1.0,{TRIANGLE(5,90,1000), TRIANGLE(5,85,900)});
CITY_BRIGHT(Nashville,1.2,{TRIANGLE(0,100,1000), SQUARE(0,100,100), SQUARE(0,100,100)});
CITY_BRIGHT(Chattanooga,1.1,{TRIANGLE(5,90,1000), TRIANGLE(5,90,600), TRIANGLE(5,90,200)});
CITY_BRIGHT(Knoxville,1.0,{TRIANGLE(10,80,700), TRIANGLE(10,85,1000)});

CITY_COLOR(Memphis,1.2,{RAINBOW(3000), {1000,CRGB::Orange});
CITY_COLOR(Clarkesville,1.1,{RAINBOW(3000), {900,CRGB:Yellow});
CITY_COLOR(Nashville,1.0,{RAINBOW(3300), {800,CRGB::Green});
CITY_COLOR(Chattanooga,1.0,{RAINBOW(3000), {700,CRGB::Blue});
CITY_COLOR(Knoxville,1.1,{RAINBOW(3000), {600,CRGB::Violet});
