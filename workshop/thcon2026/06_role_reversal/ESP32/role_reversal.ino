/*
 * THCON 2026 - DVID/WOCSA Bluetooth Workshop
 * Challenge 06 - Role Reversal
 *
 * The ESP32 acts as a BLE Central device.
 * It continuously scans for a device advertising as "PwnMe_Beacon".
 * When found, it connects and writes the flag to a well-known characteristic.
 * It also prints the flag to Serial for confirmation.
 *
 * Participants must broadcast "PwnMe_Beacon" from their computer/phone
 * and expose a WRITE characteristic at the known UUID.
 *
 * Flag: WOCSA{you_are_the_peripheral_now}
 *
 * Target characteristic UUID (on participant device):
 *   deadbeef-0000-1000-8000-00805f9b34fb
 */

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEClient.h>
#include <BLEAdvertisedDevice.h>

#define TARGET_NAME   "PwnMe_Beacon"
#define FLAG          "WOCSA{you_are_the_peripheral_now}"
#define FLAG_CHAR_UUID "deadbeef-0000-1000-8000-00805f9b34fb"

BLEScan* pBLEScan = nullptr;
bool flagDelivered = false;

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("[*] Found device: ");
    Serial.println(advertisedDevice.getName().c_str());

    if (advertisedDevice.getName() == TARGET_NAME) {
      Serial.println("[+] PwnMe_Beacon found! Stopping scan and connecting...");
      pBLEScan->stop();

      BLEClient* pClient = BLEDevice::createClient();
      if (pClient->connect(&advertisedDevice)) {
        Serial.println("[+] Connected to PwnMe_Beacon!");

        // Try to write the flag to the known characteristic
        BLERemoteService* pSvc = nullptr;
        BLERemoteCharacteristic* pChar = nullptr;

        // Iterate services to find our target characteristic
        std::map<std::string, BLERemoteService*>* services = pClient->getServices();
        for (auto& svcPair : *services) {
          BLERemoteService* svc = svcPair.second;
          std::map<std::string, BLERemoteCharacteristic*>* chars = svc->getCharacteristics();
          for (auto& charPair : *chars) {
            if (charPair.first == FLAG_CHAR_UUID) {
              pChar = charPair.second;
              break;
            }
          }
          if (pChar) break;
        }

        if (pChar && pChar->canWrite()) {
          pChar->writeValue(FLAG, strlen(FLAG));
          Serial.println("[+] Flag written to participant device characteristic!");
        } else {
          Serial.println("[!] Target characteristic not found or not writable.");
          Serial.println("[+] Flag delivery via Serial:");
        }

        // Always print the flag to Serial as a fallback
        Serial.print("[FLAG] ");
        Serial.println(FLAG);
        flagDelivered = true;

        pClient->disconnect();
      } else {
        Serial.println("[-] Connection failed.");
      }

      // Restart scan after attempt
      delay(2000);
      pBLEScan->start(5, false);
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("[*] Challenge 06 - Role Reversal");
  Serial.println("[*] ESP32 is scanning for 'PwnMe_Beacon'...");

  BLEDevice::init("THCON26_Scanner");

  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  pBLEScan->start(5, false);
}

void loop() {
  if (!flagDelivered) {
    // Continue scanning
    delay(100);
  } else {
    // Flag delivered, idle
    delay(5000);
    Serial.println("[*] Waiting... Broadcast 'PwnMe_Beacon' to receive the flag again.");
    flagDelivered = false;
    pBLEScan->start(5, false);
  }
}
