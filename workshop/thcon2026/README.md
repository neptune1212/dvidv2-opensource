# THCON 2026 - DVID/WOCSA Bluetooth Workshop

This workshop takes place at ENSEIHT in Toulouse for THCon 2026.

Many thanks to the WOCSA team for leading this workshop.

## Credits

This workshop is in collaboration with:
* DVID: https://dvid.eu
* WOCSA: https://wocsa.org

## Introduction

This workshop introduces the security concepts of **Bluetooth Low Energy (BLE)** through 9 hands-on challenges. Each challenge is served by an ESP32 microcontroller running a custom firmware. You will progressively move from basic BLE reconnaissance to advanced exploitation techniques.

### Required Tools

| Tool | Purpose | Install |
|------|---------|---------|
| **nRF Connect** (mobile) | BLE scanner & GATT browser | Android / iOS |
| **bluetoothctl** | Linux BLE CLI | `apt install bluez` |
| **gatttool** | GATT read/write CLI | `apt install bluez` |
| **Python 3 + bleak** | BLE scripting | `pip install bleak` |
| **Wireshark** | PCAP analysis (challenge 8) | https://www.wireshark.org |
| **hciconfig / hcitool** | Adapter management & spoofing | `apt install bluez` |

### Required Hardware

* A Linux laptop with a Bluetooth 4.0+ adapter (or USB dongle)
* One ESP32 board per challenge (pre-flashed)

---

## Challenge Overview

| # | Name | Tier | Difficulty | Key Concept |
|---|------|------|-----------|-------------|
| 01 | [GATT Enumeration](./01_gatt_enumeration/) | Beginner | ⭐ | Read a GATT characteristic |
| 02 | [Hidden in Plain Sight](./02_hidden_in_plain_sight/) | Beginner | ⭐ | Inspect raw advertisement data |
| 03 | [State Manipulation](./03_state_manipulation/) | Beginner | ⭐⭐ | Write to unlock a protected characteristic |
| 04 | [Identity Theft](./04_identity_theft/) | Intermediate | ⭐⭐⭐ | MAC address spoofing |
| 05 | [Needle in a Haystack](./05_needle_in_haystack/) | Intermediate | ⭐⭐⭐ | Automated BLE brute-force |
| 06 | [Role Reversal](./06_role_reversal/) | Intermediate | ⭐⭐⭐ | Broadcast a spoofed BLE advertisement |
| 07 | [Weak Link](./07_weak_link/) | Advanced | ⭐⭐⭐⭐ | BLE PIN brute-force |
| 08 | [Replay Attack](./08_replay_attack/) | Advanced | ⭐⭐⭐⭐ | PCAP analysis & traffic replay |
| 09 | [Buffer Overflow](./09_buffer_overflow/) | Advanced | ⭐⭐⭐⭐⭐ | Fuzzing & input validation bypass |

---

## BLE Concepts Recap

### Architecture

```
Peripheral (ESP32)
└── GATT Server
    └── Service  (UUID: identifies a feature domain)
        ├── Characteristic A  (UUID + Properties: READ, WRITE, NOTIFY...)
        │   └── Value (the actual data bytes)
        └── Characteristic B
```

### Key Terms

* **Advertisement**: Periodic broadcast from a peripheral. Contains device name, UUIDs, manufacturer data.
* **GATT**: Generic Attribute Profile. Defines how data is organized and accessed over BLE.
* **Service**: A logical grouping of characteristics.
* **Characteristic**: A typed data slot. Has a UUID, properties (READ/WRITE/NOTIFY), and a value.
* **UUID**: Universally Unique Identifier. 16-bit (standard) or 128-bit (custom).

---

## Setup: Flashing the ESP32

Each challenge folder contains an `ESP32/` directory with an `.ino` sketch.

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Add ESP32 board support: `File > Preferences > Additional Board URLs`:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. Install `esp32` board package via `Tools > Board > Board Manager`
4. Open the `.ino` file, select `ESP32 Dev Module` as board
5. Flash via USB (hold BOOT button if needed)

---

## Flag Format

All flags follow the format: `WOCSA{...}`
