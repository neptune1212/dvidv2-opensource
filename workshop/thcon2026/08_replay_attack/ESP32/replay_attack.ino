/*
 * THCON 2026 - DVID/WOCSA Bluetooth Workshop
 * Challenge 08 - Replay Attack
 *
 * The ESP32 requires a specific 4-byte magic payload to unlock the flag.
 * The magic bytes are: 0xDE 0xAD 0xBE 0xEF
 *
 * A PCAP file (capture.pcap) is provided alongside this challenge.
 * It contains a recording of a legitimate admin unlocking the device.
 * Participants must:
 *   1. Open the PCAP in Wireshark
 *   2. Filter for btatt WRITE operations
 *   3. Extract the magic payload (DEADBEEF)
 *   4. Replay it to unlock the flag characteristic
 *
 * Flag: WOCSA{replay_attacks_bypass_auth}
 *
 * Note: The PCAP file must be generated separately using a BLE sniffer
 * (e.g., Ubertooth One, nRF Sniffer, or Wireshark with a compatible adapter).
 * The capture should show: WRITE to AUTH_CHAR with value DE AD BE EF
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define DEVICE_NAME   "THCON26_BLE_08"
#define FLAG          "WOCSA{replay_attacks_bypass_auth}"
#define LOCKED_MSG    "Locked: Authentication Required"

// Magic bytes required to unlock
static const uint8_t MAGIC[] = {0xDE, 0xAD, 0xBE, 0xEF};

#define SERVICE_UUID   "facade00-0000-1000-8000-00805f9b34fb"
#define FLAG_CHAR_UUID "facade00-0001-1000-8000-00805f9b34fb"
#define AUTH_CHAR_UUID "facade00-0002-1000-8000-00805f9b34fb"

BLECharacteristic* pFlagChar = nullptr;
bool unlocked = false;

class AuthCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pChar) {
    String val = pChar->getValue();

    if (val.length() == 4 &&
        (uint8_t)val[0] == MAGIC[0] &&
        (uint8_t)val[1] == MAGIC[1] &&
        (uint8_t)val[2] == MAGIC[2] &&
        (uint8_t)val[3] == MAGIC[3]) {
      unlocked = true;
      pFlagChar->setValue(FLAG);
      Serial.println("[+] Magic bytes matched! Flag unlocked.");
    } else {
      Serial.print("[-] Wrong magic bytes: ");
      for (int i = 0; i < (int)val.length(); i++) {
        Serial.printf("%02X ", (uint8_t)val[i]);
      }
      Serial.println();
    }
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("[+] Client connected");
  }
  void onDisconnect(BLEServer* pServer) {
    Serial.println("[-] Client disconnected, resetting...");
    unlocked = false;
    pFlagChar->setValue(LOCKED_MSG);
    pServer->startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("[*] Challenge 08 - Replay Attack");
  Serial.println("[*] Magic bytes: DE AD BE EF");

  BLEDevice::init(DEVICE_NAME);

  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  // FLAG characteristic (READ)
  pFlagChar = pService->createCharacteristic(
    FLAG_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  pFlagChar->setValue(LOCKED_MSG);

  // AUTH characteristic (WRITE) — accepts magic bytes
  BLECharacteristic* pAuthChar = pService->createCharacteristic(
    AUTH_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  pAuthChar->setCallbacks(new AuthCallback());

  pService->start();

  BLEAdvertising* pAdv = BLEDevice::getAdvertising();
  pAdv->addServiceUUID(SERVICE_UUID);
  pAdv->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("[*] BLE advertising started");
}

void loop() {
  delay(1000);
}
