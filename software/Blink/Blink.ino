#define lRED D7
#define lGREEN D6

void setup() {
  digitalWrite(lRED, 0);
  pinMode(lRED, OUTPUT);
  digitalWrite(lGREEN, 0);
  pinMode(lGREEN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(lGREEN, 1);
  delay(500);
  digitalWrite(lRED, 1);
  delay(1000);
  digitalWrite(lGREEN, 0);
  delay(500);
  digitalWrite(lRED, 0);
  delay(1000);
}
