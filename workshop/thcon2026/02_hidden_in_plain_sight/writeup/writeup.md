# Writeup - Challenge 02: Hidden in Plain Sight

**Difficulty:** ⭐ Beginner
**Category:** Bluetooth Low Energy
**Flag:** `WOCSA{adv_data_leaks_secrets}`

---

## 🔍 Step 1: Passive Scan for BLE Advertisements

We need to observe raw advertisement packets without connecting to the device.

### Option A — nRF Connect (mobile)

1. Tap **SCAN**
2. Find `THCON26_BLE_02` in the list
3. Tap on the device name to expand details
4. Look for **Manufacturer Specific Data** in the raw advertisement section

You will see hex bytes starting with `FF FF` (company ID), followed by the flag in ASCII.

### Option B — Linux CLI

```bash
sudo hcitool lescan --passive &
sudo hcidump -X 2>/dev/null
```

Look for advertising events from `THCON26_BLE_02`. You will see the raw HCI data including a type `0xFF` field (Manufacturer Specific).

---

## 📦 Step 2: Decode the Manufacturer Specific Data

The raw bytes visible in the advertisement (after the `FF FF` company ID):

```
57 4F 43 53 41 7B 61 64 76 5F 64 61 74 61 5F 6C 65 61 6B 73 5F 73 65 63 72 65 74 73 7D
```

Decode:
```bash
echo "574f435341 7b6164765f646174615f6c65616b735f73656372657473 7d" | xxd -r -p
```

Or using Python:
```python
data = bytes.fromhex("574f43534137b6164765f646174615f6c65616b735f736563726574737d")
print(data.decode())
```

Output:
```
WOCSA{adv_data_leaks_secrets}
```

---

## 🎯 Flag

```
WOCSA{adv_data_leaks_secrets}
```

---

## Key Takeaways

* BLE advertisement packets are **broadcast in the clear** — anyone within range can capture them
* **Manufacturer Specific Data** (AD type `0xFF`) is a free-form field that applications often misuse to carry sensitive data
* Passive scanning (no connection required) is sufficient to exfiltrate this data
* Inspecting raw advertisement payloads is a critical first step in any BLE security assessment
