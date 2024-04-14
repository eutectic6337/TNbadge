#ifndef XIAO_ESP32C3_PINOUT_H_
#define XIAO_ESP32C3_PINOUT_H_

/*
  GPIO*n* is (from Arduino's PoV) pin *n*
  so, using the pin name -> function mapping spreadsheet from Seeed
*/
#define PIN_GPIO2 (2)
static const uint8_t pGPIO2 = PIN_GPIO2;
static const uint8_t pADC1_CH2 = PIN_GPIO2;
static const uint8_t pFSPIQ = PIN_GPIO2;
static const uint8_t pA0 = PIN_GPIO2;// ? 0
static const uint8_t pD0 = PIN_GPIO2;
#define PIN_GPIO3 (3)
static const uint8_t pGPIO3 = PIN_GPIO3;
static const uint8_t pADC1_CH3 = PIN_GPIO3;
static const uint8_t pA1 = PIN_GPIO3;// ? 1
static const uint8_t pD1 = PIN_GPIO3;
#define PIN_MTMS (4)
static const uint8_t pGPIO4 = PIN_MTMS;
static const uint8_t pADC1_CH4 = PIN_MTMS;
static const uint8_t pFSPIHD = PIN_MTMS;
static const uint8_t pMTMS = PIN_MTMS;
static const uint8_t pA2 = PIN_MTMS;// ? 2
static const uint8_t D2 = PIN_MTMS;
#define PIN_MTDI (5)
static const uint8_t pGPIO5 = PIN_MTDI;
static const uint8_t pADC2_CH0 = PIN_MTDI;
static const uint8_t pFSPIWP = PIN_MTDI;
static const uint8_t pMTDI = PIN_MTDI;
static const uint8_t pA3 = PIN_MTDI;// ? 3
static const uint8_t pD3 = PIN_MTDI;
#define PIN_MTCK (6)
static const uint8_t pGPIO6 = PIN_MTCK;
static const uint8_t pFSPICLK = PIN_MTCK;
static const uint8_t pMTCK = PIN_MTCK;
static const uint8_t pD4 = PIN_MTCK;
static const uint8_t pSDA = PIN_MTCK;// ? 8
#define PIN_MTDO (7)
static const uint8_t pGPIO7 = PIN_MTDO;
static const uint8_t pFSPID = PIN_MTDO;
static const uint8_t pMTDO = PIN_MTDO;
static const uint8_t pD5 = PIN_MTDO;
static const uint8_t pSCL = PIN_MTDO;// ? 9
#define PIN_GPIO8 (8)
static const uint8_t pGPIO8 = PIN_GPIO8;
static const uint8_t pD8 = PIN_GPIO8;
static const uint8_t pSCK = PIN_GPIO8;// ? 4
#define PIN_GPIO9 (9)
static const uint8_t pGPIO9 = PIN_GPIO9;
static const uint8_t pD9 = PIN_GPIO9;
static const uint8_t pMISO = PIN_GPIO9;// ? 5
#define PIN_GPIO10 (10)
static const uint8_t pGPIO10 = PIN_GPIO10;
static const uint8_t pFSPICS0 = PIN_GPIO10;
static const uint8_t pD10 = PIN_GPIO10;
static const uint8_t pMOSI = PIN_GPIO10;// ? 6
#define PIN_U0RXD (20)
static const uint8_t pGPIO20 = PIN_U0RXD;
static const uint8_t pU0RXD = PIN_U0RXD;
static const uint8_t pD7 = PIN_U0RXD;
static const uint8_t pRX = PIN_U0RXD;// ? 20
static const uint8_t pSS = PIN_U0RXD;// ? 7
#define PIN_U0TXD (21)
static const uint8_t pGPIO21 = PIN_U0TXD;
static const uint8_t pU0TXD = PIN_U0TXD;
static const uint8_t pD6 = PIN_U0TXD;
static const uint8_t pTX = PIN_U0TXD;// ? 21

#endif//XIAO_ESP32C3_PINOUT_H_
