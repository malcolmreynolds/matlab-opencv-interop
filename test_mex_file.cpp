#include "mex.h"
#include "opencv_matlab_interop.h"


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_EQUALS(1);

    CvMat* test = matlab_matrix_to_opencv_matrix(prhs[0]);
    plhs[0] = opencv_matrix_to_matlab_matrix(test);
    cvReleaseMat(&test);
}
