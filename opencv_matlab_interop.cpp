#include "opencv_matlab_interop.h"

/*
  Functions which help using opencv from matlab.
*/

//Given a matlab format array, allocates a CvMat of the same size and
//copies the data in. Double only at this point. Converts to float.
CvMat* matlab_matrix_to_opencv_matrix(const mxArray *input) {
    ASSERT_2D_ARRAY(input);
    ASSERT_IS_DOUBLE(input);
    int numSamples = mxGetM(input);
    int numVariables = mxGetN(input);
    double* inputDataPtr = (double *)mxGetPr(input);

    CvMat* cvMat = cvCreateMat(numSamples, numVariables, DEFAULT_OPENCV_MTX_TYPE);

    for (unsigned int i=0; i<numSamples; i++) {
        //access the i'th row of the CvMat
        float* thisRowOut = cvMat->data.fl + numVariables*i;
        //access the i'th row of the matlab array
        //NOTE!!! matlab is column major and opencv is row major, 
        //so I really do mean "+ i" and not "+ numVariables*i"
        double* thisRowIn = inputDataPtr + i;
        for (unsigned int j=0; j<numVariables; j++) {
            thisRowOut[j] = (float)thisRowIn[j*numSamples];
        }
    }

    return cvMat;
}
            
CvMat* matlab_array_to_opencv_array(const mxArray* input) {
    //check we are actually dealing with a vector
    ASSERT_IS_VECTOR(input);
    ASSERT_IS_DOUBLE(input);

    int numSamples = mxGetM(input);
    double* inputDataPtr = (double *)mxGetPr(input);

    CvMat* cvMat = cvCreateMat(numSamples, 1, DEFAULT_OPENCV_MTX_TYPE);
    float* outPtr = cvMat->data.fl;
    
    for (unsigned int i=0; i<numSamples; i++) {
        *outPtr = (float)(*inputDataPtr);
        outPtr++;
        inputDataPtr++;
    }

    return cvMat;
}

void print_opencv_matrix(const CvMat* input) {
    /*
    if (input->type != DEFAULT_OPENCV_MTX_TYPE) {
        char msgbuf[ERR_MSG_SIZE];
        sprintf(msgbuf,"%s:%d input provided doesn't have the type we expect.\n",
                __FILE__, __LINE__);
        mexErrMsgTxt(msgbuf);
    }
    */

    float *dataPtr = input->data.fl;
    for (unsigned int r = 0; r < input->rows; r++) {
        for (unsigned int c = 0; c < input->cols; c++) {
            mexPrintf("%f ",*dataPtr);
            dataPtr++;
        }
        mexPrintf("\n");
    }
}
