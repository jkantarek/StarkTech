# AGENTS.md

Working notes for this repo: what the project is, what tools exist, and the
hardware baseline. For humans and AI agents alike.

## Project

Playground for an **Arduino Mega** driving an **Adafruit NeoPixel 24-LED ring**,
plus the tools/steps needed to build, upload, monitor, and recover the board.

Primary sketch: `markJArmor/markJArmor.ino` (currently a minimal single-pixel
test). `haloHeartbeat/` holds an earlier heartbeat/chase effect (pre-GRBW fix).

## Board baseline (critical facts)

- **First-gen Arduino Mega — ATmega1280**, identified by its FTDI FT232R USB
  chip (`0403:6001`) → serial device **`/dev/ttyUSB0`**. (Later 2560 Megas use
  a 16U2 and appear as `ttyACM*`; `ttyACM0` here is an unrelated LG monitor.)
- **Bootloader: original `ATmegaBOOT / Arduino Mega - 090930`**, which speaks
  **stk500v1 @ 57600 baud**. It is NOT optiboot.
  - Consequently `arduino-cli upload` **cannot sync** (it defaults to the
    `wiring`/stk500v2 @ 115200 programmer). All flashing goes through raw
    **avrdude** with `-c arduino -b 57600`.
- Serial monitor baud for our sketches: **9600**.
- NeoPixel ring is **24 LEDs**, WS2812-family, **RGBW** → must construct with
  `NEO_GRBW + NEO_KHZ800` (using `NEO_GRB` produces a broken/misfiring ring —
  this was the "brown-out-like" bug that took a while to pin down).

## Tooling on the host

- `arduino-cli` 1.5.1 — compile (`arduino:avr:mega` FQBN) and `monitor`.
- `avrdude` 8.0.0 bundled with `arduino:avr` core at
  `~/.arduino15/packages/arduino/tools/avrdude/8.0.0-arduino1/bin/avrdude`.
- Python + pyserial for fixture-style serial capture/reset (baud sweeps, DTR
  pulse reset), at `~/.asdf/installs/python/3.11.11/bin/python`.

### Scripts

- `scripts/upload` — compiles `markJArmor` and flashes to the Mega via avrdude
  (`-c arduino -b 57600`). The canonical "flash it" command.
- `scripts/monitor` — `arduino-cli monitor` at 9600 with timestamps, wraps the
  `dialout`-group fallback.
- `Brant/scripts/recover-flash` — dumps flash/EEPROM/fuses (flash only if a
  bootloader is present) to `Brant/recovery/<date>/`, then builds disassembly
  + strings. Read-only; uses the same avrdude sync recipe.
- `Brant/` — notes + a dated recovery dump of the board's original Si4735
  radio firmware (flash, eeprom, fuses, disassembly, strings) taken before it
  was repurposed.

## Key gotchas (learned the hard way)

1. **`sg` is shadowed** by an ast-grep shim on this machine — always call
   `/usr/bin/sg`, never bare `sg`.
2. **Port access**): `/dev/ttyUSB0` is `root:dialout`. The user is already in
   `dialout`, but a session opened before the group change still lacks it →
   scripts re-exec via `/usr/bin/sg dialout -c ...`.
3. **Flash-write op spelling**: avrdude needs `-U flash:w:file:i` (the `w:`
   write op is required; omitting it makes avrdude fail "not readable").
4. **Disassembly naming**: AVR constant-prop gets applied liberally; the
   bootloader region begins at `0x1f000` in `Brant/recovery/.../flash.dis`.
5. **Memory**: sketch + bootloader share the 128 KB flash; fuse reads return
   `0x0` through ATmegaBOOT (it doesn't implement fuse access) — those
   fusings are invisible over the serial bootloader.

## Workflow

1. Build+flash: `~/Development/StarkTech/scripts/upload`
2. Any output/observer: `~/Development/StarkTech/scripts/monitor`
3. Recover firmware: `Brant/scripts/recover-flash`