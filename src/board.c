#include "board.h"

bool is_isolated(struct team *t)
{
#ifdef DEBUG
	assert(t);
#endif
	int count = 0;

	for (int i = 0; i < NUM_PIECES; ++i) {
		if (t->pieces[i].active)
			++count;
	}
	return count == 0;
}

bool is_king_dead(struct team *t)
{
#ifdef DEBUG
	assert(t);
	assert(t->pieces[0].type == KING);
#endif
	return t->pieces[0].hp == 0;
}

enum p_team winner(struct board *b)
{
#ifdef DEBUG
	assert(b);
#endif
	for (int i = 0; i < NUM_TEAMS; ++i) {
		if (is_king_dead(&(b->teams[i])) || b->teams[i].passes >= MAX_PASSES)
			return BLACK - i;
	}

	bool w_iso = is_isolated(&(b->teams[WHITE]));
	bool b_iso = is_isolated(&(b->teams[BLACK]));

	if (w_iso && b_iso)
		return DRAW;
	else if (w_iso)
		return BLACK;
	else if (b_iso)
		return WHITE;
	else
		return NONE;
}

static void init_team(struct team *t, enum start_pos s)
{
#ifdef DEBUG
	assert(t);
	assert(t->color != NONE);
	assert(s >= 0 && s < 4);
#endif
	t->passes = 0;
	int back_rank = t->color == BLACK ? 0 : 3;
	int offset = t->color == BLACK ? 1 : -1;

	switch (s) {
	case LEFT:
		init_piece(&(t->pieces[0]), 0, back_rank, KING, t->color);
		init_piece(&(t->pieces[1]), 1, back_rank, ARCHER, t->color);
		init_piece(&(t->pieces[2]), 2, back_rank, MEDIC, t->color);
		init_piece(&(t->pieces[3]), 3, back_rank, ARCHER, t->color);
		init_piece(&(t->pieces[4]), 0, back_rank + offset, SHIELD, t->color);
		init_piece(&(t->pieces[5]), 1, back_rank + offset, KNIGHT, t->color);
		init_piece(&(t->pieces[6]), 2, back_rank + offset, KNIGHT, t->color);
		init_piece(&(t->pieces[7]), 3, back_rank + offset, WIZARD, t->color);
		break;
	case MID_LEFT:
		init_piece(&(t->pieces[0]), 1, back_rank, KING, t->color);
		init_piece(&(t->pieces[1]), 0, back_rank, ARCHER, t->color);
		init_piece(&(t->pieces[2]), 2, back_rank, MEDIC, t->color);
		init_piece(&(t->pieces[3]), 3, back_rank, ARCHER, t->color);
		init_piece(&(t->pieces[4]), 1, back_rank + offset, SHIELD, t->color);
		init_piece(&(t->pieces[5]), 0, back_rank + offset, KNIGHT, t->color);
		init_piece(&(t->pieces[6]), 3, back_rank + offset, KNIGHT, t->color);
		init_piece(&(t->pieces[7]), 2, back_rank + offset, WIZARD, t->color);
		break;
	case MID_RIGHT:
		init_piece(&(t->pieces[0]), 2, back_rank, KING, t->color);
		init_piece(&(t->pieces[1]), 0, back_rank, ARCHER, t->color);
		init_piece(&(t->pieces[2]), 1, back_rank, MEDIC, t->color);
		init_piece(&(t->pieces[3]), 3, back_rank, ARCHER, t->color);
		init_piece(&(t->pieces[4]), 2, back_rank + offset, SHIELD, t->color);
		init_piece(&(t->pieces[5]), 0, back_rank + offset, KNIGHT, t->color);
		init_piece(&(t->pieces[6]), 3, back_rank + offset, KNIGHT, t->color);
		init_piece(&(t->pieces[7]), 1, back_rank + offset, WIZARD, t->color);
		break;
	case RIGHT:
		init_piece(&(t->pieces[0]), 3, back_rank, KING, t->color);
		init_piece(&(t->pieces[1]), 0, back_rank, ARCHER, t->color);
		init_piece(&(t->pieces[2]), 1, back_rank, MEDIC, t->color);
		init_piece(&(t->pieces[3]), 2, back_rank, ARCHER, t->color);
		init_piece(&(t->pieces[4]), 3, back_rank + offset, SHIELD, t->color);
		init_piece(&(t->pieces[5]), 1, back_rank + offset, KNIGHT, t->color);
		init_piece(&(t->pieces[6]), 2, back_rank + offset, KNIGHT, t->color);
		init_piece(&(t->pieces[7]), 0, back_rank + offset, WIZARD, t->color);
		break;
	}	
}

static void sync_sqrs(struct board *b)
{
#ifdef DEBUG
	assert(b);
#endif
	struct piece *p1, *p2;
	for (int i = 0; i < NUM_PIECES; ++i) {
		p1 = &(b->teams[WHITE].pieces[i]);
		p2 = &(b->teams[BLACK].pieces[i]);
		b->squares[XY1D(p1->x, p1->y)] = p1;
		b->squares[XY1D(p2->x, p2->y)] = p2;
	}
}

