
#change this for your platform - this is for 64 bit os x
MEX = mex
MEX_SUFFIX = mexmaci64
MEX_FLAGS = #-Wall
OBJ_SUFFIX = o
DELETE_CMD = rm

#this command should contain the include directories, library directories, and
#also the linking commands for the different opencv libraries. I personally have
#a shell script which prints all this out, so I use `opencv-comp`, but you could
#type it out manually here as well
OCV_DIRS = `opencv-comp`


UTILS = mex_utils.$(OBJ_SUFFIX) opencv_matlab_interop.$(OBJ_SUFFIX) 

RANDOM_FOREST_FUNCS = $(addprefix random_forest/, rf_delete rf_info rf_load rf_oob rf_predict rf_save rf_train)

GEOM_FUNCS = $(addprefix geometric-image-transformations/,cvFindExtrinsics)
CALIB_FUNCS = $(addprefix calibration/,posit)

MEX_FUNCS = $(addsuffix .$(MEX_SUFFIX),$(GEOM_FUNCS) $(RANDOM_FOREST_FUNCS) $(CALIB_FUNCS))

%.o : %.cpp %.h
	$(MEX) $(MEX_FLAGS) -c $< $(OCV_DIRS)

%.$(MEX_SUFFIX) : %.cpp $(UTILS)
	$(MEX) $(MEX_FLAGS) $< $(UTILS) $(OCV_DIRS) -o $@

all: utils $(MEX_FUNCS)

utils: $(UTILS)

clean:
	$(DELETE_CMD) -rf *.$(OBJ_SUFFIX)
	$(DELETE_CMD) -rf $(MEX_FUNCS)


