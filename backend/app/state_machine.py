"""
Mirrors backend/../src/railway_crossing.ino exactly:
same 6 states, same transition order, same repeat-guard logic.
Timings below are calibrated to YOUR measured Tinkercad run
(arrival->closed 2.93s, departure->open 2.98s), split in the
same 2:1 and 0.3:1.5:1 ratios the Arduino delays use.
"""

import asyncio
import time
from enum import Enum


class State(str, Enum):
    NORMAL = "NORMAL"
    TRAIN_APPROACHING = "TRAIN_APPROACHING"
    GATE_CLOSING = "GATE_CLOSING"
    TRAIN_CROSSING = "TRAIN_CROSSING"
    TRAIN_CLEARED = "TRAIN_CLEARED"
    GATE_OPENING = "GATE_OPENING"


# Measured-timing splits (seconds)
T_APPROACHING = 1.95   # red LED + buzzer warning window
T_CLOSING = 0.98        # servo travel to closed
T_LOCKOUT = 0.32        # debounce delay after departure press
T_SAFETY_CHECK = 1.60   # TRAIN_CLEARED safety pause
T_OPENING = 1.06        # servo travel to open
TOTAL_CLOSE_TIME = T_APPROACHING + T_CLOSING  # total time from arrival to gate fully closed


class RailwayCrossing:
    def __init__(self, log_event):
        self.state = State.NORMAL
        self.gate_angle = 0
        self.red_led = False
        self.green_led = True
        self.buzzer = False
        self._lock = asyncio.Lock()
        self._log_event = log_event  # async callable(state, note) -> writes SQLite + notifies websockets
        self._arrival_started_at = None  # innovation feature: powers the ETA countdown

    def eta_seconds(self):
        """Seconds remaining until the gate is fully closed. None outside the closing window."""
        if self.state not in (State.TRAIN_APPROACHING, State.GATE_CLOSING):
            return None
        if self._arrival_started_at is None:
            return None
        elapsed = time.time() - self._arrival_started_at
        remaining = TOTAL_CLOSE_TIME - elapsed
        return round(max(remaining, 0), 1)

    def snapshot(self):
        return {
            "state": self.state.value,
            "gate_angle": self.gate_angle,
            "red_led": self.red_led,
            "green_led": self.green_led,
            "buzzer": self.buzzer,
            "eta": self.eta_seconds(),
            "ts": time.time(),
        }

    async def handle_arrival(self):
        # Repeat-guard: only NORMAL accepts a new arrival, exactly like the .ino
        if self.state != State.NORMAL:
            await self._log_event(self.state, "arrival ignored (already mid-cycle)")
            return self.snapshot()

        async with self._lock:
            self._arrival_started_at = time.time()
            self.state = State.TRAIN_APPROACHING
            self.red_led, self.green_led, self.buzzer = True, False, True
            await self._log_event(self.state, "train approaching detected")
            await asyncio.sleep(T_APPROACHING)

            self.state = State.GATE_CLOSING
            await self._log_event(self.state, "gate closing")
            await asyncio.sleep(T_CLOSING)

            self.gate_angle = 90
            self.state = State.TRAIN_CROSSING
            self._arrival_started_at = None
            await self._log_event(self.state, "gate closed, road blocked, train crossing")

        return self.snapshot()

    async def handle_departure(self):
        # Repeat-guard: only TRAIN_CROSSING accepts a departure, exactly like the .ino
        if self.state != State.TRAIN_CROSSING:
            await self._log_event(self.state, "departure ignored (not crossing)")
            return self.snapshot()

        async with self._lock:
            await asyncio.sleep(T_LOCKOUT)  # debounce, matches Arduino's delay(300)

            self.state = State.TRAIN_CLEARED
            self.buzzer = False
            await self._log_event(self.state, "train cleared, safety check running")
            await asyncio.sleep(T_SAFETY_CHECK)

            self.state = State.GATE_OPENING
            await self._log_event(self.state, "gate opening")
            await asyncio.sleep(T_OPENING)

            self.gate_angle = 0
            self.red_led, self.green_led = False, True
            self.state = State.NORMAL
            await self._log_event(self.state, "system normal, road clear")

        return self.snapshot()
