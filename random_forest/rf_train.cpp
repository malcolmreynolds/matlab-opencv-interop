#include "../opencv_matlab_interop.h"

//#define PRINT_INPUTS 

//maximum depth of each tree
const int DEFAULT_MAX_DEPTH = 25;

//if there are fewer than this many samples in a node, don't bother splitting
const int DEFAULT_MIN_SAMPLE_COUNT = 5;

//one of the "stop splitting" criteria variables - still not sure about this?
const float DEFAULT_REGRESSION_ACCURACY = 0.00001;

//only useful if dealing with missing features
const bool DEFAULT_USE_SURROGATES = false;

//relates to the number of categorical types - preclustering will happen before this
const int DEFAULT_MAX_CATEGORIES = 10;

//priors on the probability of each class
float* DEFAULT_PRIORS = NULL;

//whether to calculate the importance of each variable
const bool DEFAULT_CALC_VAR_IMPORTANCE = true;

//size of the random subset of features to test at each node
const int DEFAULT_NUM_ACTIVE_VARS = 10;

//maximum number of trees
const int DEFAULT_MAX_TREE_COUNT = 200;

//These go into the termination of the training procecure
const float DEFAULT_FOREST_ACCURACY = 0.1;
const int DEFAULT_TERM_CRITERIA_TYPE = CV_TERMCRIT_ITER + CV_TERMCRIT_EPS;

CvRTParams* parse_struct_to_forest_config(const mxArray *);
void print_forest_params(const CvRTParams* params);

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    ASSERT_NUM_RHS_ARGS_GTE(2);
    ASSERT_NUM_LHS_ARGS_LT(2);

    const mxArray* dataMtx = prhs[0];
    const mxArray* targetValueVec = prhs[1];
    
    //see if we have been provided a struct containing options for the training. 
    //if not, then use defaults provided by opencv
    CvRTParams* rtParams;
    if (nrhs > 2) {
        mexPrintf("Parsing struct argument for parameters\n");
        rtParams = parse_struct_to_forest_config(prhs[2]);
    }
    else {
        mexPrintf("Using default parameters\n");
        rtParams = parse_struct_to_forest_config(NULL);
    }

    mexPrintf("Parameters:\n");
    print_forest_params(rtParams);
    
    unsigned int numSamples, numVariables;

    CvMat* dataCvMtx = matlab_matrix_to_opencv_matrix(dataMtx);
    numSamples = dataCvMtx->rows;
    numVariables = dataCvMtx->cols;
    mexPrintf("training data converted to opencv format. %d samples, each with %d variables\n",
              numSamples, numVariables);
#ifdef PRINT_INPUTS
    print_opencv_matrix(dataCvMtx);
#endif

    CvMat* targetCvMtx = matlab_array_to_opencv_array(targetValueVec);
    if (targetCvMtx->rows != numSamples) {
        char msgbuf[ERR_MSG_SIZE];
        sprintf(msgbuf,"%s:%d training data had %d samples, labels contains %d values\n",
                __FILE__,__LINE__,numSamples, targetCvMtx->rows);
        mexErrMsgTxt(msgbuf);
    }
    mexPrintf("training labels converted to opencv format.\n");
#ifdef PRINT_INPUTS
    print_opencv_matrix(targetCvMtx);
#endif

    //specify the type of our variables. In this case, all our variables are 
    CvMat* var_type = cvCreateMat(dataCvMtx->cols + 1, 1, CV_8U);
    cvSet(var_type, cvScalarAll(CV_VAR_ORDERED));

    //actually make the forest and do the training
    mexPrintf("training now...");
    CvRTrees *forest = new CvRTrees();
    forest->train(dataCvMtx, CV_ROW_SAMPLE, targetCvMtx, NULL, NULL, var_type, NULL, *rtParams);
    mexPrintf("training done.\n");

    //pack the pointer and return it to matlab
    plhs[0] = pack_pointer((void *)forest);
    

    cvReleaseMat(&var_type);
    cvReleaseMat(&dataCvMtx);
    cvReleaseMat(&targetCvMtx);
} 

