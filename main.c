#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "uttt.h"
#include "graphics.h"
#include "mcts.h"

extern char *buffer;
extern char message[];
uttt_board *board;
const int recursion = 2;
extern int board_size;
mcts_node *node;

double print_principal(char *message, uttt_board *root, uttt_board *board, enum player_square player, mcts_node *node, int depth){
	double score;
	double best_score = 0.0;
	int best_move = -1;
	int i;

	if(depth == 0){
		draw_board(root, NULL, buffer, board_size + 1, ' ');
		return 0.0;
	}

	for(i = 0; i < 9; i++){
		if(node->children[i] && node->children[i]->plays > 0 && board->depth > 1){
			score = ((double) node->children[i]->wins)/node->children[i]->plays;
		} else if(node->children[i] && node->children[i]->plays > 0 && board->depth == 1){
			score = 1.0 - ((double) node->children[i]->wins)/node->children[i]->plays;
		} else {
			score = 0.0;
		}
		if(score > best_score){
			best_score = score;
			best_move = i;
		}
	}

	if(best_move >= 0){
		sprintf(message, "%d, ", best_move);
		if(board->depth == 1){
			make_move(board, best_move, player);
			print_principal(message + 3, root, get_move_selection(board->children[best_move]), opposite_player(player), node->children[best_move], depth - 1);
			undo_move(board->children[best_move]);
		} else {
			print_principal(message + 3, root, board->children[best_move], player, node->children[best_move], depth - 1);
		}
		return best_score;
	} else {
		draw_board(root, NULL, buffer, board_size + 1, ' ');
		return 0.0;
	}
}

int main(int argc, char **argv){
	int i;
	int j;
	int num_x_wins;
	int num_o_wins;
	mcts_node *child;
	double score;
	double best_score = 0.0;
	int best_move = 0;

	srand(time(NULL));
	board_size = get_board_size(recursion);
	buffer = malloc(sizeof(char)*(board_size*(board_size + 1) + 1));
	board = create_board(recursion);

	node = create_node();

	printf("board size: %d\n", board_size);
	/*
	printf("\e[1;1H\e[2J");
	draw_board(board, NULL, buffer, board_size + 1, ' ');
	printf("%s", buffer);
	getchar();
	*/

	while(1){
		for(i = 0; i < board_size; i++){
			for(j = 0; j < board_size; j++){
				buffer[(board_size + 1)*i + j] = ' ';
			}
			buffer[(board_size + 1)*(i + 1) - 1] = '\n';
		}
		buffer[board_size*(board_size + 1)] = '\0';

		if(node->plays%100000 == 0){
			score = print_principal(message, board, board, X_PLAYER, node, 15);
			printf("%s%s\nscore: %lf (%d)\n", buffer, message, score, node->plays);
		}

		mc_search_recursive(board, board, node, X_PLAYER, 1.41, 1, &num_x_wins, &num_o_wins);
	}
}

