#include <assert.h>
#include "generate.h"

int simple_generate_swaps(struct board *b)
{
	int n;
	struct swap swaps[24];

	init_board(b, LEFT, LEFT);
	// simplfy the board
	for (int j = 0; j < 3; ++j) {
		for (int i = 1; i < 4; ++i) {
			init_piece(b->squares[XY1D(i, j)], i, j, EMPTY, NONE);
		}
	}

	init_piece(b->squares[XY1D(2, 3)], 2, 3, EMPTY, NONE);
	
	update_board(b);

	// generate swaps for black
	n = generate_swaps(b, swaps);

	assert(n == 1);
	assert(swaps[0].from == 0 && swaps[0].to == 4);

	toggle_turn(b);

	// generate swaps for white
	n = generate_swaps(b, swaps);

	assert(n == 2);
	assert(swaps[0].from == 8 && swaps[0].to == 12);
	assert(swaps[1].from == 12 && swaps[1].to == 13);

	return 0;
}

int main(void)
{
	struct board b;

	if (simple_generate_swaps(&b) != 0) {
		printf("test_generate_swaps failed\n");
	} else {
		printf("test_generate_swaps passed\n");
	}

	return 0;
}
