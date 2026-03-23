# Writeup - Challenge 06: Role Reversal

**Difficulty:** ⭐⭐⭐ Intermediate
**Category:** Bluetooth Low Energy
**Flag:** `WOCSA{you_are_the_peripheral_now}`

---

## 🔍 Step 1: Understand the Setup

Unlike previous challenges, **you don't scan for the ESP32 here**. The ESP32 is the Central — it's scanning for you. Your job is to broadcast a BLE advertisement with the name `PwnMe_Beacon`.

Once the ESP32 detects your beacon, it connects to your device and writes the flag to a characteristic, or prints it over Serial.

---

## 📡 Step 2: Broadcast as PwnMe_Beacon

### Option A — Python (recommended)

Use `bleak` to create a simple BLE peripheral:

```python
import asyncio
from bleak import BleakScanner
from bleak.backends.device import BLEDevice

# Note: BLE advertising from Python requires bless (not bleak)
# pip install bless
from bless import BlessServer, BlessGATTCharacteristic, GATTCharacteristicProperties, GATTAttributePermissions

FLAG_CHAR_UUID = "deadbeef-0000-1000-8000-00805f9b34fb"
SERVICE_UUID   = "deadbeef-0001-1000-8000-00805f9b34fb"

async def main():
    server = BlessServer(name="PwnMe_Beacon")

    # Add a WRITE characteristic for the ESP32 to write the flag to
    await server.add_new_service(SERVICE_UUID)
    char_flags = (
        GATTCharacteristicProperties.write |
        GATTCharacteristicProperties.read
    )
    permissions = (
        GATTAttributePermissions.readable |
        GATTAttributePermissions.writeable
    )
    await server.add_new_characteristic(
        SERVICE_UUID, FLAG_CHAR_UUID, char_flags, None, permissions
    )

    await server.start()
    print("[*] Broadcasting as PwnMe_Beacon...")
    print("[*] Waiting for ESP32 to connect and deliver the flag...")

    # Wait for a write event
    trigger = asyncio.Event()

    def read_request(characteristic: BlessGATTCharacteristic, **kwargs):
        return characteristic.value

    def write_request(characteristic: BlessGATTCharacteristic, value, **kwargs):
        characteristic.value = value
        print(f"\n[!!!] FLAG RECEIVED: {value.decode()}")
        trigger.set()

    server.read_request_func = read_request
    server.write_request_func = write_request

    await trigger.wait()
    await server.stop()

asyncio.run(main())
```

```bash
pip install bless
python3 beacon.py
```

### Option B — Linux CLI (bluetoothctl)

```bash
sudo bluetoothctl
[bluetooth]# power on
[bluetooth]# discoverable on
[bluetooth]# set-alias PwnMe_Beacon
[bluetooth]# advertise on
```

---

## 📋 Step 3: Wait for the ESP32 to Connect

Once `PwnMe_Beacon` is visible, the ESP32 will:
1. Stop scanning
2. Connect to your device
3. Write the flag to the characteristic (UUID `deadbeef-0000-1000-8000-00805f9b34fb`)
4. Print the flag to its Serial output

Your Python script will output:
```
[*] Broadcasting as PwnMe_Beacon...
[*] Waiting for ESP32 to connect and deliver the flag...

[!!!] FLAG RECEIVED: WOCSA{you_are_the_peripheral_now}
```

---

## 🎯 Flag

```
WOCSA{you_are_the_peripheral_now}
```

---

## Key Takeaways

* BLE devices can act as either **Central** (scanner/initiator) or **Peripheral** (advertiser/responder)
* Any host with a BLE adapter can broadcast arbitrary advertisement data — name, UUIDs, manufacturer data
* "Spoofing" a beacon is trivial: just broadcast the expected name or UUID
* This technique is used in real attacks against devices that trigger actions upon detecting specific beacons (proximity unlocking, automation systems, etc.)
