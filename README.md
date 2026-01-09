# Sorting Game 🎮

A multiplayer command-line game that challenges players to sort numbers using two stacks and a specific set of operations. This game is inspired by the 42 School's "push_swap" project and turns algorithm learning into a competitive experience.

## Overview

In Sorting Game, players compete to sort a random sequence of numbers in ascending order using two stacks (A and B) and a limited set of operations. The game supports multiple players, features a time limit, and tracks moves to determine rankings.

## Game Rules

### Initial Setup
- Each player starts with the same sequence of random numbers in Stack A
- Stack B begins empty
- All players have the same time limit to complete the sorting
- Numbers must be sorted in ascending order in Stack A to win

### Available Operations

1. **Swap Operations**
   - `sa`: Swap the first two elements of Stack A
   - `sb`: Swap the first two elements of Stack B
   - `ss`: Perform `sa` and `sb` simultaneously

2. **Push Operations**
   - `pa`: Push the first element of Stack B to Stack A
   - `pb`: Push the first element of Stack A to Stack B

3. **Rotate Operations**
   - `ra`: Rotate Stack A upward (first element becomes last)
   - `rb`: Rotate Stack B upward (first element becomes last)
   - `rr`: Perform `ra` and `rb` simultaneously

4. **Reverse Rotate Operations**
   - `rra`: Rotate Stack A downward (last element becomes first)
   - `rrb`: Rotate Stack B downward (last element becomes first)
   - `rrr`: Perform `rra` and `rrb` simultaneously

## How to Play

### Setup and Compilation
```bash
gcc SortingGame.c -o sorting_game
./sorting_game
```

### Game Flow
1. Enter the number of players (maximum 10)
2. Set the time limit per player (in minutes)
3. Define the size of the number sequence
4. Enter names for each player
5. Players take turns making moves until someone wins or time runs out

### Winning Conditions
- Stack A must contain all numbers in ascending order
- Stack B must be empty
- Must complete sorting within the time limit
- Player with the fewest moves wins in case of multiple successful solutions

### Game End Scenarios
- **SUCCESS**: Player successfully sorts the numbers
- **TIMEOUT**: Player runs out of time
- **QUIT**: Player voluntarily quits using "stop" command
- **FAILED**: Last remaining player fails to complete sorting

## Features

- 🎨 Color-coded player identification
- ⏱️ Real-time tracking of remaining time
- 📊 Visual representation of both stacks
- 🏆 Detailed final rankings
- 📝 Complete move history for each player
- 🔄 Random initial number generation
- 👥 Support for up to 10 players

## Relationship to Push_Swap (42 Project)

Sorting Game is directly inspired by the push_swap project from 42 School's curriculum. Key relationships include:

1. **Educational Purpose**
   - Both teach stack manipulation and sorting algorithms
   - Emphasize optimization and efficiency in sorting solutions

2. **Core Mechanics**
   - Use identical stack operations (sa, sb, ss, pa, pb, etc.)
   - Share the same goal of sorting numbers using two stacks
   - Require strategic thinking about stack manipulation

3. **Key Differences**
   - Sorting Game adds multiplayer competition
   - Includes time limits and real-time visualization
   - Provides immediate feedback on moves
   - Tracks player statistics and rankings

4. **Learning Benefits**
   - Helps understand push_swap concepts through interactive play
   - Provides practical experience with stack operations
   - Encourages development of efficient sorting strategies
   - Allows players to learn from each other's approaches

## Tips for Players

1. Plan your moves ahead - visualization is key
2. Learn common patterns for moving numbers efficiently
3. Use Stack B strategically as a temporary storage
4. Watch other players' strategies to improve your own
5. Practice with different sequence sizes to build experience

## Development

Feel free to contribute to the game by:
- Adding new features
- Improving visualization
- Implementing additional game modes
- Enhancing performance
- Adding network play capabilities

## License

This project is open source and available under the MIT License.
