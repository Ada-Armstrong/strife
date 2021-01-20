#ifndef FEUD_BOT_H
#define FEUD_BOT_H

#include <limits.h>
#include "generate.h"

#ifdef DEBUG
#include <assert.h>
#endif

void bot_turn(struct board *b, struct swap *s, struct action *a, int);

#endif
