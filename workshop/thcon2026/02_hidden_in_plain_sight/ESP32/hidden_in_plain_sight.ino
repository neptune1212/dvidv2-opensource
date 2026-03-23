/*
 * THCON 2026 - DVID/WOCSA Bluetooth Workshop
 * Challenge 02 - Hidden in Plain Sight
 *
 * The ESP32 broadcasts a BLE advertisement containing the flag embedded
 * in the Manufacturer Specific Data field (company ID 0xFFFF).
 * No GATT server is running — participants must inspect raw adv packets.
 *
 * Flag: WOCSA{adv_data_leaks_secrets}
 */

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>

#define DEVICE_NAME "THCON26_BLE_02"

// Flag bytes to embed in Manufacturer Specific Data
// "WOCSA{adv_data_leaks_secrets}" as ASCII
static const uint8_t flagData[] = {
  0xFF, 0xFF,  // Company ID (0xFFFF = test/unregistered)
  // WOCSA{adv_data_leaks_secrets}
  0x57, 0x4F, 0x43, 0x53, 0x41, 0x7B,  // WOCSA{
  0x61, 0x64, 0x76, 0x5F, 0x64, 0x61,  // adv_da
  0x74, 0x61, 0x5F, 0x6C, 0x65, 0x61,  // ta_lea
  0x6B, 0x73, 0x5F, 0x73, 0x65, 0x63,  // ks_sec
  0x72, 0x65, 0x74, 0x73, 0x7D         // rets}
};

void setup() {
  Serial.begin(115200);
  Serial.println("[*] Challenge 02 - Hidden in Plain Sight");

  BLEDevice::init(DEVICE_NAME);

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();

  // Build custom advertisement data
  BLEAdvertisementData advData;
  advData.setName(DEVICE_NAME);

  // Add Manufacturer Specific Data (type 0xFF)
  String mfgData((char*)flagData, sizeof(flagData));
  advData.setManufacturerData(mfgData);

  pAdvertising->setAdvertisementData(advData);
  pAdvertising->setScanResponse(false);
  pAdvertising->start();

  Serial.println("[*] BLE advertising started");
  Serial.println("[*] Flag is hidden in Manufacturer Specific Data");
}

void loop() {
  delay(1000);
}
