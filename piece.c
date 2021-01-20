#include "piece.h"

const int MY_VECX[4] = {0, 1, 0, -1};
const int MY_VECY[4] = {1, 0, -1, 0};

struct piece *init_piece(struct piece *p, int x, int y, enum p_type type, enum p_team team)
{
#ifdef DEBUG
	assert(p);
	assert(x >= 0);
	assert(x < SIZE);
	assert(y >= 0);
	assert(y < SIZE);
	if (type == EMPTY || team == NONE)
		assert(type == EMPTY && team == NONE);
#endif
	p->x = x;
	p->y = y;
	p->swapable = true;
	p->blocks = false;
	p->active = true;
	p->type = type;
	p->team = team;

	switch (type) {
	case KING:
		p->max_hp = 4;
		p->hp = 4;
		break;
	case SHIELD:
		p->max_hp = 4;
		p->hp = team == WHITE ? 4 : 3;
		p->swapable = false;
		p->blocks = true;
		break;
	case EMPTY:
		p->max_hp = 0;
		p->hp = 0;
		p->swapable = false;
		p->active = false;
		break;
	default:
		p->max_hp = 3;
		p->hp = 3;
	}
	
	return p;
}

bool inbounds(int x, int y)
{
	return 0 <= x && x < SIZE && 0 <= y && y < SIZE; 
}

static inline int abs(int x)
{
	return x >= 0 ? x : -x;
}

static inline bool adjacent(int x1, int y1, int x2, int y2)
{
	return ((x1 == x2 - 1 || x1 == x2 + 1 || x1 == x2) && (y1 == y2 - 1 || y1 == y2 + 1 || y1 == y2))
			&& (abs(x1 - x2) + abs(y1 - y2) == 1);
}

bool can_move(struct piece *p, int x, int y, struct piece *pieces[NUM_SQRS])
{
#ifdef DEBUG
	assert(p);
#endif
	return (inbounds(x, y) && adjacent(p->x, p->y, x, y) && p->active
			&& (pieces[XY1D(x, y)]->team == p->team || pieces[XY1D(x, y)]->swapable));
}

bool can_dmg(struct piece *p1, struct piece *p2) 
{
#ifdef DEBUG
	assert(p1);
	assert(p1->type != EMPTY);
	assert(p2);
#endif
	return p2->type != EMPTY && p2->team != p1->team;
}

bool can_heal(struct piece *p)
{
#ifdef DEBUG
	assert(p);
#endif
	return (0 < p->hp && p->hp < p->max_hp);
}

static bool can_king_action(struct piece *p, struct piece **trgts, int len)
{
#ifdef DEBUG
	assert(p);
	assert(p->type == KING);
	assert(trgts);
	assert(len > 0);
	for (int i = 0; i < len; ++i)
		assert(trgts[i]);
#endif
	if (len != 1)
		return false;
	struct piece *p2 = trgts[0];
	return can_dmg(p, p2) && adjacent(p->x, p->y, p2->x, p2->y);
}

static bool can_wizard_action(struct piece *p, struct piece **trgts, int len)
{
#ifdef DEBUG
	assert(p);
	assert(p->type == WIZARD);
	assert(trgts);
	assert(len > 0);
	for (int i = 0; i < len; ++i)
		assert(trgts[i]);
#endif
	if (len != 1)
		return false;
	struct piece *p2 = trgts[0];
	return (p2->type != EMPTY && p2->team == p->team && p2 != p); 
}

static bool can_knight_action(struct piece *p, struct piece **trgts, int len)
{
#ifdef DEBUG
	assert(p);
	assert(p->type == KNIGHT);
	assert(trgts);
	assert(len > 0);
	for (int i = 0; i < len; ++i)
		assert(trgts[i]);
#endif
	if (len <= 0 || len > 2)
		return false;

	struct piece *p2;
	for (int i = 0; i < len; ++i) {
		p2 = trgts[i];
		if (!can_dmg(p, p2) || !adjacent(p->x, p->y, p2->x, p2->y))
			return false;
	}
	return true;
}

static bool can_archer_action(struct piece *p, struct piece **trgts, int len, struct piece *pieces[NUM_SQRS])
{
#ifdef DEBUG
	assert(p);
	assert(p->type == ARCHER);
	assert(trgts);
	assert(len > 0);
	for (int i = 0; i < len; ++i)
		assert(trgts[i]);
#endif
	if (len != 1)
		return false;

	struct piece *p2 = trgts[0];
#ifdef DEBUG
	assert(p != p2);
#endif
	if (p2->type == EMPTY || p2->team == p->team || (p2->x != p->x && p2->y != p->y))
		return false;

	int dir, sqr;
	if (p2->x == p->x) {
		dir = p2->y - p->y > 0 ? 1 : -1;
		for (int i = 1; i < abs(p2->y - p->y); ++i) {
#ifdef DEBUG
			assert(inbounds(p->x, p->y + i * dir));
#endif
			sqr = (p->y + i * dir) * SIZE + p->x;
			if (pieces[sqr]->team != p->team && pieces[sqr]->blocks)
				return false;
		}
	} else {
		dir = p2->x - p->x > 0 ? 1 : -1;
		for (int i = 1; i < abs(p2->x - p->x); ++i) {
#ifdef DEBUG
			assert(inbounds(p->x + i * dir, p->y));
#endif
			sqr = p->y * SIZE + p->x + (i * dir);
			if (pieces[sqr]->team != p->team && pieces[sqr]->blocks)
				return false;
		}
	}

	return true;
}

