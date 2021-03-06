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

/* skip action if piece == NULL and n == 0
 */
struct action {
	struct piece *piece;
	int n;
	struct piece *trgts[4];
};

struct action_loc {
	int piece_loc;
	int n;
	int trgts[4];
};

/* fills the swaps array with all avaliable swaps this turn.
 * returns the number of valid swaps.
 */
int generate_swaps(struct board *b, struct swap swaps[24]);

/* fills the actions array with all avaliable actions this turn.
 * returns the number of valid actions.
 */
int generate_actions(struct board *b, struct action actions[100]);

#endif
