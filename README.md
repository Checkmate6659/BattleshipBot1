# BattleshipBot1

BattleshipBot1 is a very uncreatively named bot that plays Battleship. By commenting stuff in the main.c file, you can make it play against itself, or against an external agent (yourself, friend, AI...).
It doesn't handle placing ships however, so either you have to place those, or you have to insert this code into main.c to generate a random ship placement:
```c
place_ships_random(grid);
print_grid(grid, true);
```

## Rules
Each player starts with 5 ships:
- a submarine (A), 3 spaces long
- a cruiser (B), also 3 spaces long
- a destroyer (C), 2 spaces long
- a battleship (D), 4 spaces long
- a carrier (E), 5 spaces long

When a player's ship is sunk, they have to announce what type of ship it is, and where it is located.

---
WARNING: Be careful when typing in the data, as there is no undo option. Also, there is no overflow protection when typing in coordinates of sunken enemy ships, and you must use the ':' character for 10.
If you enter wrong data at one point, do not worry though: the AI is 100% deterministic, so you can set up an older grid just by restarting the AI and typing in all results up to that point.


Also, there are probably lots of horrendous bugs in my code, as in my tests on this version I got an average of 58 moves/game (with some games lasting more than 90 moves!) in a 103 game test, but in most single game tests the move count doesn't exceed 50. In an earlier version, I got an average of 24 moves, which seems too good to be true, and it probably is (since I had to fix major bugs in that code, and in the VS external agent mode, the move count was around 40).
