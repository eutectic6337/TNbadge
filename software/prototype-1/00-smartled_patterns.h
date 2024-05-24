#ifdef ENABLE_SMARTLEDS

#define SQUARE_(a,b,r,t) {0,(a)},{(r)*(t)/100,(a)},{0,(b)},{((100-(r))*(t)/100,(b)}
#define SQUARE(a,b,t) SQUARE_(a,b,50,t)

#define TRIANGLE_(a,b,r,t) {(r)*(t)/100,(a)},{((100-(r))*(t)/100,(b)}
#define TRIANGLE(a,b,t) TRIANGLE_(a,b,50,t)

#define SAWTOOTH(a,b,t) TRIANGLE_(a,b,100,t)
#define REVERSE_SAWTOOTH(a,b,t) TRIANGLE_(a,b,0,t)

const struct brightness_sequence {
  double speed;
  struct {int ms;int value} s[];
} city_brightness[] = {
/* Memphis */ {1.1,{
  TRIANGLE(10,80,1000)
}},
/* Clarkesville */ {1.0,{
  TRIANGLE(10,90,1000)
}},
/* Nashville */ {1.2,{
  TRIANGLE(10,100,1000)
}},
/* Chattanooga */ {1.1,{
  TRIANGLE(10,90,1000)
}},
/* Knoxville */ {1.0,{
  TRIANGLE(10,80,1000)
}}
}

#define RAINBOW(t) \
  {(t)/6,CRGB::Red},\
  {(t)/6,CRGB::Orange},\
  {(t)/6,CRGB::Yellow},\
  {(t)/6,CRGB::Green},\
  {(t)/6,CRGB::Blue},\
  {(t)/6,CRGB::Violet}}

const struct color_sequence {
  double speed;
  struct {int ms;CRGB value} s[];
} city_brightness[] = {
/* Memphis */ {1.1,{
  RAINBOW(3000)
}},
/* Clarkesville */ {1.0,{
  RAINBOW(3000)
}},
/* Nashville */ {1.2,{
  RAINBOW(3000)
}},
/* Chattanooga */ {1.1,{
  RAINBOW(3000)
}},
/* Knoxville */ {1.0,{
  RAINBOW(3000)
}}
}

#endif