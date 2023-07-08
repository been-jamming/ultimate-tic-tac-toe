#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "uttt.h"
#include "graphics.h"
#include "mcts.h"

int board_size;
char *buffer;
char message[256];

mcts_node *create_node(void){
	mcts_node *output;
	int i;

	output = malloc(sizeof(mcts_node));
	output->wins = 0;
	output->plays = 0;
	for(i = 0; i < 9; i++){
		output->children[i] = NULL;
	}

	return output;
}

void free_tree(mcts_node *node){
	int i;

	for(i = 0; i < 9; i++){
		if(node->children[i] != NULL){
			free_tree(node->children[i]);
		}
	}

	free(node);
}

enum player_square opposite_player(enum player_square player){
	if(player == X_PLAYER){
		return O_PLAYER;
	} else {
		return X_PLAYER;
	}
}

int possible_move(uttt_board *board, int index){
	return board->children[index]->square == EMPTY_PLAYER;
}

void mc_random_play(uttt_board *root, uttt_board *board, enum player_square player, int *num_x_wins, int *num_o_wins){
	int i;
	int possible[9];
	int num_possible = 0;
	int r;
	int choice;
	struct timespec t;
	uttt_board *next_board;
	enum player_square next_player;

	/*
	printf("\e[1;1H\e[2J");
	draw_board(root, NULL, buffer, board_size + 1, ' ');
	printf("%s\n%s\n", buffer, message);
	if(player == X_PLAYER){
		printf("player: X\n");
	} else {
		printf("player: O\n");
	}
	t = (struct timespec) {.tv_sec = 0, .tv_nsec = 2000000};
	nanosleep(&t, NULL);
	*/
	//getchar();

	if(root->square != EMPTY_PLAYER){
		*num_x_wins = (root->square == X_PLAYER);
		*num_o_wins = (root->square == O_PLAYER);
		return;
	}

	num_possible = 0;

	for(i = 0; i < 9; i++){
		if(possible_move(board, i)){
			num_possible++;
			possible[i] = 1;
		} else {
			possible[i] = 0;
		}
	}

	if(num_possible){
		r = rand()%num_possible;

		for(choice = 0; choice < 9; choice++){
			if(r == 0 && possible[choice]){
				break;
			}
			if(possible[choice]){
				r--;
			}
		}

		if(board->depth == 1){
			make_move(board, choice, player);
			next_board = get_move_selection(board->children[choice]);
			next_player = opposite_player(player);
		} else {
			next_board = board->children[choice];
			next_player = player;
		}
		
		mc_random_play(root, next_board, next_player, num_x_wins, num_o_wins);
		if(board->depth == 1){
			undo_move(board->children[choice]);
		}
	} else {
		//Drawn board
		*num_x_wins = 0;
		*num_o_wins = 0;
	}
}

void adjust_node(mcts_node *node, enum player_square player, int plays, int num_x_wins, int num_o_wins){
	node->plays += 2*plays;
	node->wins += 2*num_x_wins*(player == X_PLAYER) + 2*num_o_wins*(player == O_PLAYER) + plays - num_x_wins - num_o_wins;
}

void mc_search_recursive(uttt_board *root, uttt_board *board, mcts_node *node, enum player_square player, double exploration, int plays, int *num_x_wins, int *num_o_wins){
	double scores[9];
	double max_score = -1;
	enum player_square next_player;
	uttt_board *next_board;
	mcts_node *next_node = NULL;
	int leaf = 0;
	int i;
	int choice;
	struct timespec t;

	/*
	printf("\e[1;1H\e[2J");
	draw_board(root, board, buffer, board_size + 1, ' ');
	printf("%s\n%s\n", buffer, message);
	if(player == X_PLAYER){
		printf("player: X\n");
	} else {
		printf("player: O\n");
	}
	t = (struct timespec) {.tv_sec = 0, .tv_nsec = 200000000};
	nanosleep(&t, NULL);
	getchar();
	*/

	if(root->square != EMPTY_PLAYER){
		*num_x_wins = (root->square == X_PLAYER);
		*num_o_wins = (root->square == O_PLAYER);
		adjust_node(node, player, plays, *num_x_wins, *num_o_wins);
		return;
	}

	for(i = 0; i < 9; i++){
		if(possible_move(board, i)){
			if(node->children[i] == NULL){
				scores[i] = INFINITY;
			} else {
				//Check if the score should be inverted

				//The node should not be inverted
				if(board->depth > 1){
					scores[i] = ((double) node->children[i]->wins)/((double) node->children[i]->plays);
					scores[i] += exploration*sqrt(log(node->plays)/node->children[i]->plays);
				//The node should be inverted
				} else {
					scores[i] = 1.0 - ((double) node->children[i]->wins)/((double) node->children[i]->plays);
					scores[i] += exploration*sqrt(log(node->plays)/node->children[i]->plays);
				}
			}
		} else {
			scores[i] = -2;
		}
		if(scores[i] > max_score){
			max_score = scores[i];
		}
	}

	for(i = 0; i < 9; i++){
		if(max_score == scores[i]){
			next_board = board->children[i];
			if(node->children[i] == NULL){
				node->children[i] = create_node();
				leaf = 1;
			}
			next_node = node->children[i];
			choice = i;
			break;
		}
	}

	if(board->depth == 1){
		make_move(board, choice, player);
		next_board = get_move_selection(next_board);
		next_player = opposite_player(player);
	} else {
		next_player = player;
	}

	if(leaf){
		mc_random_play(root, next_board, next_player, num_x_wins, num_o_wins);
		adjust_node(next_node, next_player, plays, *num_x_wins, *num_o_wins);
		adjust_node(node, player, plays, *num_x_wins, *num_o_wins);
		if(board->depth == 1){
			undo_move(board->children[choice]);
		}
		return;
	} else if(next_node){
		mc_search_recursive(root, next_board, next_node, next_player, exploration, plays, num_x_wins, num_o_wins);
		adjust_node(node, player, plays, *num_x_wins, *num_o_wins);
		if(board->depth == 1){
			undo_move(board->children[choice]);
		}
		return;
	}
}

