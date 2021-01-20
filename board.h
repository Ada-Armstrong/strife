#ifndef FEUD_BOARD_H
#define FEUD_BOARD_H

#include <stdbool.h>
#include <stdio.h>
#include "piece.h"
#ifdef DEBUG
#include <assert.h>
#endif

#define MAX_PASSES 3

struct team {
	int passes;
	enum p_team color;
	struct piece pieces[NUM_PIECES];
};

struct board {
	enum p_team turn;
	struct team teams[NUM_TEAMS];
	struct piece *squares[NUM_SQRS];
};

enum start_pos {
	LEFT,
	MID_LEFT,
	MID_RIGHT,
	RIGHT
};

/* returns true if the team is isolated
 */
bool is_isolated(struct team *t);

/* returns true if the king is dead
 */
bool is_king_dead(struct team *t);

/* returns the winner
 */
enum p_team winner(struct board *b);

/* configures the board with the starting positions. Black to play first
 */
void init_board(struct board *b, enum start_pos w_s, enum start_pos b_s);

/* copies src to dst
 */
void copy_board(struct board *dst, struct board *src);

/* updates the activity flag of the pieces on the board. Call after a swap
 */
void update_board(struct board *b);

/* changes which team's turn it is
 */
void toggle_turn(struct board *b);

/* swaps the pieces at p1 and p2 (note p1 and p2 are the 1D coordinates)
 */
void swap(struct board *b, int p1, int p2);

/* prints the current state of the board
 */
void print_board(struct board *b);

#endif
