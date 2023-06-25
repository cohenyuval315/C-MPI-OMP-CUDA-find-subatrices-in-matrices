
NVCC = nvcc
CC = mpicxx
MPICXX_FLAG = -fopenmp
MPIEXEC = mpiexec
MPIEXEC_FLAGS = -np 2
MPI_MACHINE_FLAGS = -hostfile hosts.txt -map-by  node 
DEBUG = gdb
DEBUG_FLAGS = -ex run --args
CC_FLAGS = -g
NVCC_FLAGS = -gencode arch=compute_61,code=sm_61
LIBS = -L/usr/lib/x86_64-linux-gnu -lcudart
INC = -Iinclude
SRC = src
OBJ = obj
TARGET = bin/program

OUTPUT_FILE = output.txt
build:
	$(CC) $(MPICXX_FLAG) $(INC)  -c $(SRC)/main.c -o $(OBJ)/main.o
	$(CC) $(MPICXX_FLAG)  $(INC)  -c $(SRC)/c_functions.c -o $(OBJ)/c_functions.o
	$(CC) $(MPICXX_FLAG)  $(INC)  -c $(SRC)/mpi_functions.c -o $(OBJ)/mpi_functions.o
	$(CC) $(MPICXX_FLAG)  $(INC)  -c $(SRC)/omp_functions.c -o $(OBJ)/omp_functions.o
	$(NVCC) $(INC) $(NVCC_FLAGS) -c $(SRC)/cuda_functions.cu -o $(OBJ)/cuda_functions.o
	$(CC) $(CC_FLAGS) $(MPICXX_FLAG) -o $(TARGET) $(OBJ)/main.o $(OBJ)/c_functions.o $(OBJ)/mpi_functions.o $(OBJ)/omp_functions.o $(OBJ)/cuda_functions.o  $(LIBS) 
	

clean:
	rm -f $(OBJ)/*.o $(TARGET) $(OUTPUT_FILE)

debug:
	$(DEBUG) $(DEBUG_FLAGS) $(MPIEXEC) $(MPIEXEC_FLAGS) ./$(TARGET)

run:
	$(MPIEXEC) $(MPIEXEC_FLAGS) ./$(TARGET)

runOn2:
	$(MPIEXEC) $(MPIEXEC_FLAGS) $(MPI_MACHINE_FLAGS) ./$(TARGET)

