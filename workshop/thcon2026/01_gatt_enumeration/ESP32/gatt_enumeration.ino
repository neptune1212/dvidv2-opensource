/*
 * THCON 2026 - DVID/WOCSA Bluetooth Workshop
 * Challenge 01 - GATT Enumeration
 *
 * The ESP32 hosts a simple GATT server with a single service and a single
 * READ characteristic containing the flag in plain text.
 * Participants must scan, connect, enumerate, and read the characteristic.
 *
 * Flag: WOCSA{ble_gatt_is_easy_to_read}
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Device & flag
#define DEVICE_NAME   "THCON26_BLE_01"
#define FLAG          "WOCSA{ble_gatt_is_easy_to_read}"

// Custom 128-bit UUIDs
#define SERVICE_UUID        "12345678-1234-1234-1234-123456789001"
#define CHARACTERISTIC_UUID "12345678-1234-1234-1234-123456789002"

BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("[+] Client connected");
  }
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("[-] Client disconnected, restarting advertising...");
    pServer->startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("[*] Challenge 01 - GATT Enumeration");

  BLEDevice::init(DEVICE_NAME);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  // Create service
  BLEService* pService = pServer->createService(SERVICE_UUID);

  // Create READ characteristic with the flag
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  pCharacteristic->setValue(FLAG);

  pService->start();

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  BLEDevice::startAdvertising();

  Serial.println("[*] BLE advertising started");
  Serial.print("[*] Device name: ");
  Serial.println(DEVICE_NAME);
}

void loop() {
  delay(1000);
}
