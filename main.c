#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "board.h"
#include "player.h"

int is_gameover(struct board *b);

int main(void)
{
	struct board b;
	init_board(&b, LEFT, LEFT);

	struct swap swp;
	struct action_loc in_actn;
	struct action actn;

	struct piece *p1, *p2;
	enum p_team won = NONE;

	struct player players[2];
	init_player(&players[WHITE], HUMAN);
	init_player(&players[BLACK], BOT);

	// game loop
	while (1) {
		update_board(&b);
		print_board(&b);
		if (is_gameover(&b))
			break;

		// do swap 
		while (1) {
			players[b.turn].get_move(&b, &swp, &in_actn, 0);
			printf("%d -> %d\n", swp.from, swp.to);

			p1 = piece_at(swp.from % SIZE, swp.from / SIZE, b.squares);
			p2 = piece_at(swp.to % SIZE, swp.to / SIZE, b.squares);

			if ((p1->team == b.turn && can_move(p1, p2->x, p2->y, b.squares))
					|| p2->team == b.turn && can_move(p2, p1->x, p1->y, b.squares)) {
				swap_pieces(p1, p2, b.squares);
				break;
			}
			if (players[b.turn].type == BOT)
				assert(0);
		}

		update_board(&b);
		print_board(&b);
		if (is_gameover(&b))
			break;

		// do action
		while (1) {
			players[b.turn].get_move(&b, &swp, &in_actn, 1);
			printf("%d: ", in_actn.piece_loc);
			// convert in_actn to actn
			actn.piece = b.squares[in_actn.piece_loc];
			actn.n = in_actn.n;
			for (int i = 0; i < in_actn.n; ++i) {
				actn.trgts[i] = b.squares[in_actn.trgts[i]];
				printf("%d, ", in_actn.trgts[i]);
			}
			printf("\n");

			if (actn.piece->team == b.turn && can_action(actn.piece, actn.trgts, actn.n, b.squares)) {
				use_action(actn.piece, actn.trgts, actn.n, b.squares);
				break;
			}
			if (players[b.turn].type == BOT)
				assert(0);
		}

		toggle_turn(&b);
	}

	return 0;
}

int is_gameover(struct board *b)
{
	enum p_team won = winner(b);
	if (won != NONE) {
		if (won == DRAW)
			printf("IT'S A DRAW\n");
		else if (won == BLACK)
			printf("BLACK WINS\n");
		else
			printf("WHITE WINS\n");
	}
	return won != NONE;
}
