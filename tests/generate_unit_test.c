#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "generate.h"

#define NAME_BUFF 21

struct unit_test {
	int (*test)(struct board *);
	char name[NAME_BUFF];
};

struct tests {
	unsigned int i;
	unsigned int n;
	struct unit_test *t;
};


void setup_tests(struct tests *tst, unsigned int n_tests)
{
	assert(tst);
	tst->i = 0;
	tst->n = n_tests;
	tst->t = malloc(sizeof(*(tst->t)) * n_tests);
}

void add_test(struct tests *tst, char *name, int (*test)(struct board *))
{
	assert(tst);
	assert(strlen(name) < NAME_BUFF);
	assert(tst->i < tst->n);

	tst->t[tst->i].test = test;
	strcpy(tst->t[tst->i].name, name);
	tst->i += 1;
}

void run_tests(struct tests *tst)
{
	assert(tst);
	assert(tst->i);

	struct board b;
	int passed = 0;

	printf("Running %d tests\n", tst->i);

	for (int i = 0; i < tst->i; ++i) {
		if (tst->t[i].test(&b) != 0) {
			printf("%s: failed\n", tst->t[i].name);
		} else {
			++passed;
			printf("%s: passed\n", tst->t[i].name);
		}
	}
	
	printf("Passed %d/%d tests\n", passed, tst->i);
}

int simple_position(struct board *b);
int knight_actions(struct board *b);
int mid_game_bug(struct board *b);


int main(void)
{
	const int n_tests = 3;
	struct tests all_tests;

	setup_tests(&all_tests, n_tests);

	add_test(&all_tests, "simple_position", simple_position);
	add_test(&all_tests, "knight_actions", knight_actions);
	add_test(&all_tests, "mid_game_bug", mid_game_bug);

	run_tests(&all_tests);

	return 0;
}

int simple_position(struct board *b)
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

	// generate actions for white
	struct action actions[100];

	n = generate_actions(b, actions);

	assert(n == 0);

	toggle_turn(b);

	// generate actions for black
	n = generate_actions(b, actions);

	assert(n == 0);

	return 0;
}

int knight_actions(struct board *b)
{
	int n;
	struct action actions[100];

	init_board(b, LEFT, LEFT);

	swap(b, 1, 5); // place black knight at b1
	swap(b, 9, 5); // place white knight at b2
	swap(b, 12, 9); // place white king at b3

	// simplfy the board, remove c and d columns
	for (int j = 0; j < 4; ++j) {
		for (int i = 2; i < 4; ++i) {
			init_piece(b->squares[XY1D(i, j)], i, j, EMPTY, NONE);
		}
	}

	init_piece(b->squares[XY1D(0, 3)], 0, 3, EMPTY, NONE);
	init_piece(b->squares[XY1D(1, 3)], 1, 3, EMPTY, NONE);

	update_board(b);

	// generate actions for black
	n = generate_actions(b, actions);

	assert(n == 1);

	toggle_turn(b);

	// generate actions for white
	n = generate_actions(b, actions);

	assert(n == 3);

	const int exp_total_trgts = 3;
	int total_trgts = 0;

	for (int i = 0; i < n; ++i) {
		assert(actions[i].piece->type == KNIGHT);
		assert(actions[i].piece->team == WHITE);
		assert(actions[i].piece->x == 1);
		assert(actions[i].piece->y == 1);
		assert(0 < actions[i].n && actions[i].n < 3);
		total_trgts += actions[i].n;
	}

	return 0;
}

int mid_game_bug(struct board *b)
{
	int n;
	struct action actions[100];

	init_board(b, MID_RIGHT, MID_LEFT);

	init_piece(b->squares[XY1D(0, 2)], 0, 2, EMPTY, NONE);
	init_piece(b->squares[XY1D(0, 1)], 0, 1, EMPTY, NONE);
	init_piece(b->squares[XY1D(2, 0)], 2, 0, EMPTY, NONE);
	swap(b, 3, 11); 
	swap(b, 8, 9); 
	swap(b, 13, 9); 
	swap(b, 13, 7); 

	b->squares[XY1D(3, 0)]->hp = 2;
	b->squares[XY1D(1, 1)]->hp = 4;
	b->squares[XY1D(2, 2)]->hp = 2;
	b->squares[XY1D(1, 3)]->hp = 2;
	b->squares[XY1D(2, 3)]->hp = 2;

	update_board(b);

	// generate actions for black
	n = generate_actions(b, actions);

	assert(n == 8);

	return 0;
}
