
#define pGPIO2 (2)
#define pGPIO3 (3)
#define pGPIO4 (4)
#define pGPIO5 (5)
#define pGPIO6 (6)
#define pGPIO7 (7)
#define pGPIO21 (21)

#define pGPIO10 (10)
#define pGPIO9 (9)
#define pGPIO8 (8)
#define pGPIO20 (20)


#define pD0 pGPIO2
#define pD1 pGPIO3
#define pD2 pGPIO4
#define pD3 pGPIO5
#define pD4 pGPIO6
#define pD5 pGPIO7
#define pD6 pGPIO21

#define pD10 pGPIO10
#define pD9 pGPIO9
#define pD8 pGPIO8
#define pD7 pGPIO20


#define pA0 pD0
#define pA1 pD1
#define pA2 pD2
#define pA3 pD3
#define pSDA pD4
#define pSCL pD5
#define pTX pD6

#define pMOSI pD10
#define pMISO pD9
#define pSCK pD8
#define pRX pD7




#define lRED pGPIO20
#define lGREEN pGPIO21

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
