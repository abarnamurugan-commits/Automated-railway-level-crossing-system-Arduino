# Architecture

## Overview

This project has two independent layers that both express the same railway-crossing logic:

1. **Hardware layer** — the actual required deliverable: an Arduino Uno circuit simulated in Tinkercad.
2. **Software demo layer** — a Python backend and React frontend that mirror the same state machine logic in a web dashboard, as a bonus/innovation feature.

These layers are **not runtime-connected**. See "Why the layers are separate" below.

## Hardware Layer

### Pin Mapping

| Component | Arduino Pin |
|---|---|
| Train approach sensor (button) | D2 |
| Train departure sensor (button) | D3 |
| Gate servo | D9 |
| Red LED (stop signal) | D6 |
| Green LED (go signal) | D5 |
| Buzzer | D8 |
| LCD RS | D12 |
| LCD E | D11 |
| LCD D4 | D7 |
| LCD D5 | A0 |
| LCD D6 | A1 |
| LCD D7 | A2 |

LCD is wired in 4-bit parallel mode (`LiquidCrystal` library). LCD `VSS`→GND, `VDD`→5V, `V0`→potentiometer wiper (contrast), `RW`→GND, backlight anode → 5V through a 220Ω resistor, backlight cathode → GND.

### State Machine

The core logic is a 6-state finite state machine (`enum State` in `railway_crossing.ino`):

```
                 ┌───────────────────────────────────────────────┐
                 │                                                 │
                 ▼                                                 │
   ┌─────────┐  approach   ┌───────────────────┐   (2s)   ┌───────────────┐
   │ NORMAL  │────detected▶│ TRAIN_APPROACHING  │─────────▶│  GATE_CLOSING │
   └─────────┘             └───────────────────┘           └───────────────┘
        ▲                                                          │ (1s)
        │                                                          ▼
   ┌───────────────┐   (1s)    ┌───────────────┐  departure  ┌───────────────┐
   │  GATE_OPENING │◀──────────│  TRAIN_CLEARED │◀─detected───│ TRAIN_CROSSING│
   └───────────────┘   (1.5s)  └───────────────┘             └───────────────┘
```

Each state:
- Sets LED/buzzer/servo output for that phase
- Writes a two-line status message to the LCD
- Logs a verbose status block to Serial Monitor

**Safety guard:** while in `TRAIN_CROSSING`, the state machine only reacts to the departure sensor. Repeated or spurious signals from the approach sensor during this window are ignored by construction — the `switch` statement simply has no transition path out of `TRAIN_CROSSING` triggered by `approachDetected`.

**Known limitation:** state transitions currently use blocking `delay()` calls (2000ms / 1000ms / 1500ms / 1000ms across the cycle). During these windows, `loop()` cannot read input. A `millis()`-based non-blocking version is the planned next iteration — see README "Known Limitations."

## Software Demo Layer

```
backend/
├── app/
│   ├── state_machine.py   # Python re-implementation of the same 6-state logic
│   ├── main.py             # Entry point / demo server
│   └── db.py

frontend/
├── src/
│   ├── App.jsx              # Status dashboard UI
│   └── App.css
```

The Python `state_machine.py` independently reproduces the same states and transition rules as the `.ino` sketch. The React frontend renders whatever state the Python layer reports. Nothing here reads from the Tinkercad circuit — it's a standalone reproduction of the same design, used to demonstrate the logic in a browser-friendly form.

## Why the layers are separate

Tinkercad simulations run entirely inside Tinkercad's own sandboxed environment. There's no supported way to stream a running Tinkercad circuit's state to an external web server without extra bridging hardware (e.g. a real physical Arduino connected via USB serial to a local script) — which is outside the scope of a software-only hackathon submission. Building a second, independent implementation of the same logic in Python/React lets us demonstrate the same operational design in a format that's easy to browse and present, without overstating the integration as "live."

## Data / Control Flow Summary

| | Hardware layer | Software demo layer |
|---|---|---|
| Input | Physical/simulated buttons | Independent internal logic (no external input) |
| Processing | Arduino `.ino` state machine | Python `state_machine.py` |
| Output | Servo, LEDs, buzzer, LCD, Serial | React dashboard UI |
| Connected to each other? | No | No |