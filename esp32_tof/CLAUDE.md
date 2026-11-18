# VL6180X sensor readout — ESP32 port

## Goal
Port the existing Teensy 4.1 VL6180X firmware (repo `Active_Monocopter_INDI`,
branch `snail_compute`, files `include/tof_sensors.h` + `src/tof_sensors.cpp`)
to an ESP32 so the ESP32 reads the 5 ToF sensors itself. Behaviour, function
names, output formats and calibration logic must stay the same as the Teensy
version; only the board-specific parts (pins, Wire setup, printf types) change.
Keep it simple and readable: this is for bench testing and noise characterisation.

## Hardware
- Board: Seeed Studio XIAO ESP32-S3, powered over USB-C (native USB serial).
- Sensors: 5x VL6180X ToF modules. Pins on the module, in order:
  VCC, GND, SCL, SDA, INT, SHUT. (SHUT = the chip-enable / shutdown pin,
  LOW = off. The code calls it CE.)
- Shared rails:
  - 3.3 V rail -> all sensor VCC (from the XIAO 3V3 pin)
  - GND rail -> all sensor GND
  - SDA rail -> all sensor SDA, XIAO D4 (GPIO5, default SDA)
  - SCL rail -> all sensor SCL, XIAO D5 (GPIO6, default SCL)
- Individual SHUT wires (one per sensor, NOT shared):
  - s0 -> D0 (GPIO1), s1 -> D1 (GPIO2), s2 -> D3 (GPIO4),
    s3 -> D8 (GPIO7), s4 -> D9 (GPIO8)
  - D2 (GPIO3) is skipped because it is an ESP32-S3 strapping pin.
  - D6/D7 (GPIO43/44, hardware UART TX/RX) are left free for a future serial
    link to the Teensy.
  - D8-D10 are also the SPI pins. If the microSD add-on is used later, move
    s3/s4 to other pins.
- INT pins unconnected.
- All sensors power up at the same default I2C address 0x29.
- ESP32 I/O is 3.3 V only; sensors run from 3.3 V. Do not assume 5 V.

## Firmware structure (port 1:1 from the Teensy code)
`include/tof_sensors.h`: all settings as constants at the top:
- `TOF_NUM_SENSORS = 5`
- `TOF_CE_PINS[] = {D0, D1, D3, D8, D9}`. Use the XIAO `Dx` names from the
  board variant, not raw GPIO numbers, so the code matches the silkscreen.
- `TOF_SDA_PIN = D4`, `TOF_SCL_PIN = D5` (new: the ESP32 takes explicit I2C pins)
- `TOF_DEFAULT_ADDR = 0x29`, `TOF_BASE_ADDR = 0x30` (sensor i -> 0x30 + i)
- `TOF_I2C_CLOCK_HZ = 400000` (easy to drop to 100000)
- `TOF_TIMEOUT_MS = 100`, `TOF_INVALID = -1`, `TOF_SYNC_BYTE = 0xBB`
- Per-sensor calibration, `corrected_mm = raw_mm * scale + offset`:
  - `TOF_SCALE[] = {5.0, 2.0, 1.0, 1.0, 2.0}`
  - `TOF_OFFSET_MM[] = {0, -10, -10, 0, 0}`

  These are copied from the Teensy version: the ESP32 uses the same sensor
  modules on the same breadboard, so the calibration carries over. If the
  modules or their mounting change, recalibrate with a white card at a
  ruler-measured distance (e.g. 50 mm). Neutral values (scale 1, offset 0) are
  kept commented out in the header for that.

`src/tof_sensors.cpp`: same functions as the Teensy version:
- `initSensors()`:
  - `Wire.begin(TOF_SDA_PIN, TOF_SCL_PIN)`, then `Wire.setClock(TOF_I2C_CLOCK_HZ)`.
  - Drive all SHUT pins LOW and wait 10 ms. Then for each sensor in turn: SHUT
    HIGH, wait 2 ms, check that something answers at 0x29, then `setBus(&Wire)`,
    `init()`, `configureDefault()`, `setAddress(0x30 + i)` and `setTimeout()`.
  - Verify the sensor answers at its new address with model ID 0xB4
    (`IDENTIFICATION__MODEL_ID`). Pololu `init()` returns void, so this check is
    the only way to detect failure.
  - If a sensor fails, set its SHUT back to LOW (so it can't clash at 0x29 with
    the next sensor), print FAILED and continue. Never hang.
  - Print one line per sensor (`# sensor i (CE pin p): OK at 0x3i` or
    `FAILED, skipping`) plus a total, and return the number of sensors that are OK.
- `readSensor(i)`:
  - Single-shot `readRangeSingleMillimeters()`.
  - Return -1 on a timeout, a nonzero range status (`RESULT__RANGE_STATUS >> 4`),
    raw 255, or a sensor that failed init.
  - Otherwise return `lroundf(raw * TOF_SCALE[i] + TOF_OFFSET_MM[i])`.
- `printHeader()`: prints `time_ms,s0_mm,s1_mm,s2_mm,s3_mm,s4_mm`.
- `printSweep()`: one CSV line per sweep: `time_ms,s0,s1,s2,s3,s4`.
- `printReadings()`: one human-readable line per sweep, all 5 readings on ONE
  line: `t=1523 ms  s0: 87  s1: 92  s2: -1  s3: 110  s4: 95`.
  This is the default output for the serial monitor.
- `sendReadingsSerial1()`: on the Teensy, this sends a 15-byte binary packet
  to the ESP32. On the ESP32, keep the same packet layout but build it in a
  helper (e.g. `buildPacket(uint8_t out[15])`) so the same bytes can later be
  sent over Wi-Fi:
  `[0xBB][uint32 time_ms][int16 s0..s4 mm]`, little-endian, -1 = invalid.

Rules that apply to all of the above:
- Read sensors strictly one at a time; never range two at once (their cones
  overlap and interfere).
- Take the time with `millis()` at the start of each sweep.
- On the ESP32, print `uint32_t` with `%lu` and an `(unsigned long)` cast, to
  avoid printf format warnings.

`src/main.cpp`:
- `setup()`: `Serial.begin(115200)`, wait for Serial up to 3 s, then `initSensors()`.
- `loop()`: `printReadings()`, then `delay(20)`.

## Tooling
- PlatformIO on Ubuntu 20.04 (do not suggest upgrading the OS).
  `platformio.ini`:
  ```
  [env:xiao_esp32s3]
  platform = espressif32
  board = seeed_xiao_esp32s3
  framework = arduino
  monitor_speed = 115200
  lib_deps = pololu/VL6180X
  ```
- The S3 uses native USB for `Serial`. If nothing prints in the monitor, add
  `build_flags = -DARDUINO_USB_CDC_ON_BOOT=1`.
- If an upload fails or the port disappears, put the board in bootloader mode:
  hold BOOT, tap RESET, release BOOT, then upload again.
- Library: Pololu VL6180X. Use another library only if there is a clear reason,
  and say why.
- The user must be in the `dialout` group for the ESP32 USB serial port.

## Working with the user
- Ask before adding features, debug output or extra functions that were not
  requested. Keep changes to exactly what was asked.
- Don't remove existing code. If something must be disabled, comment it out.
- Output units are millimetres. The useful range is about 10-150 mm; touching
  the sensor or nothing in range gives -1.

## Later (not now)
- Send readings wirelessly from the ESP32 to a computer using the 15-byte packet
  above. Transport not decided yet (Wi-Fi UDP / TCP / ESP-NOW); ask the user.
- Timestamps per sensor reading, mandible angle input, microSD logging, faster
  continuous ranging for scan-while-closing.
