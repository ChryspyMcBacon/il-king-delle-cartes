# Il King delle Cartes

A turn-based card game for the terminal, written in **C99** as the group project for **Introduzione alla Programmazione** (Ca' Foscari University of Venice, A.Y. 2024/2025).

From 2 to 20 players take turns on the same keyboard. Each round everyone gets one face-up and one face-down card, and the cards' effects move life points around the table. The last player standing wins.

> The game's interface is in Italian.

## Rules

Every player starts with the same number of life points. Each **phase**:

1. The 40-card deck (1–7, J, Q, K in four suits) is shuffled.
2. Each player gets two cards: one face up, one face down.
3. A random player starts, and turns go around the table in ring order.

On your turn you **must** apply the effect of your face-up card, then you can peek at your face-down card and choose whether to reveal it and apply its effect too. At the end of the phase, players with no life points left are eliminated.

| Card | Effect |
|------|--------|
| 1 (Ace) | Lose 1 life point, which is left on the table |
| 2 – 6 | Nothing happens |
| 7 | The next player is forced to reveal their face-down card and apply it |
| J | Give 1 life point to the previous player |
| Q | Give 1 life point to the player two seats ahead |
| K | Collect all life points left on the table |

Effects can **chain**: a 7 can reveal another player's 7, which reveals the next one, and so on.

## Features

- Configurable number of players (2–20) and starting life points (2–10), with input validation
- Fisher–Yates shuffle and random first player each phase
- Ring-order turns and chained card effects, handled iteratively (an earlier recursive version risked infinite loops)
- Eliminated players are removed by compacting the array at the end of each phase; if everyone is eliminated in the same phase, the last one to fall wins
- Colored terminal UI with ANSI escape codes, menus and feedback messages
- Cross-platform screen clearing (Windows / macOS / Linux)
- Full **Doxygen** documentation of functions and data types

## Build & run

Requires a C compiler (`gcc` or `clang`).

```bash
make        # build
make run    # build and play
make clean  # remove the executable
```

Or without `make`:

```bash
gcc -std=c99 -Wall -Wextra src/IlKingDelleCartes.c -o IlKingDelleCartes
./IlKingDelleCartes
```

## Documentation

📖 **[Browse the documentation online](https://chryspymcbacon.github.io/il-king-delle-cartes/)**

The Doxygen documentation is generated in `docs/html/` and published with GitHub Pages. To regenerate it:

```bash
make docs   # requires doxygen
```

## Project structure

```
.
├── src/
│   └── IlKingDelleCartes.c   # game source
├── docs/
│   ├── html/                 # Doxygen documentation (published on GitHub Pages)
│   ├── index.html            # redirect to the documentation
│   ├── Consegna.pdf          # original assignment (in Italian)
│   └── Relazione.docx        # project report (in Italian)
├── Doxyfile
├── Makefile
└── README.md
```

## Team

| | Main contributions |
|---|---|
| **Christian Occhiogrosso** | Player elimination logic, terminal UI (ANSI colors, layout, menus and input feedback) |
| **Leonardo Serpelloni** | Ring-order turn management, overall phase structure and main game loop |
| **Mattia Rosin** | Deck creation, Fisher–Yates shuffle and dealing, player setup |

All parts were reviewed and debugged together.

## Credits

The game rules and the assignment were designed by the instructors of *Introduzione alla Programmazione* at Ca' Foscari University of Venice.
