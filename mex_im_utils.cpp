#include "mex.h"
#include "mex_im_utils.h"

/**
 Get the number of elements in an mxArray
 */
unsigned int num_elements(const mxArray* array) {
  mwSize numDims = mxGetNumberOfDimensions(array);
  const mwSize* dimsArray = mxGetDimensions(array);
  unsigned int num = 1;
  unsigned int i;

  for (i=0; i<numDims; i++) {
    num *= dimsArray[i];
  }

  return num;
}

unsigned int image_size_matches_mask(const mxArray* im, const mxArray* mask) {
  const mwSize* imDims = mxGetDimensions(im);
  const mwSize* maskDims = mxGetDimensions(mask);

  //mexPrintf("im is %dx%d\n",imDims[0],imDims[1]);
  //mexPrintf("mask is %dx%d\n",maskDims[0],maskDims[1]);

  if (imDims[0] != maskDims[0]) return 0;
  if (imDims[1] != maskDims[1]) return 0;
  
  //if both match..
  return 1;
}

// Normalise an array by dividing by element by the sum of src. The
// argument in src is not modified, instead dest is copied to. dest must
// be initialised already. Note they can be the same place if wanted!
void normalise_array(const mxArray* src, mxArray* dest) {
  // I don't do error checking here because this isn't directly callable from MATLAB.
  // Callers must ensure both arguments are instantiated, have correct size and type etc.
  unsigned int i, numPixels;
  double *start, *ptr, *destPtr;
  double total = 0.0;
  
  // find total
  start = ptr = mxGetPr(src);
  numPixels = num_elements(src);
  for (i=0; i<numPixels; i++) {
    total += *ptr++;
  }

  // divide into new array
  ptr = start;
  destPtr = mxGetPr(dest);
  for (i=0; i<numPixels; i++) {
    *destPtr++ = *ptr++ / total;
  }
}
 

//Normalise an array in place by dividing each element by a certain value
void normalise_array_inplace(mxArray* array, double val) {
  ASSERT_IS_DOUBLE(array);
  unsigned int i;
  unsigned int num_pixels = num_elements(array);

  double* p = mxGetPr(array);

  for (i=0; i < num_pixels; i++) {
    *p++ /= val;
  }
}

unsigned char is_vector(const mxArray* input) {
    switch (mxGetNumberOfDimensions(input)) {
    case 1:
        return true;
    case 2: {
        const int *dims = mxGetDimensions(input);
        if ((dims[0] != 1) && (dims[1] != 1)) {
            return false;
        }
        else {
            return true;
        }
    }
    default:
        return false;
    }
}
        

   
