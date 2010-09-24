#include "mex.h"
#include "opencv_matlab_interop.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_EQUALS(2); //forest and name
    
    //get the forest
    const mxArray *forestPtr = prhs[0];
    ASSERT_IS_POINTER(forestPtr);
    CvRTrees *forest = (CvRTrees *)unpack_pointer(forestPtr);

    //get the filename
    const mxArray *filenamePtr = prhs[1];
    ASSERT_IS_STRING(filenamePtr);
    mwSize numCharacters = num_elements(filenamePtr);
    char *filenameBuffer = (char *)malloc(numCharacters + 1);
    mxGetString(filenamePtr, filenameBuffer, numCharacters + 1);

    mexPrintf("Saving random forest to %s\n",filenameBuffer);

    forest->save(filenameBuffer);

    free(filenameBuffer);
}
