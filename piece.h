#ifndef FEUD_PIECE_H
#define FEUD_PIECE_H

#include <stdbool.h>
#ifdef DEBUG
#include <assert.h>
#endif

#define SIZE 4
#define NUM_SQRS (SIZE * SIZE)
#define NUM_PIECES 8
#define MAX_NEIGHBOURS 4

#define XY1D(x, y) (y * SIZE + x)

enum p_type {
	EMPTY = -1,
	KING = 0,
	WIZARD,
	KNIGHT,
	ARCHER,
	MEDIC,
	SHIELD
};

enum p_team {
	DRAW = -2,
	NONE = -1,
	WHITE = 0,
	BLACK,
	NUM_TEAMS
};

struct piece {
	int max_hp;
	int hp;
	int x;
	int y;
	bool swapable;
	bool blocks;
	bool active;
	enum p_type type;
	enum p_team team;
};

extern const int MY_VECX[4];
extern const int MY_VECY[4];

/* initializes the piece
 */
struct piece *init_piece(struct piece *p, int x, int y, enum p_type type, enum p_team team);

/* returns whether (x, y) is on the board
 */
bool inbounds(int x, int y);

/* return true if move is valid
 */
bool can_move(struct piece *p, int x, int y, struct piece *pieces[NUM_SQRS]);

/* return true if the p1 can dmg the p2
 * note doesn't check if the pieces are adjacent
 */
bool can_dmg(struct piece *p1, struct piece *p2);

/* return true if the piece can be healed, false otherwise
 */
bool can_heal(struct piece *p);

/* return true if the piece can use action on all targets
 */
bool can_action(struct piece *p, struct piece **trgts, int len, struct piece *pieces[NUM_SQRS]);

/* changes the x and y coordinates of piece,
 * can_move should be called first
 */
void move(struct piece *p, int x, int y);

/* dmg is subtracted from the piece's hp,
 * if the piece's hp goes below 0 the piece is replaced by an empty sqr
 */
void take_dmg(struct piece *p, int dmg);

/* heals the piece for amt, health cannot exceed max_hp
 */
void heal(struct piece *p, int amt);

/* swaps the locations of p1 and p2 on the board
 */
void swap_pieces(struct piece *p1, struct piece *p2, struct piece *pieces[NUM_SQRS]);

/* use the piece's action on all targets
 */
void use_action(struct piece *p, struct piece **trgts, int len, struct piece *pieces[NUM_SQRS]);

/* updates the active flag of the piece
 */
void update_activity(struct piece *p, struct piece *pieces[NUM_SQRS]);

/* returns the piece at (x, y)
 */
struct piece *piece_at(int x, int y, struct piece *pieces[NUM_SQRS]);

#endif
