#include "../opencv_matlab_interop.h"

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
    
  //broken in Windows for some reason
  CvRTrees *forest = new CvRTrees;

  forest->clear();
  delete forest;

  mexPrintf("forest deleted successfully\n");
}
