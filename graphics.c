#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "uttt.h"
#include "mcts.h"

#define RECURSION 2

extern char *buffer;
uttt_board *board;
uttt_board *selection;
mcts_node *node;

int get_board_size(int depth){
	if(depth == 0){
		return 1;
	} else if(depth == 1){
		return 6;
	} else {
		return 3*get_board_size(depth - 1) + 6;
	}
}

int draw_board(uttt_board *board, uttt_board *selection, char *buffer, size_t line_size, char void_char){
	int board_size;
	int child_size;
	int i;
	int j;

	if(board->depth == 0){
		board_size = 1;
	} else {
		child_size = draw_board(board->children[0], selection, buffer + 1 + line_size, line_size, board == selection ? '1' : void_char);

		//Adds padding
		if(board->depth != 1){
			child_size += 2;
		}
		board_size = 3*child_size + 2;

		buffer++;
		buffer += line_size;
		//

		for(i = 0; i < child_size; i++){
			buffer[child_size + line_size*i] = '|';
			buffer[line_size*child_size + i] = '-';
		}
		buffer[child_size + line_size*child_size] = '+';
		
		draw_board(board->children[1], selection, buffer + child_size + 1, line_size, board == selection ? '2' : void_char);
		for(i = 0; i < child_size; i++){
			buffer[2*child_size + 1 + line_size*i] = '|';
			buffer[child_size + 1 + line_size*child_size + i] = '-';
		}
		buffer[2*child_size + 1 + line_size*child_size] = '+';
		
		draw_board(board->children[2], selection, buffer + 2*child_size + 2, line_size, board == selection ? '3' : void_char);
		for(i = 0; i < child_size; i++){
			buffer[2*child_size + 2 + line_size*child_size + i] = '-';
		}

		draw_board(board->children[3], selection, buffer + line_size*(child_size + 1), line_size, board == selection ? '4' : void_char);
		for(i = 0; i < child_size; i++){
			buffer[line_size*(child_size + 1) + child_size + line_size*i] = '|';
			buffer[line_size*(child_size + 1) + line_size*child_size + i] = '-';
		}
		buffer[line_size*(child_size + 1) + child_size + line_size*child_size] = '+';

		draw_board(board->children[4], selection, buffer + line_size*(child_size + 1) + child_size + 1, line_size, board == selection ? '5' : void_char);
		for(i = 0; i < child_size; i++){
			buffer[line_size*(child_size + 1) + child_size + 1 + child_size + line_size*i] = '|';
			buffer[line_size*(child_size + 1) + child_size + 1 + line_size*child_size + i] = '-';
		}
		buffer[line_size*(child_size + 1) + child_size + 1 + child_size + line_size*child_size] = '+';

		draw_board(board->children[5], selection, buffer + line_size*(child_size + 1) + 2*child_size + 2, line_size, board == selection ? '6' : void_char);
		for(i = 0; i < child_size; i++){
			buffer[line_size*(child_size + 1) + 2*child_size + 2 + line_size*child_size + i] = '-';
		}

		draw_board(board->children[6], selection, buffer + line_size*(2*child_size + 2), line_size, board == selection ? '7' : void_char);
		for(i = 0; i < child_size; i++){
			buffer[line_size*(2*child_size + 2) + child_size + line_size*i] = '|';
		}

		draw_board(board->children[7], selection, buffer + line_size*(2*child_size + 2) + child_size + 1, line_size, board == selection ? '8' : void_char);
		for(i = 0; i < child_size; i++){
			buffer[line_size*(2*child_size + 2) + child_size + 1 + child_size + line_size*i] = '|';
		}

		draw_board(board->children[8], selection, buffer + line_size*(2*child_size + 2) + 2*child_size + 2, line_size, board == selection ? '9' : void_char);
	}

	if(board->square != EMPTY_PLAYER || board->depth == 0){
		for(i = 0; i < board_size; i++){
			for(j = 0; j < board_size; j++){
				if(board->square == X_PLAYER && (i == j || i == board_size - j - 1)){
					buffer[line_size*i + j] = 'X';
				} else if(board->square == O_PLAYER && (i == 0 || j == 0 || i == board_size - 1 || j == board_size - 1)){
					buffer[line_size*i + j] = 'O';
				} else if(board->square == X_PLAYER || board->square == O_PLAYER){
					buffer[line_size*i + j] = ' ';
				} else if(board->square == NO_PLAYER){
					buffer[line_size*i + j] = '#';
				} else if(board->square == EMPTY_PLAYER){
					buffer[line_size*i + j] = void_char;
				}
			}
		}
	}

	return board_size;
}

int main(){
	int i;
	int j;
	char input;
	int input_selection;
	int board_size;
	int num_x_wins;
	int num_o_wins;
	int best_move;
	double score;
	double best_score;
	mcts_node *current_node;
	enum player_square current_player;
	uttt_board *board_selection;

	srand(time(NULL));
	board_size = get_board_size(RECURSION);
	printf("%d\n", board_size);
	buffer = malloc(sizeof(char)*(board_size*(board_size + 1) + 1));
	for(i = 0; i < board_size; i++){
		for(j = 0; j < board_size; j++){
			buffer[(board_size + 1)*i + j] = ' ';
		}
		buffer[(board_size + 1)*(i + 1) - 1] = '\n';
	}
	buffer[board_size*(board_size + 1)] = '\0';
	board = create_board(RECURSION);
	board_selection = board;
	current_player = X_PLAYER;

	//Main game loop
	while(board_selection){
		if(current_player == X_PLAYER || current_player == O_PLAYER){
			node = create_node();
			for(i = 0; i < 1000000; i++){
				mc_search_recursive(board, board_selection, node, current_player, 1.41, 1, &num_x_wins, &num_o_wins);
			}
			
			current_node = node;
			best_score = 0.0;
			best_move = 0;
			while(board_selection->depth > 0){
				for(i = 0; i < 9; i++){
					if(current_node->children[i] && current_node->children[i]->plays > 0 && board_selection->depth > 1){
						score = ((double) current_node->children[i]->wins)/current_node->children[i]->plays;
					} else if(current_node->children[i] && current_node->children[i]->plays > 0 && board_selection->depth == 1){
						score = 1.0 - ((double) current_node->children[i]->wins)/current_node->children[i]->plays;
					} else {
						score = 0.0;
					}
					if(score > best_score){
						best_score = score;
						best_move = i;
					}
				}
				board_selection = board_selection->children[best_move];
				current_node = current_node->children[best_move];
			}
			make_move(board_selection->parent, best_move, current_player);
			free_tree(node);

			board_selection = get_move_selection(board_selection);

			current_player = opposite_player(current_player);
		}
		printf("\e[1;1H\e[2J");
		draw_board(board, board_selection, buffer, board_size + 1, ' ');
		if(current_player == X_PLAYER){
			printf("%sX Select Move\n", buffer);
		} else if(current_player == O_PLAYER){
			printf("%sO Select Move\n", buffer);
		}

		/*
		input = getchar();
		if(input >= '1' && input <= '9'){
			input_selection = input - '1';
			if(board_selection->children[input_selection]->square != EMPTY_PLAYER){
				continue;
			}
			board_selection = board_selection->children[input_selection];
			if(board_selection->depth == 0){
				make_move(board_selection->parent, input_selection, current_player);
				board_selection = get_move_selection(board_selection);
				if(!board_selection){
					break;
				}
				current_player = opposite_player(current_player);
			}
		}
		*/
	}
	printf("\e[1;1H\e[2J");
	draw_board(board, board_selection, buffer, board_size + 1, ' ');
	printf("%s", buffer);

	free(buffer);

	return 0;
}

