# Brant

Monitor for the Arduino Mega connected at `/dev/ttyUSB0`.

## Board

- **Hardware**: first-generation Arduino Mega — ATmega1280, identified by its
  FTDI FT232R USB chip (`0403:6001`). Later Megas (2560) use an ATmega16U2 and
  show up as `ttyACM*`. (`/dev/ttyACM0` on this machine is an unrelated LG
  monitor.)
- **Firmware on the board**: Si4735 radio project. Boots at **9600 baud** with:
  ```
  starting radio
   Resetting Si4735....done
   Powering Up..done
   Setting SPI...done
   Config IO......done
  Configuring Buttons....enter values
  31
  ```
  The sketch is **interactive**: after the boot banner it expects serial input
  (a value for the button/frequency configuration). Type while the monitor has
  focus.

## Usage

```sh
scripts/monitor                 # attach at 9600 baud on /dev/ttyUSB0
BRANT_BAUD=115200 scripts/monitor   # override baud
BRANT_PORT=/dev/ttyACM0 scripts/monitor  # override port
```

`Ctrl+C` to exit.

The script wraps `arduino-cli monitor -q --timestamp` and:

1. Installs the `arduino:avr` core on first run (its serial monitor plugin is
   required; it also bundles `avrdude`).
2. Uses the port directly when the shell has `dialout` access.
3. Falls back to `/usr/bin/sg dialout -c ...` when the group change hasn't
   taken effect in the current shell yet (your account is already in `dialout`;
   a fresh login removes the need for the fallback).

## Can we recover the sketch source from the board?

**No source.** Compiling C++ to AVR machine code is lossy; comments, names,
and structure are gone. But the **binary is fully recoverable** — see below.

**Recovery status (2026-08-25): done.** Full flash + EEPROM dumped to
`recovery/2026-08-25/` (128 KB flash, 4 KB EEPROM, disassembly, strings).
The firmware is an Si4735 radio app (AM/FM/SW/LW, freq knob, presets).

**Recipe** (proven; also in `recovery/2026-08-25/README.md`):

```sh
scripts/recover-flash    # -> recovery/<YYYY-MM-DD>/
```

The board's original 2009 bootloader (`ATmegaBOOT 090930`) speaks
**stk500v1 @ 57600** — not optiboot. avrdude's `-c arduino` programmer type
does its own DTR-reset dance and syncs reliably:

```sh
avrdude -C <conf> -p atmega1280 -c arduino -P /dev/ttyUSB0 -b 57600 \
  -U flash:r:dump.hex:i -U eeprom:r:eeprom.hex:i
```

Fuse reads return `0x0` over this bootloader (ATmegaBOOT doesn't implement
them); fuses need an ISP programmer on the ICSP header. The board is
bus-powered and dropped off USB once under load — prefer a powered hub.
