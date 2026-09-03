*This project has been created as part of the 42 curriculum by zdaouari.*

# Codexion

## Description

Codexion is a concurrency simulation written in C, inspired by the classic Dining
Philosophers problem. Multiple coders sit in a circular co-working hub, each
alternating between three phases: compiling, debugging, and refactoring. Compiling
requires holding two USB dongles simultaneously — one from each adjacent neighbor.
Dongles are scarce and shared, so coders must compete for them fairly without
deadlocking, starving, or burning out.

The simulation ends either when every coder reaches the required number of compiles,
or when any coder burns out from lack of access to dongles. The program supports two
scheduling policies — FIFO and EDF (Earliest Deadline First) — to arbitrate dongle
access fairly under contention.

The core challenge is designing a correct, race-condition-free, deadlock-free, and
starvation-free concurrent system using only POSIX threads, mutexes, and condition
variables — no semaphores, no global variables.

---

## Instructions

### Compilation

```bash
make
```

Compiles with `-Wall -Wextra -Werror -pthread`. The binary is named `codexion`.

### Cleaning

```bash
make clean    # removes object files
make fclean   # removes object files and binary
make re       # fclean + full recompile
```

### Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

All 8 arguments are mandatory. All time values are in milliseconds.

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders (and dongles). Must be ≥ 1 |
| `time_to_burnout` | Max ms a coder can go without starting a compile |
| `time_to_compile` | Duration of the compile phase (holds both dongles) |
| `time_to_debug` | Duration of the debug phase |
| `time_to_refactor` | Duration of the refactor phase |
| `number_of_compiles_required` | Simulation ends when all coders reach this count |
| `dongle_cooldown` | Ms a dongle is unavailable after being released |
| `scheduler` | Arbitration policy: exactly `fifo` or `edf` |

### Example runs

```bash
# 4 coders, generous timing, fifo scheduling — should complete cleanly
./codexion 4 800 200 100 100 3 50 fifo

# 3 coders, edf scheduling — prioritizes coders closest to burnout
./codexion 3 500 100 100 100 5 50 edf

# 1 coder — structurally cannot compile (needs 2 dongles), burns out
./codexion 1 500 200 100 100 3 50 fifo
```

