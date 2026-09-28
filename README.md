# Password Strength Benchmark

A small cross-platform desktop app, built with [GlistEngine](https://github.com/GlistEngine/GlistEngine),
that measures how long a brute-force search would take to reproduce a password.
It is a hands-on demonstration of **multithreading**, **thread synchronization**,
and a clean **GUI ⇄ engine** separation in C++.

Type a password (or generate one), and the app spins up a pool of worker threads
that brute-force the keyspace, showing live attempts and guesses/second. Weak
passwords are cracked in front of you; strong ones are never found, and their
crack time is *estimated* from the measured search rate.

## Features

- Multithreaded brute-force engine that scales to the machine's core count.
- Live progress: attempts tried and guesses/second, updated every frame without
  ever freezing the UI.
- Strength verdict graded **Weak / Medium / Strong / Very strong** from the
  estimated crack time.
- Password generator with a collapsible character-set picker (lowercase,
  uppercase, digits, symbols).
- Modern custom UI: a rounded card, rounded buttons with hover inversion, and a
  rounded dropdown with themed checkboxes.

## How it works

The engine never tries the whole keyspace for a strong password — that would take
forever. Instead it:

1. Runs the brute force for a fixed **budget** (a capped number of attempts, a few
   seconds of work).
2. **Measures** the real rate it achieved (guesses/second) with `std::chrono`.
3. **Extrapolates** the full crack time:

   ```
   estimated time = total keyspace ÷ measured rate
   ```

If the password falls inside the budget it is reported as genuinely cracked;
otherwise only the estimate is shown. `Test Strength` infers the alphabet from
the characters the password actually uses, so no manual selection is required.
`Generate & Test` builds a random password from the sets you tick, then tests it.

## Architecture

The GUI and the engine are cleanly separated: the GUI starts the engine and reads
a thread-safe snapshot every frame; it never blocks on the workers.

| Component | Responsibility |
|-----------|----------------|
| `bruteforceengine` | Multithreaded benchmark. Splits the keyspace across worker threads, stops early when any thread finds the target (atomic flag), times the run, and exposes a lock-free-ish `snapshot()`. |
| `passwordgenerator` | Holds the character sets in an `unordered_map` and builds the search pool / a random password from the selected set names. |
| `gCanvas` | The screen: builds the layout, routes button events, and mirrors `engine.snapshot()` into the labels each frame. |
| `gRoundedButton` | A `gGUIButton` drawn with rounded corners and a hover colour inversion. |
| `gRoundedSizer` | A `gGUISizer` with a filled rounded-rectangle background (the green card and the white sub-panels). |
| `gThemedCheckbox` | A `gGUICheckbox` drawn with a rounded, themed tick box. |

### Concurrency model

- `std::thread` worker pool; the keyspace `[0, N)` is split into contiguous ranges,
  one per thread.
- `std::atomic<bool> found` lets the first thread that wins stop all the others,
  and `std::atomic<unsigned long long> attempts` is a lock-free shared counter.
- A `std::mutex` guards only the human-readable summary and timing that the GUI
  reads through `snapshot()`; the critical section is kept tiny.
- The engine returns from `startTest()` immediately (a controller thread joins the
  workers), so the render loop is never blocked.

## Scope & safety

This is a benchmark, not an attack tool. It only tests a password the user typed
or the app generated, **in memory, for the current session**. It does not take
external hashes, crack files/archives/accounts, connect to any network, or touch
any authentication system. Candidate strings are compared **directly** to the
in-memory target (no hashing) — there is no secret being recovered, only a search
duration being measured.

## Limitations (what it does *not* claim)

- The measured rate is this machine + this (unoptimized) code, not a theoretical
  maximum. It is a CPU brute-force floor, not a real attacker's rate.
- Real attacks crack **hashes** (often on GPUs) and lean on dictionaries and leaked
  password lists, so a common password like `Password123!` has a huge brute-force
  keyspace yet is cracked instantly in practice. This tool measures pure
  brute-force effort, not real-world guessability.
- Defenses like **salting** and slow hashes (**bcrypt / argon2**) exist precisely
  to break the "measure × extrapolate" and GPU-parallelism assumptions.

## Build & run

Requires a working GlistEngine setup (this project lives under
`glist/myglistapps/`).

**Windows (CMake + Clang, as configured in `CMakeLists.txt`):**

```sh
cmake -S . -B _build/Release -G "Unix Makefiles"
cmake --build _build/Release --target GlistApp
```

Run the produced `GlistApp.exe` **from the project root** (assets are resolved
relative to the working directory), or launch it from Eclipse / the GlistEngine
IDE workflow.

**macOS (Xcode):** from `_macos/`, run `sh generate_glistapp_xcode.sh macos`, open
the generated project, select the `GlistApp` scheme and run.

## Code style

English-only code and comments. Lowercase variable names, `camelCase` functions,
`func(args)` / `if(cond) {` spacing, tabs for indentation.
