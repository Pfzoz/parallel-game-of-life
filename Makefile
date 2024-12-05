OBJS = main.cpp
OBJ_NAME = main

# Check if the parallel flag is passed
ifeq ($(PARALLEL), 1)
    OPENMP_FLAG = -fopenmp
else
    OPENMP_FLAG =
endif

# Default target
all: $(OBJS)
	g++ $(OBJS) -w -lSDL2 -o $(OBJ_NAME).x $(OPENMP_FLAG)