static bool can_medic_action(struct piece *p, struct piece **trgts, int len)
{
#ifdef DEBUG
	assert(p);
	assert(p->type == MEDIC);
	assert(trgts);
	assert(len > 0);
	for (int i = 0; i < len; ++i)
		assert(trgts[i]);
#endif
	if (len < 0 || len > 4)
		return false;

	struct piece *p2;
	for (int i = 0; i < len; ++i) {
		p2 = trgts[i];
		if (p2->type == EMPTY || p2->team != p->team || !adjacent(p->x, p->y, p2->x, p2->y) || !can_heal(p2))
			return false;
	}
	return true;
}

bool can_action(struct piece *p, struct piece **trgts, int len, struct piece *pieces[NUM_SQRS])
{
#ifdef DEBUG
	assert(p);
	assert(trgts);
	assert(len > 0);
	for (int i = 0; i < len; ++i)
		assert(trgts[i]);
#endif
	if (!p->active)
		return false;
	bool ret;

	switch (p->type) {
	case EMPTY:
	case SHIELD:
		ret = false;
		break;
	case KING:
		ret = can_king_action(p, trgts, len);
		break;
	case WIZARD:
		ret = can_wizard_action(p, trgts, len);
		break;
	case KNIGHT:
		ret = can_knight_action(p, trgts, len);
		break;
	case ARCHER:
		ret = can_archer_action(p, trgts, len, pieces);
		break;
	case MEDIC:
		ret = can_medic_action(p, trgts, len);
		break;
	}

	return ret;
}

void move(struct piece *p, int x, int y)
{
#ifdef DEBUG
	assert(p);
	assert(inbounds(x, y));
#endif
	p->x = x;
	p->y = y;
}

void take_dmg(struct piece *p, int dmg)
{
#ifdef DEBUG
	assert(p);
	assert(dmg > 0);
#endif
	p->hp -= dmg;
	if (p->hp <= 0 && p->type != KING) {
		init_piece(p, p->x, p->y, EMPTY, NONE);
	}
}

void heal(struct piece *p, int amt)
{
#ifdef DEBUG
	assert(p);
	assert(amt > 0);
#endif
	p->hp = p->hp + amt < p->max_hp ? p->hp + amt : p->max_hp;
}

void swap_pieces(struct piece *p1, struct piece *p2, struct piece *pieces[NUM_SQRS])
{
#ifdef DEBUG
	assert(p1);
	assert(p2);
#endif
	int tmp_x = p1->x;
	int tmp_y = p1->y;

	move(p1, p2->x, p2->y);
	move(p2, tmp_x, tmp_y); 

	pieces[XY1D(p1->x, p1->y)] = p1;
	pieces[XY1D(p2->x, p2->y)] = p2;
}

void use_action(struct piece *p, struct piece **trgts, int len, struct piece *pieces[NUM_SQRS])
{
#ifdef DEBUG
	assert(can_action(p, trgts, len, pieces));
#endif

	switch (p->type) {
#ifdef DEBUG
	case EMPTY:
	case SHIELD:
		assert(0);
#endif
	case KING:
	case ARCHER:
		take_dmg(trgts[0], 1);
		break;
	case WIZARD:
		swap_pieces(p, trgts[0], pieces);
		break;
	case KNIGHT:
		for (int i = 0; i < len; ++i) {
			take_dmg(trgts[i], 1);
		}
		break;
	case MEDIC:
		for (int i = 0; i < len; ++i) {
			heal(trgts[i], 1);
		}
		break;
	}
}

void update_activity(struct piece *p, struct piece *pieces[NUM_SQRS])
{
#ifdef DEBUG
	assert(p);
#endif
	if (p->type == EMPTY)
		return;
	int x, y;
	for (int i = 0; i < 4; ++i) {
		x = p->x + MY_VECX[i];
		y = p->y + MY_VECY[i];
		if (inbounds(x, y) && pieces[XY1D(x, y)]->team == p->team) {
			p->active = true;
			return;
		}
	}
	p->active = false;
}

struct piece *piece_at(int x, int y, struct piece *pieces[NUM_SQRS])
{
#ifdef DEBUG
	assert(inbounds(x, y));
	assert(pieces);
#endif
	return pieces[XY1D(x, y)];
}
