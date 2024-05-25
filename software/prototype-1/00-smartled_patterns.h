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

const double brightness_sequence_speed_Memphis=1.1;
struct brightness_transition brightness_tx_Memphis [] = {TRIANGLE(10,80,1000), TRIANGLE(10,80,800), SAWTOOTH(10,80,600)};
const double brightness_sequence_speed_Clarkesville=1.0;
struct brightness_transition brightness_tx_Clarkesville [] = {TRIANGLE(5,90,1000), TRIANGLE(5,85,900)};
const double brightness_sequence_speed_Nashville=1.2;
struct brightness_transition brightness_tx_Nashville [] = {TRIANGLE(0,100,1000), SQUARE(0,100,100), SQUARE(0,100,100)};
const double brightness_sequence_speed_Chattanooga=1.1;
struct brightness_transition brightness_tx_Chattanooga [] = {TRIANGLE(5,90,1000), TRIANGLE(5,90,600), TRIANGLE(5,90,200)};
const double brightness_sequence_speed_Knoxville=1.0;
struct brightness_transition brightness_tx_Knoxville [] = {TRIANGLE(10,80,700), TRIANGLE(10,85,1000)};

const double color_sequence_speed_Memphis=1.2;
struct color_transition color_tx_Memphis [] = {};//{RAINBOW(3000), {1000,CRGB::Orange}};
const double color_sequence_speed_Clarkesville=1.1;
struct color_transition color_tx_Clarkesville [] = {};//{RAINBOW(3000), {900,CRGB:Yellow}};
const double color_sequence_speed_Nashville=1.0;
struct color_transition color_tx_Nashville [] = {};//{RAINBOW(3300), {800,CRGB::Green}};
const double color_sequence_speed_Chattanooga=1.0;
struct color_transition color_tx_Chattanooga [] = {};//{RAINBOW(3000), {700,CRGB::Blue}};
const double color_sequence_speed_Knoxville=1.1;
struct color_transition color_tx_Knoxville [] = {};//{RAINBOW(3000), {600,CRGB::Violet}};
