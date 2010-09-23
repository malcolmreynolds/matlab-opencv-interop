#ifndef __OPENCV_MATLAB_INTEROP_H
#define __OPENCV_MATLAB_INTEROP_H

#include "mex.h"
#include "ml.h"
#include "mex_utils.h"

const int DEFAULT_OPENCV_MTX_TYPE = CV_32F;

//Conversion from matlab format to opencv
CvMat* matlab_matrix_to_opencv_matrix(const mxArray* input);
CvMat* matlab_array_to_opencv_array(const mxArray* input);
mxArray* opencv_matrix_to_matlab_matrix(const CvMat* input);
mxArray* opencv_cv32f_matrix_to_matlab(const CvMat* input);
mxArray* opencv_cv8u_matrix_to_matlab(const CvMat* input);
mxArray* opencv_array_to_matlab_array(const CvMat* input);

//helper functions for the above
 


//printing
void print_opencv_matrix(const CvMat* input);


#define ASSERT_IS_CVMAT(input)                                          \
    do {                                                                \
        if(!CV_IS_MAT(input)) {                                         \
            char msgbuf[ERR_MSG_SIZE];                                  \
            sprintf(msgbuf, "%s:%d input is not a CvMat!\n",            \
                    __FILE__,__LINE__);                                 \
            mexErrMsgTxt(msgbuf);                                       \
        }                                                               \
    } while(0)                                                          \
        


#endif
