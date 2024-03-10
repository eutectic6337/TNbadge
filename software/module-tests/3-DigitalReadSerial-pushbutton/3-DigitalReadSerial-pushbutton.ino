/*
  validate TNbadge hardware, one module at a time

  based on 
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/DigitalReadSerial

  Reads a digital input, prints the result to the Serial Monitor
*/

// digital pin 2 has a pushbutton attached to it. Give it a name:
int pushButton = GPIO9;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int buttonState = digitalRead(pushButton);
  // print out the state of the button:
  Serial.println(buttonState);
  delay(10);  // delay in between reads for stability
}
