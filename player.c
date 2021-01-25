#include "player.h"

static int valid_loc(char *cord)
{
#ifdef DEBUG
	assert(cord);
#endif
	return strlen(cord) == 2 && cord[0] >= 'a' && cord[0] <= 'd' && cord[1] >= '1' && cord[1] <= '4';
}

static int get_swap_input(char cords[5][3])
{
	char *line = readline("Enter swap: ");	
	if (!line)
		return 0;

	char *token;
	int count = 0;
	token = strtok(line, " \n");
	while (1) {
		if (count > 2 || !token || !valid_loc(token))
			break;
		strcpy(cords[count], token);
		++count;
		token = strtok(NULL, " \n");
	}

	free(line);
	return count == 2;
}

static void char_to_pos(int pos[10], char loc[5][3], int n)
{
	for (int i = 0; i < n; ++i) {
		pos[2 * i] = loc[i][0] - 'a';
		pos[2 * i + 1] = loc[i][1] - '1';
	}
}

static int get_action_input(char cords[5][3])
{
	char *line = readline("Enter action: ");
	if (!line)
		return 0;
	char *token;
	int count = 0;
	token = strtok(line, " \n");
	while (1) {
		if (count > 5 || !token || !valid_loc(token))
			break;
		strcpy(cords[count], token);
		++count;
		token = strtok(NULL, " \n");
	}

	free(line);
	return count > 5 ? 0 : count;
}

void human_turn(struct board *b, struct swap *s, struct action_loc *a, int flg)
{
#ifdef DEBUG
	assert(b);
	assert(s);
	assert(a);
#endif
	int swap_pos[10];
	char loc[5][3];

	if (flg == 0) {
		// pick swap
		while (1) {
			if (!get_swap_input(loc))
				continue;
			char_to_pos(swap_pos, loc, 2);
			break;
		}

		s->from = XY1D(swap_pos[0], swap_pos[1]);
		s->to = XY1D(swap_pos[2], swap_pos[3]);
	} else {
		// pick action
		int n_loc;
		while (1) {
			n_loc = get_action_input(loc);
			if (n_loc < 2 || n_loc > 5)
				continue;
			char_to_pos(swap_pos, loc, n_loc);

			a->piece_loc = swap_pos[1] * SIZE + swap_pos[0];
			a->n = n_loc - 1;

			for (int i = 1; i < n_loc; ++i) {
				a->trgts[i - 1] = swap_pos[2 * i + 1] * SIZE + swap_pos[2 * i];
			}
			break;
		}

	}
}

void init_player(struct player *p, enum player_t type)
{
#ifdef DEBUG
	assert(p);
#endif
	p->type = type;
	if (type == HUMAN) {
		p->get_move = human_turn;
	} else {
		p->get_move = bot_turn;
	}
}
