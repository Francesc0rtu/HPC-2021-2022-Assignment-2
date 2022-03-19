
////////////////////////// STRUCT ///////////////////////////
typedef struct S {
data value;         // point
int AxSplit;         // axis of splitting
int depth;           // depth of the node
int dim_sub_left;     // dimension of the left sub-tree
int dim_sub_right;    // dimension of the right sub-tree
struct S* left;     // pointer to the left child
struct S* right;    // pointer to the right child
} tree_node;

typedef struct L {     // stack node
  tree_node *elem;
  struct L* next;
} stack_node;

/////////////////// FUNCTIONS ////////////////////////////////
void push(stack_node** stack, tree_node *point);
tree_node *pop(stack_node** stack);
node* build_omp_tree(data* set, int dim, int ax, int depth);
void tree_to_array(tree_node* root, node* vtree, int dim);
tree_node* build_tree(data* set, int left,int right,int ax, int depth);
