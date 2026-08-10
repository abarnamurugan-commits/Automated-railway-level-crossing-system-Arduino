# Automated Railway Level Crossing System (Arduino / Tinkercad)

**Track 2 — Tinkercad (Arduino)**

## Problem Statement
Design an automated railway crossing that detects an approaching train,
closes the gate, warns road users, and reopens safely after the train passes.

## Hardware (simulated in Tinkercad)
| Component | Role |
|---|---|
| Arduino Uno | Controller |
| Servo motor | Gate barrier (open/close) |
| Push Button 1 | Train arrival sensor |
| Push Button 2 | Train departure sensor |
| Red LED | "Stop / Gate closed" warning |
| Green LED | "Go / Gate open" status |
| Buzzer | Audible warning during crossing |

## Circuit
See `media/circuit-screenshot.png` and the live Tinkercad link:
https://www.tinkercad.com/things/hqiDRzDUJyR-smooth-curcan?sharecode=gX1ZWKE7MTx6bXxJbYkOelXysnAqlUEK97XsrJ6DzNM

## Architecture
Full breakdown of how the Arduino, backend, and frontend fit together —
and why a software layer exists at all for an Arduino problem — is in
[`ARCHITECTURE.md`](ARCHITECTURE.md). Read that first if you're new to the repo.

## Repo Structure
```
railway-crossing-arduino/
├── README.md
├── ARCHITECTURE.md            # start here — how everything connects
├── src/
│   └── railway_crossing.ino   # main Arduino sketch (source of truth for the logic)
├── backend/
│   └── app/                   # FastAPI mirror of the same state machine + live API
├── frontend/
│   └── src/                   # React dashboard consuming the backend
├── docs/
│   └── explainer.md           # plain-language walkthrough for the team
└── media/
    └── circuit-screenshot.png # Tinkercad circuit diagram
```

## Running it locally
```
# Backend
cd backend
pip install -r requirements.txt
uvicorn app.main:app --reload

# Frontend (separate terminal)
cd frontend
npm install
npm run dev
```
Open http://localhost:5173, click "Simulate train."

## Status
- [x] Tinkercad circuit built
- [x] Arduino sketch (logic)
- [x] Backend (FastAPI + SQLite)
- [x] Frontend dashboard (React)
- [x] Live ETA countdown
- [ ] Team explainer doc
- [ ] Demo run-through

