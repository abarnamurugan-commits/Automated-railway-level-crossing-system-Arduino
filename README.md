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

## Repo Structure
```
railway-crossing-arduino/
├── README.md
├── src/
│   └── railway_crossing.ino   # main Arduino sketch
├── docs/
│   └── explainer.md           # plain-language walkthrough for the team
└── media/
    └── circuit-screenshot.png # Tinkercad circuit diagram
```

## Status
- [x] Tinkercad circuit built
- [ ] Arduino sketch (logic)
- [ ] Team explainer doc
- [ ] Demo run-through
