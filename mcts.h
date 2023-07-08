typedef struct mcts_node mcts_node;

struct mcts_node{
	mcts_node *children[9];
	int wins;
	int plays;
};

mcts_node *create_node(void);
void free_tree(mcts_node *node);
enum player_square opposite_player(enum player_square player);
int possible_move(uttt_board *board, int index);
void mc_random_play(uttt_board *root, uttt_board *board, enum player_square player, int *num_x_wins, int *num_o_wins);
void adjust_node(mcts_node *node, enum player_square player, int plays, int num_x_wins, int num_o_wins);
void mc_search_recursive(uttt_board *root, uttt_board *board, mcts_node *node, enum player_square player, double exploration, int plays, int *num_x_wins, int *num_o_wins);

