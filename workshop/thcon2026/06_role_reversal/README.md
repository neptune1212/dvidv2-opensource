# Challenge 06 - Role Reversal

**Tier:** Intermediate
**Difficulty:** ⭐⭐⭐
**Category:** Bluetooth Low Energy

## Objective

Flip the script: this time, the ESP32 is the scanner. Make your computer advertise as `PwnMe_Beacon` and wait for the ESP32 to connect to you and deliver the flag.

## Concept

BLE devices are not always peripherals. A **Central** device scans and initiates connections. This challenge teaches how to broadcast and spoof BLE advertisements from a computer or mobile device, reversing the typical attacker/target relationship.

## Device Information

| Field | Value |
|-------|-------|
| ESP32 Role | **Central** (scanner) |
| What to broadcast | A BLE advertisement with local name `PwnMe_Beacon` |
| Expected behavior | ESP32 scans, finds your beacon, connects, writes the flag to your device |

## Tools

* **bluetoothctl** (Linux) — broadcast a custom advertisement
* **Python + bleak** — advertise as a peripheral
* **hciconfig + hcitool** — low-level advertisement broadcasting

## Useful Commands

### Broadcast as PwnMe_Beacon using bluetoothctl
```bash
bluetoothctl
[bluetooth]# advertise on
[bluetooth]# set-alias PwnMe_Beacon
[bluetooth]# discoverable on
```

### Broadcast using hciconfig + hcitool
```bash
# Set device name
sudo hciconfig hci0 name PwnMe_Beacon

# Start LE advertising
sudo hcitool -i hci0 cmd 0x08 0x0008 \
  1e 02 01 06 11 09 50 77 6e 4d 65 5f 42 65 61 63 6f 6e 00 00 00 00 00 00 00 00 00 00 00 00 00
sudo hcitool -i hci0 cmd 0x08 0x000a 01
```
