/*
 * THCON 2026 - DVID/WOCSA Bluetooth Workshop
 * Challenge 04 - Identity Theft
 *
 * The ESP32 only allows connections from a specific trusted MAC address.
 * The trusted MAC is hinted in the scan response (device name suffix).
 * Any other MAC is immediately disconnected.
 *
 * Participants must:
 *   1. Scan and find the trusted MAC hint in the advertisement
 *   2. Spoof their adapter's MAC to match DE:AD:BE:EF:CA:FE
 *   3. Reconnect and read the flag
 *
 * Flag: WOCSA{mac_auth_is_not_real_security}
 */

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define DEVICE_NAME      "THCON26_BLE_04"
#define SCAN_RESP_NAME   "Trusted:DE:AD:BE:EF:CA:FE"
#define FLAG             "WOCSA{mac_auth_is_not_real_security}"
#define ACCESS_DENIED    "Access Denied: Unknown device"

// Trusted MAC address (lowercase, colon-separated as returned by ESP32 API)
#define TRUSTED_MAC      "de:ad:be:ef:ca:fe"

#define SERVICE_UUID     "cafebabe-0000-1000-8000-00805f9b34fb"
#define FLAG_CHAR_UUID   "cafebabe-0001-1000-8000-00805f9b34fb"

BLECharacteristic* pFlagChar = nullptr;
BLEServer* pServer = nullptr;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t* param) {
    // Get connecting device MAC
    char mac[18];
    uint8_t* addr = param->connect.remote_bda;
    snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
      addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    Serial.print("[*] Connection attempt from MAC: ");
    Serial.println(mac);

    if (String(mac) == String(TRUSTED_MAC)) {
      Serial.println("[+] Trusted MAC! Access granted.");
      pFlagChar->setValue(FLAG);
    } else {
      Serial.println("[-] Untrusted MAC! Disconnecting...");
      pFlagChar->setValue(ACCESS_DENIED);
      // Disconnect after a short delay
      delay(100);
      pServer->disconnect(param->connect.conn_id);
    }
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("[-] Client disconnected, restarting advertising...");
    pFlagChar->setValue(ACCESS_DENIED);
    pServer->startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("[*] Challenge 04 - Identity Theft");
  Serial.print("[*] Trusted MAC: ");
  Serial.println(TRUSTED_MAC);

  BLEDevice::init(DEVICE_NAME);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);

  pFlagChar = pService->createCharacteristic(
    FLAG_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ
  );
  pFlagChar->setValue(ACCESS_DENIED);

  pService->start();

  // Main advertisement: device name
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);

  // Scan response: hint about the trusted MAC
  BLEAdvertisementData scanRsp;
  scanRsp.setName(SCAN_RESP_NAME);
  pAdvertising->setScanResponseData(scanRsp);

  BLEDevice::startAdvertising();
  Serial.println("[*] BLE advertising started");
}

void loop() {
  delay(1000);
}
