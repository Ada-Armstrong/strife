#ifndef FEUD_GENERATE_H
#define FEUD_GENERATE_H

#include <math.h>
#include "board.h"

#ifdef DEBUG
#include <assert.h>
#endif

struct swap {
	int from;
	int to;
};

struct action {
	struct piece *piece;
	int n;
	struct piece *trgts[4];
};

/* Fills the swaps array with all avaliable swaps this turn.
 * Returns the number of valid swaps.
 */
int generate_swaps(struct board *b, struct swap swaps[24]);

/* Fills the actions array with all avaliable actions this turn.
 * Returns the number of valid actions.
 */
int generate_actions(struct board *b, struct action actions[100]);

#endif
