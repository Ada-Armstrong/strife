#include "generate.h"

static inline int min(int a, int b)
{
	return a < b ? a : b;
}

static inline int max(int a, int b)
{
	return a < b ? b : a;
}

struct adj {
	int n;
	int neighbours[2];
};

static void piece_generate_swaps(struct piece *p, struct board *b, struct adj swaps[NUM_SQRS], int visited[NUM_SQRS])
{
#ifdef DEBUG
	assert(p);
	assert(b);
#endif
	if (!p->active)
		return;

	int x, y;
 	int m_sqr;
	int p_sqr = XY1D(p->x, p->y);
	if (visited[p_sqr])
		return;
	visited[p_sqr] = 1;

	for (int i = 0; i < 4; ++i) {
		x = p->x + MY_VECX[i];
		y = p->y + MY_VECY[i];
		if (!inbounds(x, y))
			continue;

		m_sqr = XY1D(x, y);
		if (visited[m_sqr])
			continue;

		if (can_move(p, x, y, b->squares)) {
			swaps[min(p_sqr, m_sqr)].neighbours[swaps[min(p_sqr, m_sqr)].n] = max(p_sqr, m_sqr);
			swaps[min(p_sqr, m_sqr)].n += 1;
		}
	}
}

int generate_swaps(struct board *b, struct swap swaps[24])
{
#ifdef DEBUG
	assert(b);
#endif
	struct adj adj_list[NUM_SQRS];
	int visited[NUM_SQRS];
	for (int i = 0; i < NUM_SQRS; ++i) {
		adj_list[i].n = 0; 
		visited[i] = 0;
	}

	// add unqiue swaps to adj_list
	for (int i = 0; i < NUM_PIECES; ++i) {
		piece_generate_swaps(&(b->teams[b->turn].pieces[i]), b, adj_list, visited);
	}

	// convert adj_list to swaps array
	int count = 0;
	for (int i = 0; i < NUM_SQRS; ++i) {
		if (adj_list[i].n) {
			for (int j = 0; j < adj_list[i].n; ++j) {
				swaps[count].from = i;
				swaps[count].to = adj_list[i].neighbours[j];
				++count;
			}
		}
	}
	return count;
}

static void add_k_ele_subsets(int k, struct piece *p, struct piece *trgts[4], int n, struct action actions[100], int *count)
{
#ifdef DEBUG
	assert(p);
	assert(count);
#endif
	for (int i = pow(2, k) - 1; i <= pow(2, n) - pow(2, n - k);) {
		int c = 0;

		for (int j = 0; j < n; ++j) {
			if (i & (1 << j)) {
				actions[*count].trgts[c++] = trgts[j];
			}
		}

		actions[*count].piece = p;
		actions[*count].n = c;
		*count += 1;
		int u = i & (-i);
		int v = i + u;
		i = v + (((v ^ i) / u) >> 2);
	}
}

static void king_generate_actions(struct piece *p, struct board *b, struct action actions[100], int *count)
{
#ifdef DEBUG
	assert(p);
	assert(b);
	assert(count);
#endif
	int x, y;
	struct piece *p2;
	for (int i = 0; i < MAX_NEIGHBOURS; ++i) {
		x = p->x + MY_VECX[i];
		y = p->y + MY_VECY[i];
		if (!inbounds(x, y))
			continue;
		p2 = piece_at(x, y, b->squares);
		if (can_dmg(p, p2)) {
			actions[*count].piece = p;
			actions[*count].n = 1;
			actions[*count].trgts[0] = p2;
			*count += 1;
		}
	}
}

static void wizard_generate_actions(struct piece *p, struct board *b, struct action actions[100], int *count)
{
#ifdef DEBUG
	assert(p);
	assert(b);
	assert(count);
#endif
	struct piece *p2;
	for (int i = 0; i < NUM_PIECES; ++i) {
		p2 = &(b->teams[b->turn].pieces[i]);
		if (p == p2 || p2->type == EMPTY)
			continue;
		actions[*count].piece = p;
		actions[*count].n = 1;
		actions[*count].trgts[0] = p2;
		*count += 1;
	}
}

