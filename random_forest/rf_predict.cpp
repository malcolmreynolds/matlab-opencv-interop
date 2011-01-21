#include "../opencv_matlab_interop.h"

#define PRINT_INPUTS

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_EQUALS(2);
    
    //retrieve the pointer to the random forest
    const mxArray *forestPtr = prhs[0];
    ASSERT_IS_POINTER(forestPtr);
    CvRTrees *forest = (CvRTrees *)unpack_pointer(forestPtr);

    //get the data which we need to predict on into opencv format
    CvMat* dataMtx = matlab_matrix_to_opencv_matrix(prhs[1]);
    CvMat sample; //this is used to point to each row, one at a time

    int numSamples = dataMtx->rows;
    mexPrintf("predicting on %d samples\n", numSamples);

    mxArray* output = mxCreateDoubleMatrix(numSamples, 1, mxREAL);
    double *outputData = (double *)mxGetPr(output);

    if (nlhs > 1) { //are we doing variances?
        mexPrintf("calculating variances\n");
        mxArray* variances = mxCreateDoubleMatrix(numSamples, 1, mxREAL);
        double *varianceData = (double *)mxGetPr(variances);
        float varianceTemp = 5.0; //opencv returns the variance as a float, so need this temp variable.
        
        for (unsigned int i=0; i<numSamples; i++) {
            cvGetRow(dataMtx, &sample, i);
            outputData[i] = (double)forest->predict_variance(&sample, NULL, &varianceTemp);
            varianceData[i] = varianceTemp;
        }
        //variances are second argument
        plhs[1] = variances;
    }
    else {
        for (unsigned int i=0; i<numSamples; i++) {
            cvGetRow(dataMtx, &sample, i);
            outputData[i] = (double)forest->predict(&sample, NULL);
        }
    }
    
    plhs[0] = output;
    cvReleaseMat(&dataMtx);
}
