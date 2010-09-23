#include "mex.h"
#include "opencv_matlab_interop.h"

//add more as I think of them...
const char *fieldNames[] = {
    "importance",
    "active_var_mask",
    "num_trees",
    "type"
};

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_EQUALS(1); //just a pointer to the forest

    //retrieve the forest
    const mxArray* forestPtr = prhs[0];
    ASSERT_IS_POINTER(forestPtr);
    CvRTrees* forest = (CvRTrees *)unpack_pointer(forestPtr);

    //make a struct to store things in
    mxArray *output = mxCreateStructMatrix(1,1,4, fieldNames);

    //1: importance vector
    const CvMat* importanceVec = forest->get_var_importance();
    if (!importanceVec) {
        mexWarnMsgIdAndTxt("info_random_forest:importance_not_calculated",
                          "No importance vector present - this forest must have been trained without the importance setting configured");
    }
    else {
        mxSetField(output, 0, "importance", opencv_matrix_to_matlab_matrix(importanceVec));
    }

    //something in this stuff is not float - probably logical?
    
    //2: active var mask
    const CvMat* active_vars = forest->get_active_var_mask();
    if (!active_vars) {
        mexWarnMsgIdAndTxt("info_random_forest:no_active_vars_mask",
                           "no active vars mask object is present in this random forest");
    }
    else {
        mxSetField(output, 0, "active_var_mask", opencv_matrix_to_matlab_matrix(active_vars));
    }
    

    plhs[0] = output;
}
