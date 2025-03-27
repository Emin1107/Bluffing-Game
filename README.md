# Entertaining Spice Pretending – A Terminal-Based Bluffing Card Game

**Entertaining Spice Pretending** is a two-player terminal game focused on bluffing, logic, and card strategy. Written entirely in C, the game simulates the tension of classic bluff games like poker — but with spices, values, and clever challenges.

## Features

- **Bluff & Challenge Mechanics**  
  Trick your opponent by playing a fake card, or call out their bluff with a "spice" or "value" challenge.

- **Dynamic Game Flow**  
  Play cards in ascending value of the same spice, draw from the deck, or surprise your opponent with a bold swap move.

- **Strategic Points System**  
  Points are awarded based on card play and successful or failed challenges. Bonus points are possible!

- **Terminal UI**  
  Text-based prompts, instructions, and feedback for a smooth and intuitive experience.

- **Robust Memory Management**  
  Implemented with linked lists and dynamic memory allocation, fully cleaned up after each game.

## Getting Started

### Requirements
- C Compiler (e.g. `gcc`)
- Unix-like terminal (Linux/macOS)

### Compilation
To build the game, simply run:

```bash
gcc -Wall -Wextra -o esp main.c
```

### Usage
Run the game by providing a valid card configuration file:

```bash
./esp config.txt
```

### Config File Format

The configuration file must:
- Start with a header line: `ESP`
- Followed by card entries in the format: `<value>_<spice>`, e.g. `2_c`

Example:
```
ESP
1_c
2_p
3_w
4_c
5_p
6_w
```

- `c` = Cinnamon  
- `p` = Pepper  
- `w` = Wasabi  
- Values: 1–10

## Game Rules

- Each player starts with **6 cards**, taken alternately from the draw pile.
- Cards are played in increasing value of the same spice.
- First card of each round must be **value 1–3**.
- Players can **draw**, **swap**, or **challenge** opponent’s last move.
- **Challenge Types:**  
  - `challenge value`  
  - `challenge spice`
- A **correct challenge** earns you points, a **wrong one** gives points to the opponent.
- If a player runs out of cards, special draw rules apply.
- Game ends when the draw pile is empty.

## Sample Gameplay

```
Welcome to Entertaining Spice Pretending!

Player 1:
    latest played card:
    cards played this round: 0
    hand cards: 1_c 2_c 3_p 4_w 5_p 6_w

P1 > play 2_c 3_p

Player 2:
    latest played card: 3_p
    cards played this round: 1
    hand cards: 1_p 3_w 4_c 5_c 6_c 7_w

P2 > challenge spice
Challenge successful: 3_p's spice does not match the real card 2_c.
Player 2 gets 1 points.
```

## Development Notes

This game was built to:
- Reinforce understanding of **linked data structures**
- Practice **memory-safe coding in C**
- Explore interactive **game loop design**
- Simulate a turn-based game in a minimal interface

## File Structure

```
.
├── main.c              # Entire game logic
├── config.txt          # Sample game configuration
└── README.md           # You are here
```

## License

This project is open-source and available under the MIT License.

---

Made with love for logic, games, and C programming.
