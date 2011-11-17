#include "../opencv_matlab_interop.h"

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

	plhs[0] = mxCreateCellMatrix(1, num_trees);
	if (plhs[0] == NULL) {
		MEX_ERR_PRINTF("Could not allocate memory for output cell array");
	}
	
}
