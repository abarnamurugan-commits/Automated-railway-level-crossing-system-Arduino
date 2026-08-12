# Automated Railway Level Crossing System

**Track 2 — Tinkercad (Arduino)**

## Team Information

| Field | Details |
|---|---|
| Team Name | Nexora |
| Team Number | 44 |
| Team Members | M. Abarna (Team Lead)
R. Hemalatha 
A. Myeisha
K. Nethra
K. Srima
S. Nithya Lakshmi |

## Problem Statement

Railway level crossings are critical points where railway tracks intersect with roads. Manual gate operation can cause delays or accidents. This project automates that process: detecting an approaching train, closing the gate, warning road users, and safely reopening the gate once the train has passed.

## Solution Overview

An Arduino Uno circuit (simulated in Tinkercad) runs a finite state machine that manages the full lifecycle of a railway crossing — from normal operation, through train approach and gate closure, to safe reopening after departure. The system exposes its status through a servo-driven gate, LED/buzzer warnings, a 16x2 LCD, and Serial Monitor logging.

A parallel software demo layer (Python backend + React frontend) mirrors the same state machine logic for presentation purposes outside the simulator.

## Tinkercad Simulation

🔗 **Live circuit:** [Automated Railway Level Crossing — Tinkercad](https://www.tinkercad.com/things/7dJf7NdIG3R-automated-railway-level-crossing-system?sharecode=dy-jZx3d7Z0WLYDDXqx9CKfPAcqPZOhexVePSrBiWsQ)

Open the link, click **Start Simulation**, and use the pushbuttons to simulate a train arriving/departing to see the full gate → warning → reopen sequence.

## Architecture

```
Hardware layer (Tinkercad)          Software demo layer
─────────────────────────           ────────────────────
Sensors (buttons)                   Backend (Python)
   │                                  state_machine.py
   ▼                                  main.py
State machine (.ino)
   │
   ▼
Actuators                           Frontend (React/Vite)
(Servo, LEDs, Buzzer, LCD)             App.jsx — status view
```

**Note:** The hardware layer and software demo layer are **intentionally not live-connected**. Tinkercad simulations cannot stream live state to an external web app without additional bridging hardware, which is outside the software-only scope of this track. The backend/frontend instead reproduce the identical state machine logic independently, as a demonstration layer that shows the same operational sequence in a web format — a bonus feature (see "Innovation and additional features" in the evaluation criteria) that sits alongside, not on top of, the core Arduino solution.

The Arduino simulation in Tinkercad is the primary deliverable and satisfies every required feature on its own — the dashboard is supplementary.

## Tech Stack

| Layer | Technology |
|---|---|
| Hardware simulation | Arduino Uno (Tinkercad), C++/Arduino, `Servo.h`, `LiquidCrystal.h` |
| Backend demo | Python |
| Frontend demo | React + Vite |

## State Machine

```
NORMAL → TRAIN_APPROACHING → GATE_CLOSING → TRAIN_CROSSING → TRAIN_CLEARED → GATE_OPENING → NORMAL
```

| State | Behavior |
|---|---|
| `NORMAL` | Gate open, green signal on, waiting for train |
| `TRAIN_APPROACHING` | Red signal + buzzer on, warning period before closing |
| `GATE_CLOSING` | Servo moves gate to closed position |
| `TRAIN_CROSSING` | Gate locked closed; only the departure sensor can exit this state — repeated approach signals are safely ignored |
| `TRAIN_CLEARED` | Brief safety check delay, buzzer stops |
| `GATE_OPENING` | Servo returns gate to open position, system resets to `NORMAL` |

## Hardware Components

- Servo motor — gate arm
- Red / Green LEDs — traffic signal
- Buzzer — audible warning
- 2x pushbuttons — simulate train arrival/departure sensors
- 16x2 LCD — real-time two-line status text (e.g. "Status: NORMAL / Waiting train..") updated on every state transition
- Serial Monitor — secondary status/debug output

## Feature Checklist

| Required Feature | Status |
|---|---|
| Detect train arrival/departure | ✅ |
| Auto-close gate before train arrives | ✅ |
| Visual and audible warnings | ✅ |
| Prevent road traffic during crossing | ✅ |
| Auto-reopen gate after train passes | ✅ |
| Display current operational status | ✅ 16x2 LCD (live two-line status text per state) + Serial Monitor (verbose log) |
| Handle repeated crossing requests safely | ✅ state-locked during crossing |

## Known Limitations / Future Work

- Current version uses blocking `delay()` calls during transitions, meaning input can't be read during those windows. A `millis()`-based non-blocking version is planned to improve responsiveness and safety.
- No live bridge between the physical/simulated circuit and the software dashboard — the dashboard is a logic-equivalent demo, not a live mirror.
- Sensors are simulated via pushbuttons in place of physical IR/ultrasonic modules, consistent with the software-only hackathon scope.

## Repository Structure

```
railway-crossing-arduino/
├── backend/          # Python state machine mirror + demo server
├── frontend/          # React dashboard (status demo)
├── docs/
│   └── explainer.md  # Project explainer for judges
├── src/
│   └── railway_crossing.ino   # Arduino sketch (Tinkercad)
└── ARCHITECTURE.md
```
