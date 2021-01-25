#ifndef FEUD_PLAYER_H
#define FEUD_PLAYER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "bot.h"

#ifdef DEBUG
#include <assert.h>
#endif

enum player_t {
	HUMAN,
	BOT
};

struct player {
	enum player_t type;
	void (*get_move)(struct board *b, struct swap *s, struct action_loc *a, int flg);
};


void init_player(struct player *p, enum player_t type);

void human_turn(struct board *b, struct swap *s, struct action_loc *a, int flg);

#endif