static void knight_generate_actions(struct piece *p, struct board *b, struct action actions[100], int *count)
{
#ifdef DEBUG
	assert(p);
	assert(b);
	assert(count);
#endif
	int x, y;
	int n = 0;
	struct piece *p2;
	struct piece *trgts[4];
	// find all possible targets
	for (int i = 0; i < MAX_NEIGHBOURS; ++i) {
		x = p->x + MY_VECX[i];
		y = p->y + MY_VECY[i];
		if (!inbounds(x, y))
			continue;
		p2 = piece_at(x, y, b->squares);
		if (can_dmg(p, p2)) {
			trgts[n++] = p2;
		}
		
	}

	for (int k = 1; k <= min(2, n); ++k) {
		add_k_ele_subsets(k, p, trgts, n, actions, count);
	}
}

// if opt == 0, check horizontal
// else vertical
static void archer_dir_actions(int opt, int dir, struct piece *p, struct board *b, struct action actions[100], int *count)
{
#ifdef DEBUG
	assert(p);
	assert(b);
	assert(count);
	assert(dir == 1 || dir == -1);
#endif
	int x = p->x;
	int y = p->y;
	struct piece *p2;

	for (int i = 1; i < SIZE; ++i) {
		x = opt == 0 ? p->x + dir * i : x;
		y = opt == 0 ? y : p->y + dir * i;
		if (!inbounds(x, y))
			break;
		p2 = piece_at(x, y, b->squares);
		if (can_dmg(p, p2)) {
			actions[*count].piece = p;
			actions[*count].n = 1;
			actions[*count].trgts[0] = p2;
			*count += 1;
			if (p2->blocks)
				break;
		}
	}

}

static void archer_generate_actions(struct piece *p, struct board *b, struct action actions[100], int *count)
{
#ifdef DEBUG
	assert(p);
	assert(b);
	assert(count);
#endif
	// to the left
	archer_dir_actions(0, -1, p, b, actions, count);
	// right
	archer_dir_actions(0, 1, p, b, actions, count);
	// up
	archer_dir_actions(1, 1, p, b, actions, count);
	// down
	archer_dir_actions(1, -1, p, b, actions, count);
}

static void medic_generate_actions(struct piece *p, struct board *b, struct action actions[100], int *count)
{
#ifdef DEBUG
	assert(p);
	assert(b);
	assert(count);
#endif
	int x, y;
	int n = 0;
	struct piece *p2;
	struct piece *trgts[4];
	// find all possible targets
	for (int i = 0; i < MAX_NEIGHBOURS; ++i) {
		x = p->x + MY_VECX[i];
		y = p->y + MY_VECY[i];
		if (!inbounds(x, y))
			continue;
		p2 = piece_at(x, y, b->squares);
		if (p->team == p2->team && can_heal(p2)) {
			trgts[n++] = p2;
		}
		
	}
	// all subsets of targets up to size 4
	for (int k = 1; k <= min(4, n); ++k) {
		add_k_ele_subsets(k, p, trgts, n, actions, count);
	}
}

static void piece_generate_actions(struct piece *p, struct board *b, struct action actions[100], int *count)
{
#ifdef DEBUG
	assert(p);
	assert(b);
	assert(count);
#endif
	if (!p->active || p->type == SHIELD || p->type == EMPTY)
		return;
	
	switch (p->type) {
	case KING:
		king_generate_actions(p, b, actions, count);
		break;
	case WIZARD:
		wizard_generate_actions(p, b, actions, count);
		break;
	case KNIGHT:
		knight_generate_actions(p, b, actions, count);
		break;
	case ARCHER:
		archer_generate_actions(p, b, actions, count);
		break;
	case MEDIC:
		medic_generate_actions(p, b, actions, count);
		break;
#ifdef DEBUG
	default:
		assert(0);
#endif
	}
}

int generate_actions(struct board *b, struct action actions[100])
{
#ifdef DEBUG
	assert(b);
#endif
	int count = 1;
	actions[0].piece = NULL;
	actions[0].n = 0;

	for (int i = 0; i < NUM_PIECES; ++i) {
		piece_generate_actions(&(b->teams[b->turn].pieces[i]), b, actions, &count);
	}
	return count;
}
