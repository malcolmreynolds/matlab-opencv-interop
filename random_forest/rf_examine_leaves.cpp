#include "../opencv_matlab_interop.h"

// Structure fields to include for each tree
const int tree_num_fields = 1;
const char *tree_field_names[tree_num_fields] = {
	"nodes"
};

const char *internal_node_field_names[] = {
	"l",
	"r",
	"value"
};
const int internal_node_ndims = sizeof(internal_node_field_names) / sizeof(const char *);

const char *leaf_node_field_names[] = {
	"value"
};
const int leaf_node_ndims = sizeof(leaf_node_field_names) / sizeof(const char *);

const int num_dims = 1;
const mwSize dims[num_dims] = {
	1
};

// Recursively counts the number of leaf nodes in a subtree (to count the number
// of leaves in the whole thing, call on the root). When called on a leaf, returns 1.
unsigned int num_leaves_in_subtree(const CvDTreeNode* n) {
	ASSERT_NON_NULL(n);

	if (n->left == NULL && n->right == NULL) {
		// We are at a leaf
		return 1;
	}
	else {
		return num_leaves_in_subtree(n->left) + num_leaves_in_subtree(n->right);
	}
}

mxArray* make_matlab_nodes_struct(const CvDTreeNode *node) {
	ASSERT_NON_NULL(node);

	if (node->left == NULL && node->right == NULL) {
		//leaf node
		mxArray* leaf_struct = mxCreateStructArray(num_dims, dims, 
		                                           leaf_node_ndims, leaf_node_field_names);
		mxArray* double_val = mxCreateDoubleScalar(node->value);
		ASSERT_NON_NULL(leaf_struct);
		ASSERT_NON_NULL(double_val);

		mxSetField(leaf_struct, 0, "value", double_val);
		return leaf_struct;
	}
	else {
		// internal node, so need to recurse
		mxArray* node_struct = mxCreateStructArray(num_dims, dims,
			internal_node_ndims, internal_node_field_names);
		ASSERT_NON_NULL(node_struct);
		mxSetField(node_struct, 0, "l", make_matlab_nodes_struct(node->left));
		mxSetField(node_struct, 0, "r", make_matlab_nodes_struct(node->right));
		return node_struct;
	}
}

// This can potentially hold other bits of metadata but for now just has the nodes recursive
// structure.
mxArray* make_matlab_tree_struct(CvForestTree *tree) {
	ASSERT_NON_NULL(tree);
	
	mxArray* tree_struct = mxCreateStructArray(num_dims, dims, tree_num_fields, tree_field_names);
	ASSERT_NON_NULL(node_struct);
	mxSetField(tree_struct, 0, "nodes", make_matlab_nodes_struct(tree->get_root()));
	return tree_struct;
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
	ASSERT_NON_NULL(output_cell_array);
	
	for (unsigned int t = 0; t < num_trees; t++) {
		mxArray* tree_struct = mxCreateStructArray(num_dims, dims, tree_num_fields, tree_field_names);
		ASSERT_NON_NULL(tree_struct);
		mxSetCell(output_cell_array, t, make_matlab_tree_struct(forest->get_tree(t)));
	}
	plhs[0] = output_cell_array;
}




