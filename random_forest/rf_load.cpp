#include "../opencv_matlab_interop.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_EQUALS(1); //just a filename

    //extract the filename we are loading from
    const mxArray *filenamePtr = prhs[0];
    ASSERT_IS_STRING(filenamePtr);
    mwSize numCharacters = num_elements(filenamePtr);
    char *filenameBuffer = (char *)malloc(numCharacters + 1);
    mxGetString(filenamePtr, filenameBuffer, numCharacters+1);
    
    mexPrintf("Trying to load random forest from %s... ",filenameBuffer);
    
    CvRTrees *forest = new CvRTrees();
    forest->load(filenameBuffer);

    if (forest->get_tree_count() == 0) {
        char msgbuf[ERR_MSG_SIZE];
        sprintf(msgbuf,"%s:%d could not load random forest from %s",
                __FILE__,__LINE__,filenameBuffer);
        free(filenameBuffer);
        delete forest;
        mexErrMsgTxt(msgbuf);
    }

    mexPrintf("success!\n");
    free(filenameBuffer);
    plhs[0] = pack_pointer((void *)forest);
}