void init_board(struct board *b, enum start_pos w_s, enum start_pos b_s)
{
#ifdef DEBUG
	assert(b);
#endif
	b->turn = BLACK;
	b->teams[WHITE].color = WHITE;
	b->teams[BLACK].color = BLACK;

	init_team(&(b->teams[WHITE]), w_s);
	init_team(&(b->teams[BLACK]), b_s);
	sync_sqrs(b);
}

static void copy_team(struct team *dst, struct team *src)
{
#ifdef DEBUG
	assert(dst);
	assert(src);
#endif
	dst->passes = src->passes;
	dst->color = src->color;
	for (int i = 0; i < NUM_PIECES; ++i) {
		copy_piece(&(dst->pieces[i]), &(src->pieces[i]));
	}
}

void copy_board(struct board *dst, struct board *src)
{
#ifdef DEBUG
	assert(dst);
	assert(src);
#endif
	dst->turn = src->turn;
	for (int i = 0; i < NUM_TEAMS; ++i) {
		copy_team(&(dst->teams[i]), &(src->teams[i]));
	}

	sync_sqrs(dst);
}

void update_board(struct board *b)
{
#ifdef DEBUG
	assert(b);
#endif
	for (int i = 0; i < NUM_PIECES; ++i) {
		update_activity(&(b->teams[WHITE].pieces[i]), b->squares);
		update_activity(&(b->teams[BLACK].pieces[i]), b->squares);
	}
}

void toggle_turn(struct board *b)
{
#ifdef DEBUG
	assert(b);
#endif
	b->turn = b->turn == BLACK ? WHITE : BLACK;
}

void swap(struct board *b, int p1, int p2)
{
#ifdef DEBUG
	assert(b);
	assert(p1 >= 0);
	assert(p1 < NUM_SQRS);
	assert(p2 >= 0);
	assert(p2 < NUM_SQRS);
#endif
	swap_pieces(b->squares[p1], b->squares[p2], b->squares);
}

bool can_swap(struct board *b, struct piece *p1, struct piece *p2) 
{
#ifdef DEBUG
	assert(b);
#endif
	return (p1->team == b->turn && can_move(p1, p2->x, p2->y, b->squares))
		|| (p2->team == b->turn && can_move(p2, p1->x, p1->y, b->squares));
}

void apply_action(struct board *b, struct piece *p, struct piece **trgts, int len)
{
#ifdef DEBUG
	assert(b);
#endif
	if (!p && len == 0) {
		// skip action
		b->teams[b->turn].passes += 1;
		return;
	}
#ifdef DEBUG
	if (!p)
		assert(0);
#endif
	use_action(p, trgts, len, b->squares);
	b->teams[b->turn].passes = 0;
}

bool can_apply_action(struct board *b, struct piece *p, struct piece **trgts, int len)
{
#ifdef DEBUG
	assert(b);
#endif
	return (!p && !len) || (p->team == b->turn && can_action(p, trgts, len, b->squares));
}

static char char_piece(struct piece *p)
{
#ifdef DEBUG
	assert(p);
#endif
	char c;

	switch (p->type) {
	case EMPTY:
		c = 'x';
		break;
	case KING:
		c = 'k';
		break;
	case WIZARD:
		c = 'w';
		break;
	case KNIGHT:
		c = 'n';
		break;
	case ARCHER:
		c = 'a';
		break;
	case MEDIC:
		c = 'm';
		break;
	case SHIELD:
		c = 's';
		break;
	}

	if (p->team == BLACK || p->team == WHITE)
		c = p->team == BLACK ? c : c + 'A' - 'a';
	return c;
}

static char char_active(struct piece *p)
{
#ifdef DEBUG
	assert(p);
#endif
	return p->active ? '*' : '.';
}

void print_board(struct board *b)
{
#ifdef DEBUG
	assert(b);
#endif
	struct piece *p;
	printf(" |  a    b    c    d\n======================\n");
	for (int i = 0; i < SIZE; ++i) {
		printf(" |");
		for (int j = 0; j < SIZE; ++j) {
			printf("+---+");
		}
		printf("\n |");
		for (int j = 0; j < SIZE; ++j) {
			printf("|   |");
			char_piece(b->squares[XY1D(j, i)]);
		}
		printf("\n%d|", i + 1);
		for (int j = 0; j < SIZE; ++j) {
			printf("| %c |", char_piece(b->squares[XY1D(j, i)]));
		}
		printf("\n |");
		for (int j = 0; j < SIZE; ++j) {
			p = b->squares[XY1D(j, i)];
			printf("|%d %c|", p->hp, char_active(p));
		}
		printf("\n |");
		for (int j = 0; j < SIZE; ++j) {
			printf("+---+");
		}
		printf("\n");
	}
}
