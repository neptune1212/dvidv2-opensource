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

### Linux CLI (bluetoothctl)

```bash
sudo bluetoothctl
[bluetooth]# power on
[bluetooth]# discoverable on
[bluetooth]# system-alias PwnMe_Beacon
[bluetooth]# advertise on
[bluetooth]# advertise.name on
```

---

## 📋 Step 3: Wait for the ESP32 to Connect

Once `PwnMe_Beacon` is visible, the ESP32 will:
1. Stop scanning
2. Connect to your device
3. Write the flag to the characteristic (UUID `deadbeef-0000-1000-8000-00805f9b34fb`)
4. Print the flag to its Serial output

The ESP32 will print the flag to its Serial output:
```
[!!!] FLAG DELIVERED: WOCSA{you_are_the_peripheral_now}
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