//parse the matlab struct to get all the options for training
CvRTParams* parse_struct_to_forest_config(const mxArray *trainingOptions) {
    int numFields = 0;
    if (trainingOptions != NULL) {
        numFields = mxGetNumberOfFields(trainingOptions);
    }

    mexPrintf("%d training fields provided\n", numFields);

    //these should be the default values that we would get from instantiating
    //a CvRTParams object.
    int maxDepth = DEFAULT_MAX_DEPTH;
    int minSampleCount = DEFAULT_MIN_SAMPLE_COUNT;
    float regressionAccuracy = DEFAULT_REGRESSION_ACCURACY;
    bool useSurrogates = DEFAULT_USE_SURROGATES;
    int maxCategories = DEFAULT_MAX_CATEGORIES;
    float* priors = DEFAULT_PRIORS;
    bool calcVarImportance = DEFAULT_CALC_VAR_IMPORTANCE;
    int numActiveVars = DEFAULT_NUM_ACTIVE_VARS;
    int maxTreeCount = DEFAULT_MAX_TREE_COUNT;
    float forestAccuracy = DEFAULT_FOREST_ACCURACY;
    int termCriteriaType = DEFAULT_TERM_CRITERIA_TYPE;

    for (int i=0; i < numFields; i++) {
        const char *name = mxGetFieldNameByNumber(trainingOptions, i);
        mexPrintf("field %d is %s\n",i,name);
        mxArray *field = mxGetFieldByNumber(trainingOptions, 0, i);
        if (num_elements(field) != 1) { 
            mexWarnMsgIdAndTxt("train_random_forest:non_scalar_option", 
                               "field is not a scalar! cannot parse into Random Forest options.");
            continue;
        }
        
        //now need to compare against all the names we recognise
        if (strcmp(name, "max_depth") == 0) {
            if (mxIsInt32(field)) {
                maxDepth = SCALAR_GET_SINT32(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_int32","max_depth field must be a int32. Skipping...\n");
            }
            continue;
        }
        else if (strcmp(name, "min_sample_count") == 0) {
            if (mxIsInt32(field)) {
                minSampleCount = SCALAR_GET_SINT32(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_int32","min_sample_count field must be a int32. Skipping...\n");
            }
            continue;
        }
        else if (strcmp(name, "regression_accuracy") == 0) {
            if (mxIsDouble(field)) {
                regressionAccuracy = (float)SCALAR_GET_DOUBLE(field);
            }
            else if (mxIsSingle(field)) {
                regressionAccuracy = SCALAR_GET_SINGLE(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_float","regression_accuracy field must be a single or double. Skipping...\n");
            }
            continue;
        }
        else if (strcmp(name, "use_surrogates") == 0) {
            if (mxIsLogicalScalar(field)) {
                useSurrogates = mxIsLogicalScalarTrue(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_bool","use_surrogates field must be a boolean/logical. skipping...\n");
            }
            continue;
        }
        else if (strcmp(name, "max_categories") == 0) {
            if (mxIsInt32(field)) {
                maxCategories = SCALAR_GET_SINT32(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_int32","max_categories field must be an int32. Skipping...\n");
            }
            continue;
        }
        else if (strcmp(name, "calc_var_importance") == 0) {
            if (mxIsLogicalScalar(field)) {
                calcVarImportance = mxIsLogicalScalarTrue(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_bool","calc_var_importance field must be a boolean/logical. skipping...\n");
            }
            continue;
        }
        else if (strcmp(name, "num_active_vars") == 0) {
            if (mxIsInt32(field)) {
                numActiveVars = SCALAR_GET_SINT32(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_int32","num_active_vars field must be an int32. Skipping...\n");
            }
            continue;
        }
        else if (strcmp(name, "max_tree_count") == 0) {
            if (mxIsInt32(field)) {
                maxTreeCount = SCALAR_GET_SINT32(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_int32","max_tree_count field must be an int32. Skipping...\n");
            }
            continue;
        }
        else if (strcmp(name, "forest_accuracy") == 0) {
            if (mxIsDouble(field)) {
                forestAccuracy = (float)SCALAR_GET_DOUBLE(field);
            }
            else if (mxIsSingle(field)) {
                forestAccuracy = SCALAR_GET_SINGLE(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_float","forest_accuracy field must be a float. Skipping...\n");
            }
            continue;
        }
        else if (strcmp(name, "term_criteria_type") == 0) {
            if (mxIsInt32(field)) {
                termCriteriaType = SCALAR_GET_SINT32(field);
            }
            else {
                mexWarnMsgIdAndTxt("train_random_forest:not_int32","term_critera_type field must be an int32. Skipping...\n");
            }
            continue;
        }
        else {
            char msgbuf[ERR_MSG_SIZE];
            sprintf(msgbuf,"field provided was not recognised: %s",name);
            mexWarnMsgIdAndTxt("train_random_forest:unrecognised_field",msgbuf);
        }
    }

    return new CvRTParams(maxDepth, minSampleCount, regressionAccuracy,
                          useSurrogates, maxCategories, priors,
                          calcVarImportance, numActiveVars, maxTreeCount,
                          forestAccuracy, termCriteriaType);
}

//print out parameters using mexPrintf
void print_forest_params(const CvRTParams* params) {
    mexPrintf("max depth: %d\n", params->max_depth);
    mexPrintf("min sample count: %d\n", params->min_sample_count);
    mexPrintf("regression accuracy: %f\n", params->regression_accuracy);
    mexPrintf("use surrogates: %s\n", params->use_surrogates ? "yes" : "no");
    mexPrintf("max categories: %d\n", params->max_categories);
    mexPrintf("priors: unprintable...\n");
    mexPrintf("calc var importance: %s\n", params->calc_var_importance ? "yes" : "no");
    mexPrintf("num active variables: %d\n", params->nactive_vars);
    mexPrintf("max tree count: %d\n", params->term_crit.max_iter); //need to check if this is right!
    mexPrintf("forest accuracy: %f\n", params->term_crit.epsilon); //ditto
    switch(params->term_crit.type) {
    case 0:
        mexPrintf("term criteria type: none (?)\n");
        break;
    case CV_TERMCRIT_ITER:
        mexPrintf("term criteria type: CV_TERMCRIT_ITER\n");
        break;
    case CV_TERMCRIT_EPS:
        mexPrintf("term criteria type: CV_TERMCRIT_EPS\n");
        break;
    case (CV_TERMCRIT_EPS | CV_TERMCRIT_ITER):
        mexPrintf("term criteria type: CV_TERMCRIT_ITER | CV_TERMCRIT_EPS\n");
        break;
    default:
        mexPrintf("term criteria type: unknown!!\n");
        break;
    }
}
