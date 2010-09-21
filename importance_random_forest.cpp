#include "mex.h"
#include "opencv_matlab_interop.h"


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_EQUALS(1); //just a pointer to the forest

    const mxArray* forestPtr = prhs[0];
    ASSERT_IS_POINTER(forestPtr);
    CvRTrees* forest = (CvRTrees *)unpack_pointer(forestPtr);

    const CvMat* importanceVec = forest->get_var_importance();

    if (!importanceVec) {
        mexErrMsgIdAndTxt("importance_random_forest:importance_not_calculated",
                          "No importance vector present - this forest must have been trained without the importance setting configured");
    }

    //convert to matlab format
    plhs[0] = opencv_matrix_to_matlab_matrix(importanceVec);

}
