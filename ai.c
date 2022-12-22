#include "ai.h"


uint32_t prob[100]; //Unnormalized probability grid
uint8_t tmp[100] = {}; //Temporary grid for brute force computation

//Reset probability board
void reset_prob()
{
    for (uint8_t i = 0; i < 100; i++) prob[i] = 0;
}

void print_prob()
{
    char display[] = ".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%%B@$";
    uint8_t display_length = 70;
    
    //calculate min probability value and difference between min and max
    uint32_t prob_min = -1;
    double prob_diff = 0; //-1 corresponds to 4294967295 here, it's just unsigned
    for (uint8_t i = 0; i < 100; i++)
    {
        uint32_t elt = prob[i];
        if (prob_diff < elt) prob_diff = elt; //calculate maximum
        else if (prob_min > elt) prob_min = elt; //calculate minimum
    }
    prob_diff -= prob_min; //turn maximum into max difference
    if (prob_diff < 1) prob_diff = 1; //failsafe
    prob_diff /= display_length;

    for (uint8_t i = 0; i < 100; i++)
    {
        uint8_t chr_index = (uint8_t)((prob[i] - prob_min) / prob_diff);
        if (chr_index >= display_length) chr_index = display_length - 1; //failsafe
        putchar(display[chr_index]);
        putchar((i % 10 == 9) ? '\n' : ' ');
    }
}

bool ship_brute_force(uint8_t *tgt, uint8_t *g, uint8_t type, uint8_t index, bool vertical)
{
    uint8_t length = LENGTHS[type];                            // ship length
    uint8_t critical = vertical ? (index / 10) : (index % 10); // critical value: exceeding will lead to hitting edge of board
    if (critical + length > 10)
        return false; // ship hits edge: can't place

    // try placing ship
    uint8_t offset = 1 + 9 * vertical;                                      // ship placement offset
    for (uint8_t pos = index; pos < index + length * offset; pos += offset) // iterate through squares
    {
        if (g[pos] & M_ANY_SHIP)
            return false; // there is already a ship there!
        if ((tgt[pos] & M_HIT) && !(!(tgt[pos] & M_SUNK) && !!(tgt[pos] & M_ANY_SHIP)))
            return false; // either there is a sunken ship there, or there is a miss: ship can't go there
    }

    // the ship can be placed: place it
    for (uint8_t pos = index; pos < index + length * offset; pos += offset) // iterate through squares
        g[pos] |= 1 << type;                                                // the mask calculation is THAT SIMPLE!

    return true;
}

//Try placing a ship and report result on the probability board. Returns true if placement is possible.
//Assumes current ship isn't sunken.
bool ship_prob(uint8_t *g, uint8_t type, uint8_t index, bool vertical)
{
    uint8_t length = LENGTHS[type];                            // ship length
    uint8_t critical = vertical ? (index / 10) : (index % 10); // critical value: exceeding will lead to hitting edge of board
    if (critical + length > 10)
        return false; // ship hits edge: can't place

    // try placing ship
    uint16_t multiplier = 1;                                                // if the ship has been hit more, it's more likely to be there
    uint8_t offset = 1 + 9 * vertical;                                      // ship placement offset
    for (uint8_t pos = index; pos < index + length * offset; pos += offset) // iterate through squares
        if (g[pos] & M_HIT)
        {
            if (g[pos] & M_SUNK) //we are checking a sunken ship
                return false; //assuming current ship is not sunk
            if(g[pos] & M_ANY_SHIP) //hit: increase multiplier
                multiplier += 7; //TODO: tweak value to find the best (7:1 is current best, but it's basically flat)
            //Dict with results: {1:5360194,2:5250556,3:5176133,4:4828245,5:4635436,6:4610869,7:4605931,8:4499729,9:4463241,10:4461501,11:4465640,12:4461271,13:4447356,14:4447319,15:4449067,16:4442097,17:4444499,18:4444245,19:4443575,20:4443087,21:4446538,22:4442180,23:4445019,24:4449566,25:4446331,26:4442172,27:4444763,28:4438112,29:4446331,30:4447007,31:4448937,32:4443473,33:4447042,34:4446333,35:4444073,36:4443767,37:4443866,38:4445369,39:4450915,40:4449701,41:4452980,42:4450717,43:4448031,44:4451208,45:4450153,46:4453170,47:4454406,48:4456488,49:4448101,50:4453242,51:4453986,52:4449922,53:4452645,54:4450614,55:4451166,56:4446497,57:4455200,58:4447715,59:4449148,60:4449271,61:4450195,62:4452641,63:4450581,64:4449639,65:4449355,66:4455150,67:4449735,68:4452417,69:4449509,70:4452273,71:4449842,72:4457912,73:4454380,74:4451981,75:4452039,76:4450876,77:4451930,78:4449446,79:4448632,80:4448016,81:4450623,82:4449631,83:4455959,84:4449357,85:4450611,86:4447662,87:4453777,88:4443911,89:4447533,90:4450288}
            else //miss: we know there is no ship there
                return false; //so the ship cannot be there
        }

    // the ship can be placed: add a hit on the probability board
    for (uint8_t pos = index; pos < index + length * offset; pos += offset) // iterate through squares
        prob[pos] += multiplier; //add value with multiplier

    return true;
}

