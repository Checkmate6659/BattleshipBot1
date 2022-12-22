#ifndef AI_H
#define AI_H

#include <stdio.h>
#include "board.h"


void reset_prob();
void print_prob();
bool ship_brute_force(uint8_t *tgt, uint8_t *g, uint8_t type, uint8_t index, bool vertical);
bool ship_prob(uint8_t *g, uint8_t type, uint8_t index, bool vertical);
void compute_ship_prob(uint8_t *g, uint8_t type);
void compute_brute_force(uint8_t *g, uint8_t *tmp, uint8_t shipmask);
void generate_all_prob(uint8_t *g);
uint8_t generate_best_move(uint8_t *g);

#endif
