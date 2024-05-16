#ifndef BADGE_BLUETOOTH_H_
#define BADGE_BLUETOOTH_H_

#ifdef ENABLE_BLUETOOTH
//284050
//284228
#include "SimpleBLE.h"
SimpleBLE ble;
void setup_simpleBLE()
{
  ble.begin("ESP32 SimpleBLE");
}
void update_simpleBLE()
{
    String out = "BLE32 name: ";
    out += String(millis() / 1000);
    Serial.println(out);
    ble.begin(out);
}
#else
#define setup_simpleBLE() ((void)0)
#define update_simpleBLE() ((void)0)
#endif


#ifdef ENABLE_BLUETOOTH
// IDEA: BLE uses so much flash it's probably not worth it
//284228
//1002926

//284050
//1002746
#include <BLEDevice.h>
#include <BLEAdvertising.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEBeacon.h>
#include <BLEServer.h>
// https://www.bluetooth.com/wp-content/uploads/Files/Specification/HTML/Assigned_Numbers/out/en/Assigned_Numbers.pdf
#define SERVICE_UUID        "6050bcfe-a8f8-4ad6-961d-5af97ac37e09"
#define CHARACTERISTIC_UUID "cab5c6ff-31a7-4b2a-aada-4bae02ca1ca7"
void setup_Bluetooth()
{
  // ======== CUSTOMIZE Bluetooth HERE ========
  // IDEA: https://en.wikipedia.org/wiki/Bluetooth_Low_Energy_beacon


  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello World says Neil");
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}
void update_Bluetooth()
{
  // ======== CUSTOMIZE Bluetooth HERE ========
}
#else
#define setup_Bluetooth() ((void)0)
#define update_Bluetooth() ((void)0)
#endif

#endif//BADGE_BLUETOOTH_H_
