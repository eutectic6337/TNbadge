/*
  validate TNbadge hardware, one module at a time

  based on 
  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogReadSerial

  Reads an analog input, prints the result to the Serial Monitor.
*/

int Half_Battery_Voltage = A3;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(Half_Battery_Voltage);
  // print out the value you read:
  Serial.println(sensorValue);
  delay(100);  // delay in between reads for stability
}
