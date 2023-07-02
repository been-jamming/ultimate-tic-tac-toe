#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "uttt.h"
#include "mcts.h"

static mcts_node *root;

mcts_node *create_node(mcts_node *parent, uttt_board *board, int index){
	mcts_node *output;
	int i;

	output = malloc(sizeof(mcts_node));
	output->parent = parent;
	output->board = board;
	output->parent_index = index;
	output->num_wins = 0;
	output->total = 0;
	for(i = 0; i < 9; i++){
		output->children[i] = NULL;
	}
}

//Implement this next
void mc_random_play(uttt_board *board, enum player_square next_player, int *num_x_wins, int *num_o_wins);

void mc_search_recursive(mcts_node *parent, enum player_square player, double exploration, int *num_plays, int *num_x_wins, int *num_o_wins){
	double scores[9];
	double max_score = -1;
	enum player_square next_player;
	uttt_board *next_board;
	int i;
	int choice;

	if(parent->board->depth == 1 && player == X_PLAYER){
		next_player = O_PLAYER;
	} else if(parent->board->depth == 1 && player == O_PLAYER){
		next_player = X_PLAYER;
	} else {
		next_player = player;
	}
	if(!parent->leaf){
		for(i = 0; i < 9; i++){
			if(parent->children[i] && parent->children[i]->total > 0){
				scores[i] = ((double) parent->children[i]->num_wins)/((double) parent->children[i]->total);
				scores[i] += exploration*sqrt(log(parent->total)/parent->children[i]->total);
			} else if(parent->children[i]){
				scores[i] = INFINITY;
			} else {
				scores[i] = -2;
			}
			if(scores[i] > max_score){
				max_score = scores[i];
			}
		}
		for(i = 0; i < 9; i++){
			if(scores[i] == max_score){
				if(parent->board->depth == 1){
					make_move(parent->board, i, player);
				}
				//Add win check here
				mc_search_recursive(parent->children[i], next_player, exploration, num_plays, num_x_wins, num_o_wins);
				parent->total += *num_plays;
				if(player == X_PLAYER){
					parent->num_wins += *num_x_wins;
				} else if(player == O_PLAYER){
					parent->num_wins += *num_o_wins;
				}
				if(parent->board->depth == 1){
					undo_move(parent->board->children[i]);
				}
				break;
			}
		}
	} else {
		//Expand the tree
		for(i = 0; i < 9; i++){
			if(parent->board->children[i]->square == EMPTY_PLAYER){
				if(parent->board->depth == 1){
					next_board = get_move_selection(parent->board->children[i]);
				} else {
					next_board = parent->board->children[i];
				}
				parent->children[i] = create_node(parent, next_board, i);
			}
		}
		//Select a random new node
		choice = rand()%(9 - parent->board->filled_squares);
		for(i = 0; i < 9; i++){
			if(parent->children[i] != NULL){
				if(choice == 0){
					if(parent->board->depth == 1){
						make_move(parent->board, i, player);
					}
					mc_random_play(parent->children[i]->board, next_player, num_x_wins, num_o_wins);
					parent->children[i]->total += 1;
					if(next_player == X_PLAYER){
						parent->children[i]->num_wins += *num_x_wins;
					} else if(next_player == O_PLAYER){
						parent->children[i]->num_wins += *num_o_wins;
					}
					parent->total += 1;
					if(player == X_PLAYER){
						parent->num_wins += *num_x_wins;
					} else if(player == O_PLAYER){
						parent->num_wins += *num_o_wins;
					}
					if(parent->board->depth == 1){
						undo_move(parent->board->children[i]);
					}
					break;
				} else {
					choice--;
				}
			}
		}
	}
}

void mc_search(mcts_node *root, int max_nodes){

}
