import aiosqlite

DB_PATH = "crossing.db"


async def init_db():
    async with aiosqlite.connect(DB_PATH) as db:
        await db.execute(
            """CREATE TABLE IF NOT EXISTS logs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                ts REAL NOT NULL,
                state TEXT NOT NULL,
                note TEXT NOT NULL
            )"""
        )
        await db.commit()


async def insert_log(ts: float, state: str, note: str):
    async with aiosqlite.connect(DB_PATH) as db:
        await db.execute(
            "INSERT INTO logs (ts, state, note) VALUES (?, ?, ?)", (ts, state, note)
        )
        await db.commit()


async def fetch_logs(limit: int = 50):
    async with aiosqlite.connect(DB_PATH) as db:
        db.row_factory = aiosqlite.Row
        cursor = await db.execute(
            "SELECT ts, state, note FROM logs ORDER BY id DESC LIMIT ?", (limit,)
        )
        rows = await cursor.fetchall()
        return [dict(r) for r in rows]
