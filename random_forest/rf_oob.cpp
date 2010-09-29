#include "../opencv_matlab_interop.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_EQUALS(1); //pointer to the forest

    const mxArray* forestPtr = prhs[0];
    ASSERT_IS_POINTER(forestPtr);
    CvRTrees* forest = (CvRTrees *) unpack_pointer(forestPtr);

    double oob_error = forest->get_oob_error();

    plhs[0] = mxCreateDoubleScalar(oob_error);
}
