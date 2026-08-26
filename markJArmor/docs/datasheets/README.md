# Chip Reference: ATmega640/1280/1281/2560/2561

**Local datasheet** — the authoritative reference for every microcontroller an
Arduino Mega board can carry. Do not rely on memory for register semantics;
read it here first.

## Document

- `Atmel-2549-ATmega640-1280-1281-2560-2561-Complete-Datasheet.pdf`
  (435 pages, 8.8 MB, PDF 1.6)
- Title/metadata: "ATmega640/1280/1281/2560/2561 datasheet", Atmel
  Corporation, created 2014-02-18 (FrameMaker 10.0.2 / Acrobat Distiller).
- SHA-256:
  `89db28704377bf06f9bbf3006048e1a5e320a8865616c626234b38bb06c86b02`
- Downloaded 2026-08-26 from Microchip's official CDN:
  `https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2549-8-bit-AVR-Microcontroller-ATmega640-1280-1281-2560-2561_Datasheet.pdf`
  (HTTP 200, `application/pdf`, no redirects). Microchip's product page
  (www.microchip.com/en-us/product/ATmega2560) is behind an Akamai bot-block;
  the datasheet CDN itself is directly fetchable and is the canonical file.
  Verified identical at the alternative canonical slug
  `…/en/devicedoc/Atmel-2549-…-datasheet.pdf` (same 8,823,111 bytes).

## What it covers

One family datasheet serves all four relevant MCUs:

| MCU | Arduino board |
|---|---|
| ATmega1280 | Arduino Mega (first generation) |
| ATmega2560 | Arduino Mega 2560 / Mega ADK |

The active FQBN is `arduino:avr:mega` ("Arduino Mega or Mega 2560"), which
the `arduino:avr` core maps to ATmega2560 by default.

## Register semantics we actually use

### MCUSR — MCU Status Register (Reset Flags), Section 12-9-1, page 64

Reset source latch. Per the datasheet's own bit table:

| Bit | Flag | Reset source |
|---|---|---|
| 4 | JTRF | JTAG Interface Reset |
| 3 | WDRF | Watchdog System Reset |
| 2 | BORF | Brown-out Reset |
| 1 | EXTRF | External Reset (RESET pin / USB-DTR) |
| 0 | PORF | Power-on Reset |

Datasheet guidance (page 64): read MCUSR **then clear it** as early as
possible in the program to identify the reset cause on a later boot.

## Search tips

Text extraction is lossless for this doc (it's a vector PDF):

```sh
pdftotext reference.pdf - | grep -i "MCUSR"
```

- Register Summary: page 399 (per the document's own cross-reference).
- RESET and Clock Control chapter: Section 12, starts ~page 61.
- Watchdog prescaler table: Table 12-2, page 66.