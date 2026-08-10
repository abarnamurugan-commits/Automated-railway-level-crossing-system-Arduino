# Explainer — for the team

Read this before the demo. It's written so anyone on the team can explain
the project to a judge, even without having touched the code.

## The problem, in one line

A train is coming. The gate needs to close before it arrives, warn people,
stay closed while the train crosses, then reopen safely — even if someone
mashes the buttons or a second train "arrives" too soon.

## What we actually built

Three things, all doing the *same* logic, at three different levels:

1. **A Tinkercad circuit** — a real Arduino Uno simulation. A servo motor
   acts as the gate. Two buttons simulate a train arriving and a train
   leaving. Red/green LEDs and a buzzer show status. This proves the
   embedded-systems logic actually works, cycle by cycle, with real timing.

2. **A backend (FastAPI)** — a Python service that runs the *identical*
   sequence of states as the Arduino code. It logs every step to a database
   and can answer "what's happening right now?" over the web.

3. **A frontend dashboard (React)** — a webpage that shows the current
   status live: a colored badge, an animated gate, a countdown before the
   gate closes, and a scrolling log of everything that's happened.

## The one thing to say clearly if a judge asks "is this connected to the Arduino?"

**No — and that's a deliberate, explainable choice, not something to hide.**

Tinkercad runs entirely in the browser. It has no way to send live signals
to a server running on someone's laptop — there's no cable, no serial link,
nothing physical connecting them. So instead, the backend runs its own copy
of the exact same state machine, and clicking "Simulate train" on the
dashboard triggers that copy, timed to match what we measured in the actual
Tinkercad run (2.93 seconds to close, 2.98 seconds to reopen).

**If asked, say this:** *"The dashboard demonstrates the same safety logic as
the circuit, running on the same timing — it's a software mirror of the
Arduino logic, not a live feed from the physical simulation, since Tinkercad
can't provide one."* That's an honest, engineering-grounded answer. Judges
respect knowing your system's real boundaries far more than a vague claim
that everything is "connected."

## The sequence, step by step

```
NORMAL              gate open, green light, road clear
   |
   |  (a train is detected)
   v
TRAIN_APPROACHING    red light + buzzer turn on, countdown starts
   |
   v
GATE_CLOSING          gate physically closes
   |
   v
TRAIN_CROSSING        gate locked shut -- nothing can reopen it early
   |
   |  (the train has left)
   v
TRAIN_CLEARED         short safety pause before reopening
   |
   v
GATE_OPENING          gate opens
   |
   v
NORMAL (again)
```

## Why it's safe, not just functional

- **While a train is crossing, a second "arrival" is ignored completely.**
  We tested this by repeatedly pressing the arrival button mid-crossing --
  nothing happens until the real departure signal comes in.
- **A departure only counts if a train is actually marked as crossing.**
  You can't skip straight from "normal" to "gate opening."
- **A short delay after departure detection** stops a single button press
  from being read as two events.

## Why there's an "innovation" feature

The live countdown ("gate closing in 1.4s...") isn't just decoration -- it's
a genuine improvement over a system that silently closes the gate. It gives
road users visible warning time, which is exactly what the evaluation
criteria ask for under "innovation and additional features."

## Quick answers for common judge questions

- **"Why Arduino if you're also using a web stack?"** -- The problem
  statement is Arduino-based. The web stack exists to demonstrate the same
  logic in an observable, inspectable way, and to add a real-time UI layer
  on top of the embedded system.
- **"What happens with multiple trains?"** -- Covered above: the system
  ignores overlapping/duplicate detection events by design, and only reacts
  to the correct signal at the correct state.
- **"Is this running on real hardware right now?"** -- No. It's a Tinkercad
  simulation plus a software demo layer, exactly as scoped for a
  software-only hackathon submission.

## Where everything lives

See `ARCHITECTURE.md` at the repo root for the technical breakdown, and
`src/railway_crossing.ino` for the actual Arduino code this is all based on.
