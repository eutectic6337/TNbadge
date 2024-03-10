/*
  validate TNbadge hardware, one module at a time

  based on 
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink

  Turns the single LED on for one second, then off for one second, repeatedly.
*/

int Single_LED = GPIO21;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(Single_LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(Single_LED, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(Single_LED, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
