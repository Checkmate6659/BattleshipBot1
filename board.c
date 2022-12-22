#include "board.h"


const uint8_t LENGTHS[] = {L_SUBMARINE, L_CRUISER, L_DESTROYER, L_BATTLESHIP, L_CARRIER};   // Lengths of ships
uint8_t ship_spot_count[] = {L_SUBMARINE, L_CRUISER, L_DESTROYER, L_BATTLESHIP, L_CARRIER}; // to know which ships have been sank already
uint8_t ship_root[5];
uint8_t ship_offset[5];


//Single bit only 8-bit DeBruijn bitscan
uint8_t bitscan8(uint8_t x)
{
    const uint8_t _BITSCAN8[] = {0, 1, 6, 2, 7, 5, 4, 3}; //Bitscan table
    uint8_t index = x * 29; //clipping top useless bits
    index >>= 5; //extracting top 3 bits as index into table
    return _BITSCAN8[index];
}

//reset a game of battleship
void reset_game(uint8_t *g)
{
    for (uint8_t i = 0; i < 100; i++) g[i] = 0; //clear the grid
    for (uint8_t i = 0; i < 5; i++) ship_spot_count[i] = LENGTHS[i]; //reset the ship spot counts
}

//Hit a square on a grid. Returns true if it's a hit and false if it's a miss.
bool hit(uint8_t *g, uint8_t index)
{
    g[index] |= M_HIT; //mark the hit
    if (!(g[index] & M_ANY_SHIP)) return false; //miss

    uint8_t type = bitscan8(g[index] & M_ANY_SHIP);
    ship_spot_count[type] -= 1; //decrement ship spot count
    if (!ship_spot_count[type]) //no spots remaining: the ship has been sunk
    {
        //mark ship as sunk
        for (uint8_t pos = ship_root[type]; pos < ship_root[type] + LENGTHS[type] * ship_offset[type]; pos += ship_offset[type])
            g[pos] |= M_SUNK;
    }

    return true;
}

// Place a ship. Returns true if ship placement succeeded.
bool place_ship(uint8_t *g, uint8_t type, uint8_t index, bool vertical)
{
    uint8_t length = LENGTHS[type];                            // ship length
    uint8_t critical = vertical ? (index / 10) : (index % 10); // critical value: exceeding will lead to hitting edge of board
    if (critical + length > 10)
        return false; // ship hits edge: can't place

    // try placing ship
    uint8_t offset = 1 + 9 * vertical;                                      // ship placement offset
    for (uint8_t pos = index; pos < index + length * offset; pos += offset) // iterate through squares
        if (g[pos] & M_ANY_SHIP)
            return false; // there is already a ship there!

    // the ship can be placed: place it
    for (uint8_t pos = index; pos < index + length * offset; pos += offset) // iterate through squares
        g[pos] |= 1 << type;                                                // the mask calculation is THAT SIMPLE!
    ship_root[type] = index;
    ship_offset[type] = offset;

    return true;
}

// Place all ships randomly
void place_ships_random(uint8_t *g)
{
    uint8_t type = 5;
    while (type)
    {
        type--; // decrement type here!
        bool success = false;
        while (!success)
        {
            uint16_t rng = rand();
            success = place_ship(g, type, rng % 100, !(rng & 32768)); // try placing ship in a random position
        }
    }
}
