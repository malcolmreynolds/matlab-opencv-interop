#include "mex.h"
#include "mex_utils.h"
#include "opencv_matlab_interop.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_EQUALS(1);

    CvRTrees* forest = (CvRTrees *)unpack_pointer(prhs[0]);
    forest->clear();
    delete forest;
}
