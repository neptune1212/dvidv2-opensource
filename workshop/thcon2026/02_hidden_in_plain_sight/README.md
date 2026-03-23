# Challenge 02 - Hidden in Plain Sight

**Tier:** Beginner
**Difficulty:** ⭐
**Category:** Bluetooth Low Energy

## Objective

Find the flag without ever connecting to the device. The data is hiding in the BLE advertisement packets themselves.

## Concept

BLE devices broadcast **advertisement packets** before any connection is established. These packets can carry arbitrary data in fields like **Manufacturer Specific Data** or **Service Data**. This challenge teaches passive reconnaissance — useful data can be exfiltrated simply by observing the air.

## Device Information

| Field | Value |
|-------|-------|
| BLE Device Name | `THCON26_BLE_02` |
| Hint | You don't need to connect. Look at the raw advertisement payload. |

## Tools

* **nRF Connect** (mobile app) — inspect raw advertisement data
* **hcitool lescan** + **hcidump** (Linux CLI)
* **btlejuice** / **bettercap** — advanced passive sniffing

## Useful Commands

### Passive scan with hcitool
```bash
sudo hcitool lescan --passive
```

### Dump raw advertisement packets
```bash
sudo hcidump -X | grep -A 20 "THCON26_BLE_02"
```

### Alternative: bettercap
```bash
sudo bettercap
> ble.recon on
> ble.show
```
