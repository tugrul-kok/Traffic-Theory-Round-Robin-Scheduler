# Round-Robin Scheduler — Traffic Theory Project

Implementation of a **Round-Robin scheduling discipline** in **OMNeT++**, developed as part of the *Traffic Theory* course at Politecnico di Milano.  
The project compares the behavior of a Round-Robin (RR) queueing system with a standard **M/M/1 queue**, analyzing performance under different load conditions.

The simulation is built on top of the **Source–Queue–Sink** model and extends it with message sizes, self-messages, and RR scheduling logic.

---

## 🚀 Project Overview

This project implements:

- A **Round-Robin scheduler** where each message receives a quantum of service equal to one *avgServiceTime*.
- A standard **M/M/1 scheduler** for comparison.
- A configurable **message size model**, where each packet has a random service-time demand.
- Simulation scenarios for:
  - **ρ = 0.4** (light load)
  - **ρ = 0.8** (heavy load)
  - **ρ > 1** (unstable system)

---

## 📁 Repository Structure

```
Traffic-Theory-Round-Robin-Scheduler/
├─ src/
│  ├─ Queue.cc
│  ├─ Source.cc
│  ├─ Sink.cc
│  └─ RoundRobin modifications
├─ omnetpp.ini
├─ RoundRobinScheduler.ned
├─ Project_Report.pdf
└─ README.md
```

---

## ⚙️ Implementation Details

### 1. Message Processing Time  
Each message has an integer `MsgSize` that determines how many quanta of service it needs.

- `MsgSize` is drawn from a **uniform distribution** based on `meanOfMsgSize`.
- A message requiring size = 2 will need **2 × avgServiceTime** total service time.

---

### 2. omnetpp.ini Configuration  
The scheduler type is selected via:

```
**.queue.isRoundRobin = true/false
```

The utilization factor is:

\[
ρ = λ \cdot E[S] = 
	ext{meanOfMsgSize} \cdot 
rac{	ext{avgServiceTime}}{	ext{avgInterArrivalTime}}
\]

Configurations included:
- ρ = 0.4  
- ρ = 0.8  
- ρ = 1.1 (unstable)

---

### 3. Handling Messages

- **Round-Robin mode**
  - Decrease `MsgSize` by 1 per quantum.
  - If remaining size > 0 → reinsert into queue.
  - If size = 0 → forward to the sink.

- **M/M/1 mode**
  - Service time = `MsgSize × avgServiceTime`.
  - Message leaves the system after processing.

Self-messages represent service completion events.

---

## 📊 Results Summary

### ρ = 0.4 — Light Load
- RR gives **lower response time** compared to M/M/1.
- Fair time-sharing allows processing more messages.

### ρ = 0.8 — Heavy Load
- RR still slightly faster in response time.
- Matches theoretical prediction:
  \[
  W_{RR} < W_{M/M/1} 	ext{ when } nQ < rac{1}{1-ρ}
  \]

### ρ > 1 — Unstable
- Both systems unstable.
- M/M/1 performs **better** because RR cycles messages without finishing them fast enough.

---

## 🧪 How to Run

1. Install **OMNeT++** (5.x or later)
2. Clone the repository:

```bash
git clone https://github.com/tugrul-kok/Traffic-Theory-Round-Robin-Scheduler
cd Traffic-Theory-Round-Robin-Scheduler
```

3. Open in OMNeT++ IDE
4. Build the project
5. Run desired configuration from `omnetpp.ini`:
   - `Net1` → ρ = 0.4  
   - `Net2` → ρ = 0.8  
   - `Net3` → ρ = 1.1  

---

## 📘 Reference

See full project explanation in `Project_Report.pdf`.

