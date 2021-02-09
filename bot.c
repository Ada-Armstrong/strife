#include "bot.h"

#define MIN(a, b) ( a < b ? a : b )

static int evaluate_piece(struct piece *p, struct board *b)
{
#ifdef DEBUG
	assert(p);
	assert(b);
#endif
	if (p->type == EMPTY)
		return 0;

	int val;
	int x, y;
	int n_neigh = 0, n_empty = 0, n_team = 0;
	struct piece *p2;

	for (int i = 0; i < MAX_NEIGHBOURS; ++i) {
		x = p->x + MY_VECX[i];
		y = p->y + MY_VECY[i];
		if (!inbounds(x, y))
			continue;

		p2 = b->squares[XY1D(x, y)];

		++n_neigh;
		if (p2->type == EMPTY)
			++n_empty;
		else if (p->team == p2->team)
			++n_team;
	}

	int diff = n_neigh - n_empty;

	if (!p->active && p->type != KING)
		return diff + (diff > 0) * p->hp;

	switch (p->type) {
	case KING:
		if (p->hp == 0) {
			val = INT_MIN;
			break;
		} else if (!p->active) {
			val = diff + (diff > 0) * p->hp;
			break;
		}
	case WIZARD:
	case KNIGHT:
	case ARCHER:
	case MEDIC:
	case SHIELD:
		val = (p->hp > 1) * n_team + diff + p->hp * p->hp;
		break;
	}

	return val;
}

static int evaluate(struct board *b, enum p_team mine, enum p_team them)
{
#ifdef DEBUG
	assert(b);
	assert(mine != them);
#endif
	int val = 0, tmp;

	for (int i = 0; i < NUM_PIECES; ++i) {
		tmp = evaluate_piece(&(b->teams[mine].pieces[i]), b);
		if (tmp == INT_MIN) {
			val = tmp;
			break;
		}
		val += tmp;
		tmp = evaluate_piece(&(b->teams[them].pieces[i]), b);
		if (tmp == INT_MIN) {
			val = INT_MAX;
			break;
		}
		val -= tmp;
	}

	return val;
}

static int evaluate_one_deep(struct board *b)
{
#ifdef DEBUG
	assert(b);
#endif
	enum p_team my_team = b->turn;
	enum p_team enemy_team = 1 - my_team;

	struct swap swaps[24];
	struct action actions[100];

	bool me_iso, them_iso;

	struct board cpy, cpy2;

	int tmp;
	int s_index;

	int n_swaps = generate_swaps(b, swaps);

	int score;

	for (int i = 0; i < n_swaps; ++i) {
		copy_board(&cpy, b);

		swap(&cpy, swaps[i].from, swaps[i].to);
		update_board(&cpy);
		
		me_iso = is_isolated(&(cpy.teams[my_team]));
		them_iso = is_isolated(&(cpy.teams[enemy_team]));

		if (me_iso && them_iso) {
			// draw
			tmp = 0;
			score = MIN(score, tmp);
			continue;
		} else if (me_iso) {
			// lose
			tmp = INT_MIN;
			score = MIN(score, tmp);
			continue;
		} else if (them_iso) {
			// win
			tmp = INT_MAX;
			score = MIN(score, tmp);
			continue;
		}

		copy_board(&cpy2, &cpy);
		int n_actions = generate_actions(&cpy2, actions);
		for (int j = 0; j < n_actions; ++j) {
			use_action(actions[j].piece, actions[j].trgts, actions[j].n, cpy2.squares);
			update_board(&cpy2);

			tmp = evaluate(&cpy2, my_team, enemy_team);
			score = MIN(score, tmp);
			copy_board(&cpy2, &cpy);
		}
	}

	return score;
}

void bot_turn(struct board *b, struct swap *s, struct action_loc *a, int flg)
{
#ifdef DEBUG
	assert(b);
	assert(s);
	assert(a);
#endif
	if (flg != 0)
		return;
	enum p_team my_team = b->turn;
	enum p_team enemy_team = 1 - my_team;

	struct swap swaps[24];
	struct action actions[100];

	bool me_iso, them_iso;

	struct board cpy, cpy2;

	int max = INT_MIN, tmp;
	int s_index;

	int n_swaps = generate_swaps(b, swaps);

	for (int i = 0; i < n_swaps; ++i) {
		copy_board(&cpy, b);

		swap(&cpy, swaps[i].from, swaps[i].to);
		update_board(&cpy);
		
		me_iso = is_isolated(&(cpy.teams[my_team]));
		them_iso = is_isolated(&(cpy.teams[enemy_team]));

		if (me_iso && them_iso) {
			// draw
			tmp = 0;
		} else if (me_iso) {
			// lose
			tmp = INT_MIN;
		} else if (them_iso) {
			// win
			tmp = INT_MAX;
			s->from = swaps[i].from;
			s->to = swaps[i].to;
			return;
		}

		copy_board(&cpy2, &cpy);
		int n_actions = generate_actions(&cpy2, actions);
		for (int j = 0; j < n_actions; ++j) {
			use_action(actions[j].piece, actions[j].trgts, actions[j].n, cpy2.squares);
			update_board(&cpy2);

			tmp = evaluate_one_deep(&cpy2);
			if (tmp == INT_MAX) {
				// win
				s_index = i;
				// action
				a->piece_loc = actions[j].piece->y * SIZE + actions[j].piece->x;
				a->n = actions[j].n;
				for (int k = 0; k < a->n; ++k) {
					a->trgts[k] = actions[j].trgts[k]->y * SIZE + actions[j].trgts[k]->x;
				}
				goto BREAK_LOOPS;
			}
			if (tmp >= max) {
				max = tmp;
				s_index = i;
				// action
				a->piece_loc = actions[j].piece->y * SIZE + actions[j].piece->x;
				a->n = actions[j].n;
				for (int k = 0; k < a->n; ++k) {
					a->trgts[k] = actions[j].trgts[k]->y * SIZE + actions[j].trgts[k]->x;
				}
			}
			copy_board(&cpy2, &cpy);
		}
	}
BREAK_LOOPS:
	s->from = swaps[s_index].from;
	s->to = swaps[s_index].to;
}
