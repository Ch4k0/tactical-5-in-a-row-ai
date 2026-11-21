# Tactical 5-in-a-Row — Adversarial Search AI (C++)

Adversarial search AI agent developed in **C++** for **Tactical 5-in-a-Row (9×9 Competition Mode)**, an asymmetric, rule-constrained variant of N-in-a-Row with a large branching factor and tactical board mechanics.

> **Note on Architecture:** The base OpenGL simulator (`n_en_raya`) and game engine (`Tablero`) were provided by the Department of Computer Science and Artificial Intelligence (DECSAI, University of Granada). All adversarial search algorithms and heuristic evaluation functions are implemented in `Comportamiento_Agente/AgenteEstudiante.cpp` and `AgenteEstudiante.hpp`.

## Game Mechanics (9×9 Competition Mode)
* **Asymmetric Turn Sequence (1-2-2-2):** Player 1 places 1 piece on turn 1; all subsequent turns allow up to 2 consecutive placements per player.
* **Trinity Rule:** Valid placements must satisfy `(row + col) % 3 == currentPhase % 3`, cycling on every turn change.
* **Adjacency Constraint:** Every new piece must be placed adjacent (8 directions) to an existing piece.
* **Special Cells:** **Green** (+1 extra move in the current turn), **Red** (sabotage: converts placed piece to opponent's color), and **Yellow** (bomb: clears the entire row and column).
* **Tie-Breaker:** If the board saturates without a 5-in-a-row, the player with the most 4-in-a-row lines wins.

## Implemented Algorithms
* **Exhaustive Search (`Status`):** Backtracking full game-tree solver computing the theoretical game-theoretic verdict (`VICTORIA`, `EMPATE`, `DERROTA`) and optimal move for smaller boards or late endgames.
* **Minimax (`minimax`):** Standard depth-limited adversarial search supporting asymmetric multi-move turns.
* **Alpha-Beta Pruning (`alfaBeta`):** Optimized Minimax reaching depth 7 within strict time limits, featuring **depth-biased terminal scoring** (`WIN - depth` / `LOSS + depth`) to prioritize the shortest path to victory and delay unavoidable defeats.

## Heuristic Evaluation & Benchmark Results
Three heuristics were evaluated in a full tournament against 4 pre-trained benchmark opponents (`ninja1`–`ninja4`) using Alpha-Beta pruning at **depth 7**, playing both as Player 1 (First Mover) and Player 2 (Defender):

* **ID 0 (`heuristicaPrueba`):** Positional baseline rewarding center proximity.
* **ID 1 (`heuristica1` — Competition Heuristic):** Pattern-based evaluation with a strong defensive bias (`+10 / +100 / +1000` for own lines of 2/3/4 vs. `-15 / -200 / -5000` for opponent threats), forcing immediate threat blocking.
* **ID 2 (`heuristica2`):** Purely offensive contrast heuristic prioritizing own line-building (`+1,000,000` for 4-in-a-row) with minimal defensive penalties.

| Heuristic | Role | vs Ninja 1 | vs Ninja 2 | vs Ninja 3 | vs Ninja 4 | Win Rate |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **ID 0** (Positional) | P1 / P2 | W / W | L / W | W / W | W / W | **7 / 8** |
| **ID 1** (Balanced-Defensive) | P1 / P2 | W / W | W / W | W / W | W / L | **7 / 8** |
| **ID 2** (Pure Offensive) | P1 / P2 | W / W | W / W | L / W | L / W | **6 / 8** |

## Build & Run (Linux / Ubuntu)

```bash
# Install dependencies and build (first time)
./install.sh

# Subsequent builds
make -j

# Play in GUI mode against the AI (Alpha-Beta, Heuristic 1)
./n_en_raya -p1 humano -p2 inteligente -id2 1 -d 5

# Run headless AI vs AI benchmark (single-threaded, no GUI)
./n_en_rayaSH -p1 inteligente -id1 1 -p2 inteligente -id2 2 -d 7 -nogui

# Run exhaustive solver on classic 3x3 board
./n_en_rayaSH -p1 status -f 3 -c 3 -n 3 -nogui
