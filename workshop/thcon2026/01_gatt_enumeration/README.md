# Challenge 01 - GATT Enumeration

**Tier:** Beginner
**Difficulty:** ⭐
**Category:** Bluetooth Low Energy

## Objective

Retrieve the flag by exploring the BLE device. Connect to the ESP32, enumerate its services and characteristics, and read the flag.

## Concept

This challenge teaches the fundamental architecture of Bluetooth Low Energy: **Services**, **Characteristics**, and **UUIDs**. You will use basic inspection tools to navigate the GATT tree and issue a `READ` request.

## Device Information

| Field | Value |
|-------|-------|
| BLE Device Name | `THCON26_BLE_01` |
| Hint | The flag is stored in plain text inside a characteristic |

## Tools

* **nRF Connect** (mobile app) — recommended for beginners
* **bluetoothctl** + **gatttool** (Linux CLI)

## Useful Commands

### Scan for the device
```bash
bluetoothctl scan on
```

### Connect and list services
```bash
gatttool -b <MAC_ADDRESS> -I
> connect
> primary
> characteristics
```

### Read a characteristic
```bash
> char-read-hnd <handle>
```
