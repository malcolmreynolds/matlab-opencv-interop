#include "opencv_matlab_interop.h"

/*
  Functions which help using opencv from matlab.
*/

//Given a matlab format array, allocates a CvMat of the same size and
//copies the data in. Double only at this point. Converts to float.
CvMat* matlab_matrix_to_opencv_matrix(const mxArray *input) {
    ASSERT_2D_ARRAY(input);
    ASSERT_IS_DOUBLE(input);
    size_t numSamples = mxGetM(input);
    size_t numVariables = mxGetN(input);
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

    size_t numSamples = mxGetM(input);
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

void printCommonTypes() {
    mexPrintf("CV_32F = %x\n", CV_32F);
    mexPrintf("CV_32S = %x\n", CV_32S);
    mexPrintf("CV_16S = %x\n", CV_16S);
    mexPrintf("CV_16U = %x\n", CV_16U);
    mexPrintf("CV_8S = %x\n", CV_8S);
    mexPrintf("CV_8U = %x\n", CV_8U);
    //    mexPrintf("CV_32U = %x", CV_32UC1);
    return;
}

mxArray* opencv_matrix_to_matlab_matrix(const CvMat* input) {
    ASSERT_IS_CVMAT(input);
    
    
    if (CV_MAT_CN(input->type) != 1) {
        mexErrMsgTxt("opencv matrices with multiple channels are not supported.");
    }        

    //switch on the various types - I don't want to try and write an all encompassing function now
    //this gets us just the lower 8 bits from the type, which can be matched against CV_32U etc.
    // CAUTION!! This does NOT handle channels yet!
    switch(input->type & 0xFF) {
    case CV_32F: return opencv_cv32f_matrix_to_matlab(input);
    case CV_8U: return opencv_cv8u_matrix_to_matlab(input);
    default:
        char msgbuf[ERR_MSG_SIZE];
        sprintf(msgbuf, "%s:%d input type not supported: 0x%x\n", 
                __FILE__,__LINE__, input->type & 0xFF);
        mexErrMsgTxt(msgbuf);
        return NULL;
    }
}

//convert an opencv matrix of type CV_32F to matlab format
mxArray* opencv_cv32f_matrix_to_matlab(const CvMat* input) {    

    int rows = input->rows;
    int cols = input->cols;
    int step = input->step;
    
    //this is (apparently?) the best way to work out the element type
    if (CV_ELEM_SIZE(input->type) != 4) { //we expect a double
        mexErrMsgTxt("this doesn't seem to be a CV_32F, error!.\n");
    }
    
    /*
    mexPrintf("bytesPerElem = %d\n", bytesPerElem);
    mexPrintf("type = %x\n", input->type);
    mexPrintf("CV_ELEM_SIZE = %x\n", CV_ELEM_SIZE(input->type));
    mexPrintf("CV_MAT_CN = %x\n", CV_MAT_CN(input->type));
    printCommonTypes();
    */

    mxArray* output = mxCreateDoubleMatrix(rows, cols, mxREAL);
    double* outputPtr = (double *)mxGetPr(output);

    //i and j are indexing into the rows and columns of the OUTPUT!!!!
    for (unsigned int i=0; i < cols; i++) {
        float* thisColInput = input->data.fl + i;
        for (unsigned int j=0; j < rows; j++) {
            *outputPtr = (double)thisColInput[j*step/4];
            outputPtr++;
        }
    }

    return output;
}
        
mxArray* opencv_cv8u_matrix_to_matlab(const CvMat* input) {
    int rows = input->rows;
    int cols = input->cols;
    int step = input->step;

    if (CV_ELEM_SIZE(input->type) != 1) {
        mexErrMsgTxt("this doesn't seem to be a CV_8U, error!\n");
    }

    mxArray* output = mxCreateNumericMatrix(rows, cols, mxUINT8_CLASS, mxREAL);
    unsigned char* outputPtr = (unsigned char*)mxGetPr(output);

    for (unsigned int i=0; i < cols; i++) {
        unsigned char* thisColInput = input->data.ptr + i;
        for (unsigned int j=0; j < rows; j++) {
            *outputPtr = thisColInput[j*step];
            outputPtr++;
        }
    }

    return output;
}
    

void print_opencv_matrix(const CvMat* input) {
    float *dataPtr = input->data.fl;
    for (unsigned int r = 0; r < input->rows; r++) {
        for (unsigned int c = 0; c < input->cols; c++) {
            mexPrintf("%f ",*dataPtr);
            dataPtr++;
        }
        mexPrintf("\n");
    }
}
