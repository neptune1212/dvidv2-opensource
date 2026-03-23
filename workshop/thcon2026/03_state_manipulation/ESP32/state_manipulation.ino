/*
 * THCON 2026 - DVID/WOCSA Bluetooth Workshop
 * Challenge 03 - State Manipulation
 *
 * The ESP32 exposes two characteristics:
 *   - FLAG_CHAR (READ): returns "Access Denied" until unlocked, then the flag
 *   - UNLOCK_CHAR (WRITE): writing 0x01 sets unlocked=true
 *
 * Participants must discover the WRITE characteristic, write 0x01 to it,
 * then re-read the FLAG characteristic to get the flag.
 *
 * Flag: WOCSA{write_to_unlock_the_secret}
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define DEVICE_NAME   "THCON26_BLE_03"
#define FLAG          "WOCSA{write_to_unlock_the_secret}"
#define ACCESS_DENIED "Access Denied"

#define SERVICE_UUID        "abcdef01-0000-1000-8000-00805f9b34fb"
#define FLAG_CHAR_UUID      "abcdef02-0000-1000-8000-00805f9b34fb"
#define UNLOCK_CHAR_UUID    "abcdef03-0000-1000-8000-00805f9b34fb"

bool unlocked = false;
BLECharacteristic* pFlagChar = nullptr;

// Callback for the UNLOCK characteristic
class UnlockCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pChar) {
    String value = pChar->getValue();
    if (value.length() > 0 && (uint8_t)value[0] == 0x01) {
      unlocked = true;
      pFlagChar->setValue(FLAG);
      Serial.println("[+] Device unlocked! Flag is now readable.");
    } else {
      Serial.println("[-] Wrong unlock value received.");
    }
  }
};

// Callback for the FLAG characteristic
class FlagCallback : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic* pChar) {
    if (!unlocked) {
      pChar->setValue(ACCESS_DENIED);
      Serial.println("[-] Read attempt on locked flag.");
    } else {
      Serial.println("[+] Flag read by client.");
    }
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("[+] Client connected");
  }
  void onDisconnect(BLEServer* pServer) {
    Serial.println("[-] Client disconnected, resetting state...");
    unlocked = false;
    pFlagChar->setValue(ACCESS_DENIED);
    pServer->startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("[*] Challenge 03 - State Manipulation");

  BLEDevice::init(DEVICE_NAME);

  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  // FLAG characteristic (READ only)
  pFlagChar = pService->createCharacteristic(
    FLAG_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  pFlagChar->setCallbacks(new FlagCallback());
  pFlagChar->setValue(ACCESS_DENIED);

  // UNLOCK characteristic (WRITE only)
  BLECharacteristic* pUnlockChar = pService->createCharacteristic(
    UNLOCK_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pUnlockChar->setCallbacks(new UnlockCallback());

  pService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("[*] BLE advertising started");
}

void loop() {
  delay(1000);
}
