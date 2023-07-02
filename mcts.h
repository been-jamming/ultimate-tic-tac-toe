typedef struct mcts_node mcts_node;

struct mcts_node{
	mcts_node *parent;
	mcts_node *children[9];
	uttt_board *board;
	int leaf;
	int parent_index;
	int num_wins;
	int total;
};