void compute_ship_prob(uint8_t *g, uint8_t type)
{
    if (!ship_spot_count[type]) return; //ship has already been sunk: we shouldn't compute its probabilities

    for (uint8_t i = 0; i < 100; i++)
    {
        ship_prob(g, type, i, false);
        ship_prob(g, type, i, true);
    }
}

void compute_brute_force(uint8_t *g, uint8_t *tmp, uint8_t shipmask)
{
    if (!shipmask)
    {
        //reject a position that can't stick to the grid
        for (uint8_t i = 0; i < 100; i++)
        {
            //known info does not match current ship position
            if ((g[i] & M_HIT) && !(g[i] & M_SUNK) && (!(g[i] & M_ANY_SHIP) != !tmp[i]))
                return; //so we just return false
        }

        //add all ship tiles to the probability grid
        for (uint8_t i = 0; i < 100; i++)
        {
            prob[i] += !!(tmp[i] & M_ANY_SHIP); //add once to the probability map if ship placement valid
        }

        return;
    }

    uint8_t type = bitscan8(shipmask & -shipmask); //Extract least significant set bit and get current ship type
    shipmask &= shipmask - 1; //Remove that bit for subsequent recursive calls

    //Horizontal positions
    for (uint8_t i = 0; i < 100; i++) //cycle through all different positions for the current ship (could be improved)
    {
        bool result = ship_brute_force(g, tmp, type, i, false);
        if (result)
        {
            //recursive call
            compute_brute_force(g, tmp, shipmask);

            //remove ship
            for (uint8_t pos = i; pos < i + LENGTHS[type]; pos++)
                tmp[pos] &= ~M_ANY_SHIP; //remove all the ship tiles
        }
    }

    //Vertical positions
    for (uint8_t i = 0; i < 100 - 10 * LENGTHS[type]; i++) //cut the last bit, as it can't be used anyway
    {
        bool result = ship_brute_force(g, tmp, type, i, true);
        if (result)
        {
            //recursive call
            compute_brute_force(g, tmp, shipmask);

            //remove ship
            for (uint8_t pos = i; pos < i + LENGTHS[type] * 10; pos += 10)
                tmp[pos] &= ~M_ANY_SHIP; //remove all the ship tiles
        }
    }
}

void generate_all_prob(uint8_t *g)
{
    reset_prob();

    //Count number of remaining ships and create ship mask for brute force
    uint8_t remaining = 5;
    uint8_t shipmask = 0;
    for (uint8_t i = 0; i < 5; i++)
    {
        remaining -= !(ship_spot_count[i]);
        shipmask |= !!(ship_spot_count[i]) << i;
    }

    //Brute force method selector
    //With remaining > 2, average move count (10k samples) is 32.2201
    //With remaining > 3, average move count (1k samples) is 28.0794
    //With remaining > 4, average move count (80 samples, quite SSS) is 24.725
    //it takes a relatively long time to decide a single move, but still usable in a game (and a bug improvement in playing strength)
    if (remaining > 4) //many ships: can't use brute force method
        for (uint8_t type = 0; type < 5; type++) compute_ship_prob(g, type); //compute added probabilities for all ships
    else //few ships: can use brute force method
    {
        compute_brute_force(g, tmp, shipmask);
    }
}

uint8_t generate_best_move(uint8_t *g)
{
    generate_all_prob(g);
    uint32_t best_index = 0;
    bool invalid = true;
    for (uint8_t i = 0; i < 100; i++)
    {
        if (invalid && !(g[i] & M_HIT)) //snatch first valid square
        {
            best_index = i;
            invalid = false;
        }
        else if (!(g[i] & M_HIT) && prob[i] > prob[best_index]) best_index = i; //get highest index of a non-hit square
    }
    
    return best_index;
}
