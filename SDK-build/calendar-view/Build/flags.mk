
DEBUG_OP = 
CPP_DEBUG_OP = -g3 

OPTIMIZATION_OP = 
CPP_OPTIMIZATION_OP = -O0 

COMPILE_FLAGS = $(DEBUG_OP) $(OPTIMIZATION_OP) 

CPP_COMPILE_FLAGS = $(CPP_DEBUG_OP) $(CPP_OPTIMIZATION_OP) -Wall -c -fmessage-length=0 -std=c++0x -fPIC 

LINK_FLAGS = -Xlinker -rpath=$$"ORIGIN"/../lib -shared -Wl,--no-undefined 

AR_FLAGS = 

EDC_COMPILE_FLAGS = 