import { useEffect, useRef, useState } from 'react'

const API_BASE = 'http://localhost:8000'
const WS_URL = 'ws://localhost:8000/ws/status'

const STATE_META = {
  NORMAL: { label: 'Normal — road clear', color: '#1d9e75' },
  TRAIN_APPROACHING: { label: 'Train approaching', color: '#ba7517' },
  GATE_CLOSING: { label: 'Gate closing', color: '#ba7517' },
  TRAIN_CROSSING: { label: 'Train crossing — road blocked', color: '#a32d2d' },
  TRAIN_CLEARED: { label: 'Train cleared — safety check', color: '#ba7517' },
  GATE_OPENING: { label: 'Gate opening', color: '#ba7517' },
}

function GateIcon({ state }) {
  const closed = state === 'TRAIN_CROSSING' || state === 'TRAIN_CLEARED' || state === 'GATE_CLOSING'
  return (
    <div className="gate-wrap">
      <div className="gate-post" />
      <div
        className="gate-barrier"
        style={{ transform: closed ? 'rotate(0deg)' : 'rotate(-75deg)' }}
      />
      <div className="gate-post gate-post-right" />
    </div>
  )
}

export default function App() {
  const [status, setStatus] = useState({ state: 'NORMAL' })
  const [logs, setLogs] = useState([])
  const [connected, setConnected] = useState(false)
  const wsRef = useRef(null)

  const refreshLogs = async () => {
    try {
      const res = await fetch(`${API_BASE}/api/logs`)
      setLogs(await res.json())
    } catch (e) {
      console.error('log fetch failed', e)
    }
  }

  useEffect(() => {
    fetch(`${API_BASE}/api/status`)
      .then((r) => r.json())
      .then(setStatus)
      .catch(() => {})
    refreshLogs()

    const ws = new WebSocket(WS_URL)
    wsRef.current = ws
    ws.onopen = () => setConnected(true)
    ws.onclose = () => setConnected(false)
    ws.onmessage = (event) => {
      const payload = JSON.parse(event.data)
      setStatus((prev) => ({ ...prev, state: payload.state, eta: payload.eta }))
      // eta ticks arrive every 0.2s and don't need a log refresh; only
      // refresh the log table when the state itself actually changed
      if (payload.eta === null || payload.eta === undefined) {
        refreshLogs()
      }
    }
    return () => ws.close()
  }, [])

  const runDemo = async () => {
    await fetch(`${API_BASE}/api/demo/start`, { method: 'POST' })
  }

  const meta = STATE_META[status.state] || STATE_META.NORMAL

  return (
    <div className="page">
      <header>
        <h1>Automated Railway Level Crossing</h1>
        <span className={`ws-dot ${connected ? 'live' : 'down'}`}>
          {connected ? 'live' : 'disconnected'}
        </span>
      </header>

      <section className="status-panel">
        <div className="badge" style={{ background: meta.color }}>
          {meta.label}
        </div>
        {typeof status.eta === 'number' && (
          <div className="eta">Gate closing in {status.eta.toFixed(1)}s</div>
        )}
        <GateIcon state={status.state} />
        <button onClick={runDemo}>Simulate train</button>
      </section>

      <section className="log-panel">
        <h2>Crossing log</h2>
        <table>
          <thead>
            <tr>
              <th>Time</th>
              <th>State</th>
              <th>Note</th>
            </tr>
          </thead>
          <tbody>
            {logs.map((row, i) => (
              <tr key={i}>
                <td>{new Date(row.ts * 1000).toLocaleTimeString()}</td>
                <td>{row.state}</td>
                <td>{row.note}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </section>
    </div>
  )
}
