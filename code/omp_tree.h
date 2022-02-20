
////////////////////////// STRUCT ///////////////////////////
typedef struct S {
data value;
int AxSplit;
int depth;
int dim_sub_left;
int dim_sub_right;
struct S* left;
struct S* right;
} tree_node;

typedef struct L {
  tree_node *elem;
  struct L* next;
} stack_node;

/////////////////// FUNCTIONS ////////////////////////////////
void push(stack_node** stack, tree_node *point);
tree_node *pop(stack_node** stack);
node* build_omp_tree(data* set, int dim, int ax, int depth);
void tree_to_array(tree_node* root, node* vtree, int dim);
tree_node* build_tree(data* set, int left,int right,int ax, int depth);
