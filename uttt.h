enum player_square{
	EMPTY_PLAYER,
	NO_PLAYER,
	X_PLAYER,
	O_PLAYER
};

typedef struct uttt_board uttt_board;

struct uttt_board{
	int depth;
	int filled_squares;
	int parent_index;
	uttt_board *children[9];
	enum player_square square;
	uttt_board *parent;
	uttt_board *parent_move;
};

uttt_board *create_board(int depth);
enum player_square get_win(uttt_board *board, int index);
void make_move(uttt_board *board, int index, enum player_square square);
uttt_board *get_move_selection(uttt_board *prev_move);

