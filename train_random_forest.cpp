#include "mex.h"
#include "opencv_matlab_interop.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_GTE(2);
    ASSERT_NUM_LHS_ARGS_LT(2);

    const mxArray* dataMtx = prhs[0];
    const mxArray* targetValueVec = prhs[1];

    CvMat* dataCvMtx = matlab_matrix_to_opencv_matrix(dataMtx);
    mexPrintf("data matrix converted to opencv. %d rows, %d columns\n",
              dataCvMtx->rows, dataCvMtx->cols);
    print_opencv_matrix(dataCvMtx);


    CvMat* targetCvMtx = matlab_array_to_opencv_array(targetValueVec);
    mexPrintf("target values converted to opencv. %d rows, %d columns\n",
              targetCvMtx->rows, targetCvMtx->cols);
    print_opencv_matrix(targetCvMtx);

    cvReleaseMat(&dataCvMtx);
    cvReleaseMat(&targetCvMtx);
} 
    
