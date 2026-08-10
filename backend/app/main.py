import asyncio
import time

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware

from . import db
from .state_machine import RailwayCrossing

app = FastAPI(title="Automated Railway Level Crossing API")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # tighten before final submission if time permits
    allow_methods=["*"],
    allow_headers=["*"],
)

connected_sockets: list[WebSocket] = []


async def log_and_broadcast(state, note: str):
    ts = time.time()
    await db.insert_log(ts, state.value, note)
    payload = {"state": state.value, "note": note, "ts": ts}
    dead = []
    for ws in connected_sockets:
        try:
            await ws.send_json(payload)
        except Exception:
            dead.append(ws)
    for ws in dead:
        connected_sockets.remove(ws)


crossing = RailwayCrossing(log_event=log_and_broadcast)


@app.on_event("startup")
async def startup():
    await db.init_db()


@app.get("/api/status")
async def get_status():
    return crossing.snapshot()


@app.get("/api/logs")
async def get_logs(limit: int = 50):
    return await db.fetch_logs(limit)


@app.post("/api/event")
async def post_event(payload: dict):
    event_type = payload.get("type")
    if event_type == "arrival":
        return await crossing.handle_arrival()
    elif event_type == "departure":
        return await crossing.handle_departure()
    return {"error": "type must be 'arrival' or 'departure'"}


@app.post("/api/demo/start")
async def demo_start():
    """Fires the same arrival->departure sequence a real Arduino press would,
    timed to your measured Tinkercad run, so the dashboard and simulation
    move in sync during the live demo."""
    asyncio.create_task(_run_demo())
    return {"status": "demo started"}


async def _run_demo():
    await crossing.handle_arrival()
    await asyncio.sleep(2.0)  # simulate train visibly "crossing" before departure
    await crossing.handle_departure()


@app.websocket("/ws/status")
async def websocket_status(websocket: WebSocket):
    await websocket.accept()
    connected_sockets.append(websocket)
    try:
        await websocket.send_json(crossing.snapshot())
        while True:
            await websocket.receive_text()  # keep-alive; client doesn't need to send anything meaningful
    except WebSocketDisconnect:
        connected_sockets.remove(websocket)
