#ifndef __MEX_IM_UTILS_H
#define __MEX_IM_UTILS_H

#include "mex.h"

//These functions are found in mex_im_utils.c
unsigned int num_elements(const mxArray* array);
unsigned int image_size_matches_mask(const mxArray* im, const mxArray* mask);
void normalise_array(const mxArray* src, mxArray* dest);
void normalise_array_inplace(mxArray* array, double val);
unsigned char is_vector(const mxArray* input);

//pointer stuff
mxArray *pack_pointer(void *);
void *unpack_pointer(const mxArray *);

//inline utils
inline double max(double d1, double d2) {
    return (d1 > d2) ? d1 : d2;
}

//*****************************
// Macros to check number of arguments etc.

#define ERR_MSG_SIZE 2048

// // Way to write into error buffer. Wish I knew about this before
// #define MEX_ERR_PRINTF(fstring) \
//     do {
// 		char msgbuf[ERR_MSG_SIZE]; \
// 		sprintf(msgbuf, "%s:%d " fstring, __FILE__, __LINE__)
// }

#define MEX_ERR_PRINTF(fstring, ...) \
	do { \
		char msgbuf[ERR_MSG_SIZE]; \
		sprintf(msgbuf, "%s:%d " fstring, __FILE__, __LINE__, ##__VA_ARGS__); \
		mexErrMsgTxt(msgbuf); \
	} while(0)

// the do { ... } while(0) means I can have a semi colon after the
// macro invocations, which makes things look super sweet.

//Assert that we have exactly `num' right hand side arguments.
#define ASSERT_NUM_RHS_ARGS_EQUALS(num)					\
    do {                                                                \
        if (nrhs != (num)) {						\
			MEX_ERR_PRINTF("expected %d rhs arguments, but got %d.", (num), nrhs); \
        } } while(0)


//Assert that we have exactly `num' left hand side arguments
#define ASSERT_NUM_LHS_ARGS_EQUALS(num)					\
    do {                                                                \
        if (nlhs != (num)) {						\
			MEX_ERR_PRINTF("expected %d lhs arguments, but got %d.", (num), nlhs); \
        } } while(0)

//Assert that we have less than `num' left hand side arguments
#define ASSERT_NUM_LHS_ARGS_LT(num)					\
    do {                                                                \
        if (nlhs >= (num)) {						\
            MEX_ERR_PRINTF("expected less than %d lhs arguments, but got %d.",(num),nlhs);                      \
        } } while(0)

//assert we have at least num rhs args
#define ASSERT_NUM_RHS_ARGS_GTE(num)                                    \
    do {                                                                \
        if (nrhs < (num)) {                                             \
			MEX_ERR_PRINTF("expected at least %d rhs args, but got %d.", (num), nrhs); \
        } } while(0)

// Put ASSERT_NUM_RHS_ARGS_LT and others here, once needed.

//***************************
//Macros to check type of args

// Assert the third dimension of an array is of size 3 (as in, an RGB image)
#define ASSERT_HAS_3_CHANNELS(array)					            \
    do {                                                            \
        if (mxGetNumberOfDimensions(array) != 3 ||                  \
            mxGetDimensions(array)[2] != 3) {				        \
				MEX_ERR_PRINTF("argument should have 3 channels."); \
        } } while(0)


#define ASSERT_2D_ARRAY(array)						\
    do {                                                                \
        if (mxGetNumberOfDimensions(array) != 2) {                      \
			MEX_ERR_PRINTF("argument should be 2 dimensional.");        \
        } } while(0) 


#define ASSERT_IS_VECTOR(array)                                 \
    do {                                                        \
        if (!is_vector(array)) {                                \
			MEX_ERR_PRINTF("argument must be a vector.");       \
        } } while(0)

#define ASSERT_IS_UINT8(array)						\
    do {                                                                \
        if (!mxIsUint8(array)) {                                        \
			MEX_ERR_PRINTF("argument must be of type Uint8."); \
        } } while(0)

#define ASSERT_IS_UINT32(array)                                         \
    do {                                                                \
        if (!mxIsUint32(array)) {                                       \
			MEX_ERR_PRINTF("argument must be of type Uint32.");         \
        } } while(0)

#define ASSERT_IS_SINT32(array)                                         \
    do {                                                                \
        if (!mxIsInt32(array)) {                                        \
			MEX_ERR_PRINTF("argument must be of type Sint32.");         \
        } } while(0)

#define ASSERT_IS_SINT64(array)                                         \
    do {                                                                \
        if (!mxIsInt64(array)) {                                        \
			MEX_ERR_PRINTF("argument must be of type Sint64.");         \
        } } while(0)


#define ASSERT_IS_DOUBLE(array)						\
    do {                                                                \
        if (!mxIsDouble(array)) {                                       \
			MEX_ERR_PRINTF("argument must be of type Double.");         \
        } } while(0)

#define ASSERT_IS_SCALAR(array)                                         \
    do {                                                                \
        if (num_elements(array) != 1) {                                 \
			MEX_ERR_PRINTF("argument must be a single number.");        \
        } } while(0)

#define ASSERT_IS_STRUCT(array)                 \
    do {                                        \
        if (!mxIsStruct(array)) {               \
			MEX_ERR_PRINTF("argument must be a struct."); \
        } } while(0)

#define ASSERT_IS_3_BY_3(array)                                         \
    do {                                                                \
        if ((mxGetNumberOfDimensions(array) != 2) ||                    \
            (mxGetM(array) != 3) ||                                     \
            (mxGetN(array) != 3)) {                                     \
				MEX_ERR_PRINTF("argument should be a 3x3 matrix."); \
        } } while(0)



//Check that two arrays have the exact same dimensionality.
#define ASSERT_SAME_SIZE(array1,array2)					\
    do {                                                                \
        const mwSize c1nd = mxGetNumberOfDimensions(array1);            \
        const mwSize c2nd = mxGetNumberOfDimensions(array2);            \
        if (c1nd != c2nd) {                                             \
			MEX_ERR_PRINTF("arrays are not same size! %d dims vs %d dims!", c1nd, c2nd); \
        }                                                               \
        const mwSize* dims1 = mxGetDimensions(array1);                  \
        const mwSize* dims2 = mxGetDimensions(array2);                  \
        unsigned int i;                                                 \
        for (i=0; i<c1nd; i++) {                                        \
            if (dims1[i] != dims2[i]) {                                 \
				MEX_ERR_PRINTF("array1 dim %d = %d, array2 dim %d = %d", \
                               i, dims1[i], i, dims2[i]);               \
            }                                                           \
        } } while(0)

//Assert that the dimensions are correct for mask to be an alpha mask on
// the image 'image', which can have 2 or 3 channels
#define ASSERT_IMAGE_AND_MASK(image,mask)				\
    do {                                                                \
        const mwSize im_nd = mxGetNumberOfDimensions(image);            \
        const mwSize mask_nd = mxGetNumberOfDimensions(mask);           \
        if ((im_nd != 2) && (im_nd != 3)) {                             \
			MEX_ERR_PRINTF("'image' has %d dimensions", im_nd);  \
        }                                                               \
        else if (mask_nd != 2) {                                        \
			MEX_ERR_PRINTF("mask doesn't have 2 dimensions"); \
        }                                                               \
        const mwSize* im_dims = mxGetDimensions(image);                 \
        const mwSize* mask_dims = mxGetDimensions(mask);                \
        if (im_dims[0] != mask_dims[0]) {                               \
			MEX_ERR_PRINTF("image first dim=%d, mask first dim = %d", \
						   im_dims[0], mask_dims[0]); \
        }                                                               \
        else if (im_dims[1] != mask_dims[1]) {                          \
			MEX_ERR_PRINTF("image second dim=%d, mask second dim=%d", \
                           im_dims[1], mask_dims[1]); \
        }                                                               \
    } while(0)
    
    
//As above but also asserts that the image has 3 channels
#define ASSERT_FULL_IMAGE_AND_MASK(image,mask)                          \
    ASSERT_IMAGE_AND_MASK(image,mask);                                  \
    do {                                                                \
        const mwSize numd = mxGetNumberOfDimensions(image);             \
        if (numd != 3) {                                                \
			MEX_ERR_PRINTF("image has dimensionality %d, != 3", numd); \
        }                                                               \
        const mwSize* im_dims = mxGetDimensions(image);                 \
        if (im_dims[2] != 3) {                                          \
			MEX_ERR_PRINTF("size of image's 3rd dimension is %d, expecting 3!", im_dims[2]); \
        }                                                               \
    } while (0)

//make sure this is whatever type we define our pointers to be packed into
#define IS_POINTER(x) mxIsInt64(x)

#define ASSERT_IS_POINTER(array)                                        \
    do {                                                                \
        if (!IS_POINTER(array)) {                                       \
			MEX_ERR_PRINTF("expected a pointer!"); \
        }                                                               \
    } while(0)

#define ASSERT_IS_STRING(array)                                         \
    do {                                                                \
        if (!mxIsChar(array)) {                                         \
			MEX_ERR_PRINTF("expected a string!"); \
        }                                                               \
    } while(0)

#define ASSERT_NON_NULL(ptr) \
    do {  \
		if (ptr == null) { \
			MEX_ERR_PTRINT("ptr %s is null", #ptr);\
		} \
	} while(0)


//Get an unsigned int from an mxArray. Most useful when the mxArray represents a scalar.
#define SCALAR_GET_SINGLE(array) *((float *)mxGetPr(array))
#define SCALAR_GET_DOUBLE(array) *((double *)mxGetPr(array))
#define SCALAR_GET_UINT32(array) *((unsigned int *)mxGetPr(array))
#define SCALAR_GET_SINT32(array) *((signed int *)mxGetPr(array))
#define SCALAR_GET_SINT64(array) *((signed long *)mxGetPr(array))





#endif


