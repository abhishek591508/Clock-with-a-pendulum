# Analog Clock with Pendulum

A simple **Computer Graphics** mini project in C: an analog clock with real-time hands and a physics-based swinging pendulum.

## Features

- Live clock (hour, minute, second hands)
- Pendulum motion using simple harmonic motion: θ = A·cos(√(g/L)·t)
- Classic drawing algorithms (no built-in `line()` / `circle()` for main shapes)

## CG Concepts Used

| Concept | Usage |
|--------|--------|
| Bresenham's line algorithm | Hands, tick marks, pendulum rod |
| Midpoint circle algorithm | Clock dial |
| 2D rotation | Hand angles from system time |

## Requirements

- **Windows** with **Code::Blocks** and **MinGW**
- **WinBGIM** (`graphics.h`, `libbgi.a`) installed in your MinGW `include` and `lib` folders

## How to Run

1. Open `clock_pendulum.cbp` in Code::Blocks (or add `clock_pendulum.c` to a new project).
2. Link these libraries: `bgi`, `gdi32`, `comdlg32`, `uuid`, `oleaut32`, `ole32`
3. **Build → Build and run** (F9)
4. Press **any key** to exit

## Files

- `clock_pendulum.c` — source code
- `clock_pendulum.cbp` — Code::Blocks project file

## Optional Tweaks

- **Smoother animation:** reduce `delay(50)` (e.g. `delay(16)`)
- **Faster/slower pendulum:** change `pend_len` or `PX_TO_M` in the source
