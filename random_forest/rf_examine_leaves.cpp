#include "../opencv_matlab_interop.h"

// Structure fields to include for each tree
const int num_fields = 1;
const char *field_names[num_fields] = {
	"leaves"
};

const int num_dims = 1;
const mwSize dims[num_dims] = {
	1
};

// Recursively counts the number of leaf nodes in a subtree (to count the number
// of leaves in the whole thing, call on the root). When called on a leaf, returns 1.
unsigned int num_leaves_in_subtree(const CvDTreeNode* n) {
	if (n == NULL) {
		MEX_ERR_PRINTF("num_leaves_in_subtree called on NULL pointer");
	}
	if (n->left == NULL && n->right == NULL) {
		// We are at a leaf
		return 1;
	}
	else {
		return num_leaves_in_subtree(n->left) + num_leaves_in_subtree(n->right);
	}
}


/* Examines the values at each leaf node in order to see what the distribution of data
  we put in is doing */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
	ASSERT_NUM_RHS_ARGS_EQUALS(1);
	
	const mxArray* forest_ptr = prhs[0];
	ASSERT_IS_POINTER(forest_ptr);
	CvRTrees *forest = (CvRTrees *) unpack_pointer(forest_ptr);
	
	// We are going to return a cell array with one cell per tree, so need this number
	int num_trees = forest->get_tree_count();
	mexPrintf("Loaded forest of %d trees, retrieving leave node values.\n", num_trees);

	mxArray *output_cell_array = mxCreateCellMatrix(1, num_trees);
	if (output_cell_array == NULL) {
		MEX_ERR_PRINTF("Could not allocate memory for output cell array");
	}
	
	
	for (unsigned int t = 0; t < num_trees; t++) {
		mxArray* tree_struct = mxCreateStructArray(num_dims, dims, num_fields, field_names);
		if (tree_struct == NULL) {
			MEX_ERR_PRINTF("couldn't allocate structure for tree %d", t);
		}
		mxSetCell(output_cell_array, t, tree_struct);
	}
#ifdef DEBUG
	mexPrintf("allocated a struct for each tree\n");
#endif

	// Now we need to do a depth first search down the tree, first to find the number of nodes
	// then to store the regression value at each of them
	for (unsigned int t = 0; t < num_trees; t++) {
		const CvDTreeNode* root = forest->get_tree(t)->get_root();
		
		unsigned int num_leaf_nodes = num_leaves_in_subtree(root);
#ifdef DEBUG
		mexPrintf("tree %d has %d leaves\n", t, num_leaf_nodes);
#endif
		// Fill this array with the 'value' field of each leaf node
		mxArray* leaf_node_values_mx = mxCreateDoubleMatrix(1, num_leaf_nodes, mxREAL);
		if (leaf_node_values_mx == NULL) {
			MEX_ERR_PRINTF("couldn't allocate leav_nodes_values_mx");
		}
		double* leaf_node_values = (double *)mxGetPr(leaf_node_values_mx);
		
		// Create a stack for nodes to do DFS - needs to be num_leaf_nodes at most although
		// this is a massive overestimate in 99% of cases. Whatever.
		const CvDTreeNode **next_nodes_to_visit = (const CvDTreeNode **)malloc(num_leaf_nodes * sizeof(CvDTreeNode *));
		if (next_nodes_to_visit == NULL) {
			MEX_ERR_PRINTF("couldn't allocate stack for DFS of tree %d", t);
		}
		unsigned int sp = 0; //points to next free_slot in array
		unsigned int leaf_idx = 0; //used to index into leaf_node_values
		
		//put the first node into the stack
		next_nodes_to_visit[sp] = root;
		sp++;
		
		// Depth first search happens in this node
		while( sp != 0 ) { // when sp is zero we are done
			// get a node off the stack
			sp--;
			const CvDTreeNode* current_node = next_nodes_to_visit[sp];
			
			if (current_node == NULL) {
				MEX_ERR_PRINTF("error: current_node is NULL");
			}
			
			if (current_node->left == NULL && current_node->right == NULL) {
				// if it's a leaf node then add the value to the output array
				leaf_node_values[leaf_idx] = current_node->value;
				mexPrintf("found value %f for tree %d leaf id %d\n", 
						  current_node->value, t, leaf_idx);
				leaf_idx++;
			}
			else {
				// if an internal node, push both children onto stack
				next_nodes_to_visit[sp] = current_node->left;
				next_nodes_to_visit[sp+1] = current_node->right;
				sp += 2;
			}
		}
		
		mxArray *tree_struct = mxGetCell(output_cell_array, t);
		mxSetField(tree_struct, 0, field_names[0], leaf_node_values_mx);
		
		// Could do better by not allocating this for each tree, but meh
		free(next_nodes_to_visit);
	}
	
	plhs[0] = output_cell_array;
}




