#include <stdlib.h>
#include <stdio.h>
#include "uttt.h"

static int win_codes[9][5][2] = {
	//top left 0
	{{1, 2}, {3, 6}, {4, 8}, {-1, -1}, {-1, -1}},
	//top middle 1
	{{0, 2}, {4, 7}, {-1, -1}, {-1, -1}, {-1, -1}},
	//top right 2
	{{0, 1}, {5, 8}, {4, 6}, {-1, -1}, {-1, -1}},
	//middle left 3
	{{0, 6}, {4, 5}, {-1, -1}, {-1, -1}, {-1, -1}},
	//middle middle 4
	{{1, 7}, {3, 5}, {0, 8}, {2, 6}, {-1, -1}},
	//middle right 5
	{{2, 8}, {3, 4}, {-1, -1}, {-1, -1}, {-1, -1}},
	//bottom left 6
	{{0, 3}, {7, 8}, {2, 4}, {-1, -1}, {-1, -1}},
	//bottom middle 7
	{{1, 4}, {6, 8}, {-1, -1}, {-1, -1}, {-1, -1}},
	//bottom right 8
	{{2, 5}, {6, 7}, {0, 4}, {-1, -1}, {-1, -1}}
};

static uttt_board *create_board_recursive(int depth, uttt_board *parent, int child_id){
	uttt_board *output;
	int i;

	output = malloc(sizeof(uttt_board));
	output->depth = depth;
	output->filled_squares = 0;
	output->parent_index = child_id;
	output->parent = parent;
	if(depth > 0){
		for(i = 0; i < 9; i++){
			output->children[i] = create_board_recursive(depth - 1, output, i);
		}
		output->square = EMPTY_PLAYER;
	} else {
		output->square = EMPTY_PLAYER;
	}
	output->square = EMPTY_PLAYER;

	return output;
}

static void link_board_recursive(uttt_board *board){
	int i;

	board->parent_move = board->parent->parent_move->children[board->parent_index];
	if(board->depth > 0){
		for(i = 0; i < 9; i++){
			link_board_recursive(board->children[i]);
		}
	}
}

uttt_board *create_board(int depth){
	uttt_board *output;
	uttt_board temp;
	int i;

	output = malloc(sizeof(uttt_board));
	output->depth = depth;
	output->filled_squares = 0;
	output->parent = NULL;
	output->parent_move = &temp;
	for(i = 0; i < 9; i++){
		temp.children[i] = output;
	}
	if(depth > 0){
		for(i = 0; i < 9; i++){
			output->children[i] = create_board_recursive(depth - 1, output, i);
			output->children[i]->parent_move = output;
		}
		for(i = 0; i < 9; i++){
			link_board_recursive(output->children[i]);
		}
		output->square = EMPTY_PLAYER;
	} else {
		output->square = EMPTY_PLAYER;
	}

	output->parent_move = NULL;
	output->square = EMPTY_PLAYER;

	return output;
}

enum player_square get_win(uttt_board *board, int index){
	int i;

	if(board->children[index]->square == X_PLAYER || board->children[index]->square == O_PLAYER){
		for(i = 0; win_codes[index][i][0] != -1; i++){
			if(board->children[win_codes[index][i][0]]->square == board->children[index]->square &&
			   board->children[win_codes[index][i][1]]->square == board->children[index]->square){
				return board->children[index]->square;
			}
		}
	}

	if(board->filled_squares >= 9){
		return NO_PLAYER;
	}

	return EMPTY_PLAYER;
}

void make_move(uttt_board *board, int index, enum player_square square){
	enum player_square player_win;

	board->children[index]->square = square;
	board->filled_squares++;
	player_win = get_win(board, index);
	if(player_win != EMPTY_PLAYER && board->parent){
		make_move(board->parent, board->parent_index, player_win);
	} else if(player_win != EMPTY_PLAYER && !board->parent){
		board->square = player_win;
	}
}

void undo_move(uttt_board *board){
	while(board){
		board->square = EMPTY_PLAYER;
		board = board->parent;
	}
}

uttt_board *get_move_selection(uttt_board *prev_move){
	uttt_board *selection;

	selection = prev_move->parent_move;
	while(selection && selection->square != EMPTY_PLAYER){
		selection = selection->parent;
	}

	return selection;
}

