#include "../opencv_matlab_interop.h"

#include "calib3d/calib3d.hpp"

#define DEBUG

/* RHS arguments
  prhs[0]: object points in object coordinate space - 3xN
  prhs[1]: image points 2xN
  prhs[2]: camera matrix [fx 0 cx;0 fy cy; 0 0 1]: 3x3
  prhs[3]: distortion coefficients: 1x5

  plhs[0]: rotation matrix
  plhs[1]: translation vector
*/
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_EQUALS(4);
    ASSERT_NUM_LHS_ARGS_EQUALS(2);

    const mxArray *objectPoints = prhs[0];
    const mxArray *imagePoints = prhs[1];
    const mxArray *cameraMatrix = prhs[2];
    const mxArray *distortionCoeffs = prhs[3];

    ASSERT_IS_DOUBLE(objectPoints);
    ASSERT_IS_DOUBLE(imagePoints);
    ASSERT_IS_DOUBLE(cameraMatrix);
    ASSERT_IS_DOUBLE(distortionCoeffs);

    //check stuff is the right shape, etc
    ASSERT_IS_3_BY_3(cameraMatrix);
    if (mxGetM(objectPoints) != 3) {
        mexErrMsgTxt("object points should have first dimension of size 3");
    }
    else if (mxGetM(imagePoints) != 2) {
        mexErrMsgTxt("image points should have first dimension of size 3");
    }
    else if ((mxGetM(distortionCoeffs) != 1) ||
             (mxGetN(distortionCoeffs) != 5)) {
        mexErrMsgTxt("distortion coeffs should be 1x5");
    }

    //make sure we have the right number of points
    int numPoints = mxGetN(objectPoints);
    if (mxGetN(imagePoints) != numPoints) {
        mexErrMsgTxt("number of points in imagePoints and objectPoints do not match");
    }

#ifdef DEBUG
    mexPrintf("Calculating extrinsics - %d points found\n", numPoints);
#endif

    //convert everything to opencv world
    CvMat* cvObjectPoints = matlab_matrix_to_opencv_matrix(objectPoints);
    CvMat* cvImagePoints = matlab_matrix_to_opencv_matrix(imagePoints);
    CvMat* cvCamMatrix = matlab_matrix_to_opencv_matrix(cameraMatrix);
    CvMat* cvDistCoeffs = matlab_matrix_to_opencv_matrix(distortionCoeffs);

    //these will be filled up by the algorithm
    CvMat* cvRotVec = cvCreateMat(3, 1, CV_32F);
    CvMat* cvRotMtx = cvCreateMat(3, 3, CV_32F);
    CvMat* cvTransVec = cvCreateMat(3, 1, CV_32F);


    cvFindExtrinsicCameraParams2(cvObjectPoints, cvImagePoints, cvCamMatrix, 
                                 cvDistCoeffs, cvRotVec, cvTransVec);

    //need to convert rotation vector into rotation matrix
    cvRodrigues2(cvRotVec, cvRotMtx);

    //put the results out to matlab format
    plhs[0] = opencv_matrix_to_matlab_matrix(cvRotMtx);
    plhs[1] = opencv_matrix_to_matlab_matrix(cvTransVec);
                                 
    //delete the opencv objects
    cvReleaseMat(&cvObjectPoints);
    cvReleaseMat(&cvImagePoints);
    cvReleaseMat(&cvCamMatrix);
    cvReleaseMat(&cvDistCoeffs);
    cvReleaseMat(&cvRotVec);
    cvReleaseMat(&cvRotMtx);
    cvReleaseMat(&cvTransVec);
}
