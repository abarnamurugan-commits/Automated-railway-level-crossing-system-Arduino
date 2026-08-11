# Explainer 

Hi, thanks for looking at our submission. Here's what we built and why we built it this way.

## What this is

We built an Automated Railway Level Crossing System for Track 2 — Tinkercad/Arduino. It's a working Arduino simulation that detects a train approaching, closes the gate, warns road users with lights and sound, blocks traffic while the train crosses, and reopens the gate automatically once the train has safely passed.

## How to see it work

Open the Tinkercad circuit and hit simulate. Press the **approach button** — you'll see the red LED and buzzer activate, the LCD update to show the warning, and after a short delay the gate servo swings closed. While the gate is closed, you can press the approach button again as many times as you like — nothing happens, because the system is designed to only respond to the **departure button** while a train is on the crossing. Press departure, and you'll see a brief safety-check pause on the LCD before the gate reopens and the system resets to normal.

## Why we made the choices we did

**The state machine has six states**, not just "open" and "closed." We did this on purpose — `NORMAL`, `TRAIN_APPROACHING`, `GATE_CLOSING`, `TRAIN_CROSSING`, `TRAIN_CLEARED`, `GATE_OPENING`. Splitting it out this way makes each phase's behavior explicit and testable, and it's what let us build in the safety guard: **the `TRAIN_CROSSING` state can only be exited by the departure sensor.** We treated that as the most important safety property in the whole system — repeated or accidental approach signals while a train is present must never reopen the gate early.

**We added a 16x2 LCD** on top of the base LED/buzzer signaling, because we wanted status information that's readable at a glance without needing to open the Serial Monitor. Every state writes a two-line status message to it, so you can watch the full sequence play out just by reading the screen.

**We also built a small software layer** — a Python backend and a React dashboard — that reproduces the exact same state machine logic in a web interface. We want to be upfront about this: **it is not live-connected to the Tinkercad circuit.** Tinkercad doesn't support streaming a running simulation's state out to an external app without extra bridging hardware, which is outside the scope of a software-only submission. So we built it as a second, independent implementation of the same design, to show the logic in a format that's easy to browse. We consider it a bonus feature, not the core deliverable — the Arduino simulation is the thing that actually satisfies the problem statement on its own.

## Quick feature checklist, if it helps

- Detects train arrival and departure — via approach/departure buttons standing in for IR/ultrasonic sensors
- Closes the gate automatically before the train reaches the crossing
- Warns with red/green LEDs and a buzzer
- Blocks road traffic for the full duration the gate is closed
- Reopens the gate automatically once the train has departed
- Shows live status on the LCD and Serial Monitor
- Handles repeated/rapid button presses safely, without unsafe reopening

Happy to walk through the code or answer anything live — thanks for your time.

## Where everything lives

See `ARCHITECTURE.md` at the repo root for the technical breakdown, and
`src/railway_crossing.ino` for the actual Arduino code this is all based on.
