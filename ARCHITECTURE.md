# Architecture

**SIH Internal Hackathon — Track 2 (Tinkercad / Arduino)**

## What this system does

A train approaches a level crossing. The system detects it, warns road users,
closes the gate, waits for the train to fully pass, then reopens safely —
handling repeated or overlapping detection events without unsafe behavior.

## Why there's a backend and frontend for an Arduino problem

The Tinkercad circuit is the physical simulation layer — it proves the wiring
and the Arduino logic work. The backend and frontend exist to demonstrate the
same logic as a live, observable software system: real-time state, a safety
log judges can inspect, and a dashboard that shows the system "thinking" —
not just a servo moving. All three layers implement the *identical* state
machine, so nothing shown on screen is invented separately from the circuit.

## The three layers

| Layer | Tech | Role |
|---|---|---|
| Hardware simulation | Tinkercad + Arduino Uno | Servo gate, 2 buttons (arrival/departure sensors), red/green LEDs, buzzer |
| Backend | FastAPI + SQLite | Same 6-state machine, event API, safety log, live WebSocket broadcast |
| Frontend | React + Vite | Status badge, animated gate, live ETA countdown, crossing log |

## State machine (shared across all three layers)

```
NORMAL
  -> TRAIN_APPROACHING   (red LED + buzzer on, ETA countdown starts)
  -> GATE_CLOSING        (servo travels to closed)
  -> TRAIN_CROSSING      (gate locked closed; repeated arrivals ignored)
  -> TRAIN_CLEARED       (departure debounced, safety-check pause)
  -> GATE_OPENING        (servo travels to open)
  -> NORMAL
```

Timings are calibrated to a measured Tinkercad run (arrival→closed 2.93s,
departure→open 2.98s), so the backend's scripted demo mode and the physical
simulation move at the same pace.

## Safety guarantees (evaluation criteria: safety & reliability)

- **Repeat-guard:** a second arrival is ignored while already mid-cycle; a
  departure is ignored unless the train is actually marked crossing.
- **Debounce:** a 0.3s delay after departure detection prevents a single
  button press from double-triggering.
- **Single source of truth:** the backend's state machine is a line-for-line
  mirror of the Arduino sketch's logic — not a re-guess.

## Data flow

```
Tinkercad button press (simulated)
        |
        v
Arduino state machine (src/railway_crossing.ino)
        |
        v   (mirrored logic, not a live serial link — see demo mode below)
FastAPI backend (backend/app/state_machine.py)
        |
        v
SQLite log  +  WebSocket broadcast
        |
        v
React dashboard (frontend/src/App.jsx)
```

## Demo mode

Tinkercad can't push live serial data to the backend during judging. Instead,
`POST /api/demo/start` fires the same arrival→departure sequence a real
Arduino would send, timed to the measured Tinkercad run. Click "Simulate
train" on the dashboard to trigger it — the badge, gate animation, ETA
countdown, and log all move together as if driven by the real circuit.

## Repo map

```
railway-crossing-arduino/
├── src/railway_crossing.ino   Arduino sketch (source of truth for the logic)
├── media/circuit-screenshot.png   Tinkercad circuit
├── backend/app/                FastAPI mirror of the state machine
├── frontend/src/                React dashboard
└── docs/explainer.md           Plain-language walkthrough for the team
```
