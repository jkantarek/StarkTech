# Recovery — 2026-08-25

Full flash/EEPROM dump of the Brant Mega (ATmega1280) performed 2026-08-25.

## Files

| File | Contents |
|---|---|
| `flash.hex` | Full 128 KB flash in Intel HEX (131,072 bytes, read 100%) |
| `flash.bin` | Same flash as raw binary |
| `flash.elf` | Flash converted to an ELF object (for disassembly) |
| `flash.dis` | Full disassembly (`avr-objdump -D -m avr5`) |
| `strings.txt` | Printable strings (`strings -n 4`) |
| `eeprom.hex` | EEPROM dump in Intel HEX (4 KB, the sketch's saved settings) |
| `lfuse.txt`/`hfuse.txt`/`efuse.txt`/`lock.txt` | Fuse/lock reads — **all report `0x0`, see limitation** |
| `avrdude.log` | Full avrdude session log |

## What the firmware is

Si4735 DSP radio project, confirmed by strings in `flash.bin` and the live boot
banner: AM/FM + shortwave (`sw`) / longwave (`lw`), frequency knob, presets
("Getting Previous Settings", "found in memory"), volume ("Volume saved as"),
seek bands. The sketch prints its banner at **9600 baud** on the main UART and
then waits for serial input.

The dump also contains the bootloader string
`ATmegaBOOT / Arduino Mega - (C) Arduino LLC - 090930` — the original
2009-09-30 bootloader build.

## Procedure (repeatable)

The discovery took three steps; the final recipe is **one command**:

```sh
~/Development/StarkTech/Brant/scripts/recover-flash
```

Writes to `Brant/recovery/<YYYY-MM-DD>/` (reads only, never writes the chip).

### What was tried and what worked

1. **Baud sweep + DTR pulses** — found the sketch's 9600 baud, but no
   bootloader sync. The sketch's own output bytes (`0x6f` `'o'`, `0x73` `'s'` =
   "st"arting radio) came back to stk500 sync attempts — the chip never entered
   the bootloader with pre-connect DTR pulses.
2. **Manual RESET dance** (press-hold, release mid-connect) — proved a
   bootloader exists and identified the protocol: **stk500v1 @ 57600** (not
   optiboot's stk500v2 @ 115200; not ATmegaBOOT's sometimes-57600-v2 variant).
   Signature: `1E 97 03 (ATmega1280)`.
3. **`-c arduino` programmer type** — avrdude's built-in stk500v1 + DTR-reset
   dance syncs on its own, no external pulse, no manual button. This is the
   working recipe: `arduino` programmer, 57600 baud.

`-c stk500v2`, `-c stk500v1` with pre-pulses, and external DTR pulses during
the read all fail (external python port opens re-baud the tty to 115200 and
desync the 57600 read).

## Limitations

- **Fuse/lock reads return `0x0`** — ATmegaBOOT does not implement fuse reads;
  the values are zeros from the bootloader, not the chip's real fuses. Reading
  fuses requires a hardware ISP programmer on the ICSP header.
- **The 09:00 disconnect** — during initial probing the board dropped off USB
  (`usb 1-4: USB disconnect` at 06:55) and needed a physical replug. Board is
  bus-powered (FTDI + radio under load). A powered hub or external PSU is
  recommended before any long upload session.
- **Source code is not recoverable from this dump.** `flash.dis` gives the
  machine code behavior; `strings.txt` the string table; but comments, names,
  and structure are lost in compilation. If the original `.ino` is needed,
  search backups/git; the code will likely have to be reverse-engineered from
  `flash.dis` or rewritten from the strings.
- Flash was read with the sketch still running; the bootloader replaced it
  only during the read window. Nothing was written to the chip.