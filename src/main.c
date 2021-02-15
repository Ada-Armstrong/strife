#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "board.h"
#include "player.h"

int is_gameover(struct board *b);
void choose_start(struct board *b);

char *rank_file[] = {"a1", "b1", "c1", "d1",
		     "a2", "b2", "c2", "d2",
		     "a3", "b3", "c3", "d3",
		     "a4", "b4", "c4", "d4"
};

void choose_color(struct player players[2])
{
	char c;
	int color = -1;

	while (color == -1) {
		printf("Choose your color BLACK (0) or White (1)\n");
		c = getchar() - '0';
		switch (c) {
		case 0:
			color = BLACK;
			break;
		case 1:
			color = WHITE;
			break;
		}
		while(getchar() != '\n');
	}
	init_player(&players[color], HUMAN);
	init_player(&players[1 - color], BOT);
}


int main(void)
{
	struct board b;
	struct player players[2];

	choose_color(players);
	choose_start(&b);

	struct swap swp;
	struct action_loc in_actn;
	struct action actn;

	struct piece *p1, *p2;

	// game loop
	while (1) {
		update_board(&b);
		print_board(&b);
		if (is_gameover(&b))
			break;

		// do swap 
		while (1) {
			players[b.turn].get_move(&b, &swp, &in_actn, 0);

			printf("%s <-> %s\n", rank_file[swp.from], rank_file[swp.to]);

			p1 = piece_at(swp.from % SIZE, swp.from / SIZE, b.squares);
			p2 = piece_at(swp.to % SIZE, swp.to / SIZE, b.squares);

			if (can_swap(&b, p1, p2)) {
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

			if (in_actn.piece_loc == -1 && in_actn.n == 0) {
				// skip
				printf("skip action\n");
				actn.piece = NULL;
				actn.n = in_actn.n;
			} else {
				printf("%s: ", rank_file[in_actn.piece_loc]);
				// convert in_actn to actn
				actn.piece = b.squares[in_actn.piece_loc];
				actn.n = in_actn.n;
				for (int i = 0; i < in_actn.n; ++i) {
					actn.trgts[i] = b.squares[in_actn.trgts[i]];
					printf("%s, ", rank_file[in_actn.trgts[i]]);
				}
				printf("\n");
			}

			if (can_apply_action(&b, actn.piece, actn.trgts, actn.n)) {
				apply_action(&b, actn.piece, actn.trgts, actn.n);
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

void choose_start(struct board *b)
{
	char c;
	enum start_pos black, white;
	while (1) {
		printf("Black choose starting pos (0-3)\n");
		c = getchar() - '0';
		switch (c) {
		case 0:
			black = LEFT;
			goto white_choice;
		case 1:
			black = MID_LEFT;
			goto white_choice;
		case 2:
			black = MID_RIGHT;
			goto white_choice;
		case 3:
			black = RIGHT;
			goto white_choice;
		}
		while(getchar() != '\n');
	}

white_choice:
	while(getchar() != '\n');

	while (1) {
		printf("White choose starting pos (0-3)\n");
		c = getchar() - '0';
		switch (c) {
		case 0:
			white = LEFT;
			goto setup_board;
		case 1:
			white = MID_LEFT;
			goto setup_board;
		case 2:
			white = MID_RIGHT;
			goto setup_board;
		case 3:
			white = RIGHT;
			goto setup_board;
		}
		while(getchar() != '\n');
	}
setup_board:
	init_board(b, white, black);
}