## Resources

 -  most of concepts that I learned is from this book ["The Linux Programming inTerface"](https://www.bogotobogo.com/Linux/files/the-linux-programming-interface-1.pdf)
 - [how mutex works](https://www.youtube.com/watch?v=1tZhmTnk-vc)
 - [geeksforgeeks for theorics](https://www.geeksforgeeks.org/operating-systems/difference-between-process-and-thread/)
 - [Coffman Conditions — Wikipedia](https://en.wikipedia.org/wiki/Deadlock#Necessary_conditions)

- [Binary Heap — Wikipedia](https://en.wikipedia.org/wiki/Binary_heap)
 ### AI usage

AI was used throughout this project for the following:

- **Concept explanation**: explaining POSIX thread primitives (`pthread_cond_wait`,
  `pthread_cond_timedwait`, mutex semantics, context switching internals) in depth,
  including edge cases lost wakeups, and lock ordering.
---

## Blocking Cases Handled

### 1. Deadlock prevention — Coffman's four conditions

The classic deadlock scenario: every coder holds one dongle and waits for the other,
forming a circular chain where nobody can proceed. Codexion breaks the
**circular wait** condition (one of Coffman's four necessary conditions) by enforcing

- **locking the dongles with a mutex, making impossible for other to take it by force**

- **sleeping all the coders with a even id number for a specific time in the begining**
```c
sleep_time = (compilation_time + time_to_cold_down ) / 2 
// to leave the coder with odd number id take first 
```

- **consistent global lock acquisition order based on pointer address**:

```c
if (c->left < c->right)
    { d1 = c->left;  d2 = c->right; }
else
    { d1 = c->right; d2 = c->left; }
pthread_mutex_lock(&d1->lock);
pthread_mutex_lock(&d2->lock);
```

By always locking the lower-address dongle first, no two coders can form a circular


### 2. Atomic two-dongle acquisition — eliminating hold-and-wait

Codexion addresses the **hold-and-wait** condition by never letting a coder hold one
dongle while blocking on the other. Before attempting acquisition, both requests are
registered in both dongles' priority heaps simultaneously. Then, inside
`acquire_dognles`, the coder only proceeds to take both dongles when **all conditions
are simultaneously satisfied**: neither dongle is in use, neither dongle is in
cooldown, and the coder is the top-priority requester on both heaps:

```c
if (status == 0
    && !is_cooldown_active(d1, d2, now)
    && check_periority(d1, d2, c->id))
    break; // take both atomically
```

If any condition fails, the coder releases both locks and waits — never holding one
resource while blocking on another.

### 3. Starvation prevention — EDF scheduling

Under FIFO scheduling alone, a coder that consistently loses the mutex race can be
perpetually delayed while others keep cycling. The EDF (Earliest Deadline First)
scheduler addresses this by always granting dongle access to the coder whose burnout
deadline is soonest (`last_compile_start + time_to_burnout`). A coder that hasn't
compiled in a long time automatically rises to the top of the priority queue before
it burns out, guaranteeing liveness under feasible parameters.

Each dongle maintains a hand-rolled binary min-heap of pending requests. Under EDF,
the heap is keyed by deadline — the coder with the smallest deadline value is always
at the root and served first. Tie-breaking uses a monotonic sequence counter to ensure
fully deterministic ordering even when two coders share the same deadline.

### 4. Dongle cooldown handling

After a dongle is released, it enters a cooldown period during which it cannot be
acquired by any coder. The cooldown end time is stored as an absolute simulation
timestamp (`available_at_ms = elapsed_ms(sim) + dongle_cooldown`) on the dongle
struct. Inside the acquisition wait loop, `is_cooldown_active` checks both dongles:

```c
bool is_cooldown_active(t_dongle *d1, t_dongle *d2, long now)
{
    return (now < d1->available_at_ms || now < d2->available_at_ms);
}
```

When the only blocking condition is cooldown (both dongles free but cooling down),
the coder uses `pthread_cond_timedwait` with a deadline computed from the remaining
cooldown time — avoiding busy-waiting while still waking up precisely when the
cooldown expires.

### 5. Precise burnout detection — monitor thread

A dedicated monitor thread polls all coders' `last_compile_start` timestamps every
1ms and computes whether any coder has exceeded its burnout deadline:

```c
deadline = sim->coders[i].last_compile_start + sim->cfg.time_to_burnout;
if (now > deadline)
    // burnout detected
```

Polling at 1ms intervals guarantees burnout is detected and logged within the
required 10ms tolerance. The monitor is the single authority that sets `sim->stop`
and broadcasts to all dongle condition variables to wake sleeping coder threads so
they exit cleanly.

### 6. Log serialization

All log output is protected by a dedicated `log_lock` mutex. Every call to
`log_state` locks this mutex for the entire duration of the `printf` call, preventing
any two threads from writing to stdout simultaneously. Output lines are never
interleaved.

---

## Thread Synchronization Mechanisms

### Mutexes used

| Mutex | Protects |
|---|---|
| `dongle.lock` | `in_use`, `available_at_ms`, `waiting` heap for each dongle |
| `sim.state_lock` | `stop`, `start`, `someone_burned_out`, `last_compile_start`, `compiles_done`, `request_counter` |
| `sim.log_lock` | stdout output serialization |

### Condition variables used

| Cond var | Associated mutex | Signals when |
|---|---|---|
| `dongle.cond` | `dongle.lock` | Dongle released (available_at_ms updated, in_use set false) |

### How race conditions are prevented

**Race on dongle state**: Two coders could both read `in_use == false` and both
proceed to take the same dongle (TOCTOU race). Prevented by holding `dongle.lock`
for the entire check-and-set sequence:

```c
pthread_mutex_lock(&d->lock);
// check in_use, available_at_ms, heap priority
// set in_use = true only after all checks pass
pthread_mutex_unlock(&d->lock);
```

**Race on `last_compile_start`**: The monitor reads this value to compute burnout
deadlines while the coder thread writes it after acquiring dongles. Both operations
are protected by `state_lock`:

```c
// coder writes:
pthread_mutex_lock(&sim->state_lock);
c->last_compile_start = elapsed_ms(sim);
pthread_mutex_unlock(&sim->state_lock);

// monitor reads:
pthread_mutex_lock(&sim->state_lock);
deadline = sim->coders[i].last_compile_start + sim->cfg.time_to_burnout;
pthread_mutex_unlock(&sim->state_lock);
```

**Race on heap modification**: Dongle heaps are modified (push on request, extract
on acquisition) and read (peek for priority check) from multiple threads. All heap
operations on a dongle's heap are performed while holding that dongle's `lock` —
the same mutex that protects the dongle's other fields. No separate heap lock needed.

**Race on `stop` flag**: The monitor writes `stop = true`; every coder thread reads
it in `is_stoped()`. Both sides use `state_lock`:

```c
bool is_stoped(t_sim *sim)
{
    bool val;
    pthread_mutex_lock(&sim->state_lock);
    val = sim->stop;
    pthread_mutex_unlock(&sim->state_lock);
    return (val);
}
```

### Thread-safe communication between coders and monitor

The monitor never directly signals coder threads — it communicates exclusively through
the shared `stop` flag (protected by `state_lock`). When `stop` is set, the monitor
broadcasts on every dongle's condition variable to wake all sleeping coders:

```c
while (i < sim->cfg.num_coders)
{
    pthread_mutex_lock(&sim->dongles[i].lock);
    pthread_cond_broadcast(&sim->dongles[i].cond);
    pthread_mutex_unlock(&sim->dongles[i].lock);
    i++;
}
```

Coder threads check `is_stoped(sim)` at the top of their main loop and inside
`acquire_dognles` — upon waking from `cond_wait` or `cond_timedwait`, they recheck
the stop flag and exit cleanly, removing their pending requests from both dongle
heaps before releasing locks.

### Startup synchronization — `sim.start` gate

All coder threads spin-wait on `sim->start` before entering their main loop,
ensuring no coder begins competing for dongles until all threads (including the
monitor) are created and ready. `sim->start` is set by main under `state_lock` after
all `pthread_create` calls succeed:

```c
pthread_mutex_lock(&sim->state_lock);
sim->start = true;
pthread_mutex_unlock(&sim->state_lock);
```

---

