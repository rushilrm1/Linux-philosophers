# Dining Philosophers Problem — Processes & System V Semaphores

A C implementation of the classic **Dining Philosophers Problem** using `fork()` for process creation and **System V semaphores** for synchronization.

## 📖 Overview

This project simulates five philosophers sitting at a round table, each needing two forks (shared resources) to eat. Each philosopher is modeled as a **separate process** (not a thread), and each fork is represented by a **semaphore** in a System V semaphore set. The implementation avoids deadlock through an asymmetric fork-acquisition strategy.

## 🧩 Problem Analysis

- Five philosophers and five forks arranged around a table.
- Each philosopher alternates between **thinking** and **eating**.
- A philosopher must acquire **both** adjacent forks before eating.
- Since forks are shared between neighbors, naive acquisition can lead to deadlock.

## 🔒 Deadlock Avoidance Strategy

To prevent all philosophers from picking up their left fork simultaneously (the classic deadlock scenario), this implementation uses an **asymmetric ordering**:

- **Even-numbered philosophers** acquire the **left** fork first, then the **right**.
- **Odd-numbered philosophers** acquire the **right** fork first, then the **left**.

This breaks the circular wait condition required for deadlock.

> **Alternative approach:** Limit the number of philosophers competing for forks at any time to four (using an additional counting semaphore), guaranteeing at least one philosopher can always eat.

## ⚙️ Algorithm

**Parent process:**
1. Create a semaphore set using `semget()`.
2. Initialize all five semaphores to `1` using `semctl()`.
3. Create five child processes using `fork()`.
4. Wait for all children to finish using `wait()`.
5. Remove the semaphore set using `IPC_RMID`.

**Child process (each philosopher):**
```
repeat 5 times:
    think
    wait(fork_left / fork_right)   // order depends on even/odd id
    wait(fork_right / fork_left)
    eat
    signal(fork1)
    signal(fork2)
exit
```

## 🖥️ System Calls Used

| Call | Purpose |
|------|---------|
| `fork()` | Create philosopher processes |
| `wait()` | Parent waits for children to terminate |
| `semget()` | Create/access the semaphore set |
| `semop()` | Acquire (`P`) / release (`V`) operations on forks |
| `semctl()` | Initialize and remove semaphores |
| `sleep()` | Simulate thinking/eating delays |
| `exit()` | Terminate child processes |

## 🚀 Getting Started

### Prerequisites
- A Unix/Linux environment (System V semaphores are POSIX/Linux-specific).
- `gcc` compiler.

### Build
```bash
gcc -o dining_philosophers dining_philosophers.c
```

### Run
```bash
./dining_philosophers
```

## 📄 License

This project is intended for educational purposes (Operating Systems coursework).
