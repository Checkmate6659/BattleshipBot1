#ifndef BOARD_H
#define BOARD_H

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

enum {
    //Ship types
    SUBMARINE = 0,
    CRUISER = 1,
    DESTROYER = 2,
    BATTLESHIP = 3,
    CARRIER = 4,

    //Masks
    M_SUBMARINE = 1,
    M_CRUISER = 2,
    M_DESTROYER = 4,
    M_BATTLESHIP = 8,
    M_CARRIER = 16,
    M_ANY_SHIP = 31,
    M_HIT = 32,
    M_SUNK = 64,

    //Ship lengths
    L_SUBMARINE = 3,
    L_CRUISER = 3,
    L_DESTROYER = 2,
    L_BATTLESHIP = 4,
    L_CARRIER = 5,
};

extern const uint8_t LENGTHS[]; //Lengths of ships
extern uint8_t ship_spot_count[]; //to know which ships have been sank already
extern uint8_t ship_root[5];
extern uint8_t ship_offset[5];


uint8_t bitscan8(uint8_t x);
void reset_game(uint8_t *g);
bool hit(uint8_t *g, uint8_t index);
bool place_ship(uint8_t *g, uint8_t type, uint8_t index, bool vertical);
void place_ships_random(uint8_t *g);

#endif
