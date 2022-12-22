#include <stdio.h>
#include <time.h>

#include "board.h"
#include "ai.h"


uint8_t grid[100];


// Display grid
void print_grid(uint8_t *g, bool friendly)
{
    printf("   A B C D E F G H I J\n 1 ");
    for (uint8_t i = 0; i < 100; i++)
    {
        uint8_t elt = g[i];
        if (friendly && (elt & M_SUNK)) //sunken ship on friendly board
            putchar('#'); //use # not to confuse with ship letters on non-sunken ships
        else if (elt & M_SUNK) //enemy sunken ship
            putchar('A' + bitscan8(elt & M_ANY_SHIP)); //use ship letter instead of # to avoid confusion
        else if (elt & M_HIT)
            putchar((elt & M_ANY_SHIP) ? 'X' : '~'); // hit/miss
        else if (friendly && (elt & M_ANY_SHIP))
            putchar('A' + bitscan8(elt & M_ANY_SHIP)); // friendly board: make ships appear
        else
            putchar('.'); // empty space

        if (i == 99) putchar('\n');
        else if(i % 10 == 9) printf("\n%2d ", i / 10 + 2);
        else putchar(' ');
    }
}

int main()
{
    srand(time(NULL)); //initialize RNG

    //Play a game of Battleship against an external agent
    // uint8_t move_count = 0;
    // uint8_t remaining = 5;
    // while (remaining)
    // {
    //     print_grid(grid, false);

    //     move_count++;
    //     printf("MOVE %d\n", move_count);
    //     uint8_t move = generate_best_move(grid);
    //     printf("TARGET: %c%c\n", move % 10 + 'A', move / 10 + '1');
    //     printf("NOTE: THE ':' CHARACTER MEANS 10\n");

    //     printf("IF HIT, ENTER 'X'. IF MISS, ENTER 'O'. IF SUNK, ENTER SHIP LETTER:\n");
    //     printf("A FOR SUBMARINE (LENGTH 3)\n");
    //     printf("B FOR CRUISER (LENGTH 3)\n");
    //     printf("C FOR DESTROYER (LENGTH 2)\n");
    //     printf("D FOR BATTLESHIP (LENGTH 4)\n");
    //     printf("E FOR CARRIER (LENGTH 5)\n");
    //     bool invalid = true;
    //     while (invalid)
    //     {
    //         invalid = false;
    //         char result = getchar();
    //         if (result == 'X') grid[move] = M_HIT | M_ANY_SHIP; //hit: put ship tile there (all at once!!!)
    //         else if (result == 'O') grid[move] = M_HIT; //miss
    //         else if (result >= 'A' && result <= 'E') //sank ship
    //         {
    //             uint8_t type = result - 'A';
    //             uint8_t length = LENGTHS[type];
    //             uint8_t top_left;
    //             uint8_t offset;

    //             char coords[3]; //first 2 characters are coordinates, 3rd is direction
    //             printf("THE AI SANK A SHIP. INDICATE THE COORDINATES OF THE TOP LEFT OF THE SHIP:\n");
    //             printf("ENTER ONLY 2 DIGITS (example: E4). IF MORE ARE ENTERED, THE PROGRAM MAY BEHAVE UNRELIABLY.\n");
    //             printf("NOTE: USE THE ':' CHARACTER FOR 10\n");
    //             scanf("%s", coords);
    //             printf("ENTER A H IF THE SHIP IS HORIZONTAL, AND A V IF THE SHIP IS VERTICAL\n");
    //             printf("MOST INVALID INPUT WILL BE TREATED AS HORIZONTAL\n");
    //             scanf("%c", coords + 2);
    //             scanf("%c", coords + 2);
    //             for (uint8_t i = 0; i < 3; i++) coords[i] &= 15; //only keep the lower part of it (allow small letters and make numbers easier to work with)

    //             top_left = coords[0] + 10 * coords[1] - 11;
    //             offset = 1 + 9 * (coords[2] == 6);

    //             //one ship has been sunk, keep track of it
    //             remaining -= 1;
    //             ship_spot_count[type] = 0;

    //             //mark the sunken ship there
    //             for (uint8_t i = top_left; i < top_left + length * offset; i += offset)
    //                 grid[i] = (1 << type) | M_HIT | M_SUNK;
    //         }
    //         else invalid = true;
    //     }
    // }

    //Automatic self-play battleship game
    place_ships_random(grid);

    clock_t duration = clock();

    uint8_t move_count = 0;
    uint8_t remaining = 17;
    while (remaining)
    {
        uint8_t move = generate_best_move(grid);
        remaining -= hit(grid, move);
        move_count++;
    }

    duration = clock() - duration;

    print_grid(grid, false);
    // print_grid(grid, true);
    printf("Move count: %d\n", move_count);
    printf("Time: %lf\n", (double)duration / CLOCKS_PER_SEC);

    return 0;
}
