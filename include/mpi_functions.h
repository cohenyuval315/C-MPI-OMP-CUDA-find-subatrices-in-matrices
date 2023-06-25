#ifndef MPI_FUNC
#define MPI_FUNC

#define WORK_TAG 0
#define WORK_DATA_TAG 1
#define WORK_SIZE_TAG 2
#define DIE_TAG 3
#define RESULT_TAG 4
#define NO_RESULT_TAG 5
#define RESULT_SIZE_TAG 6
#define RESULT_DATA_TAG 7
#define FREE_TAG 8

#define PRINT_MPI_SUCCESS 1
#define MPI_EXIT_ON_ERROR 0
#include "struct.h"

void master_routine(int num_matrices, int num_submatrices,Position** all_positions,int* num_positions_matrix, Matrix* matrices,MPI_Datatype positionType ,MPI_Status status);
void worker_routine(int rank ,Matrix* submatrices, int num_submatrices,double matching_value,MPI_Datatype positionType,MPI_Status status);
void createPositionDatatype(MPI_Datatype *positionType);
char* pack_matrices(Matrix* submatrices, int *num_submatrices,int* buffer_size);
Matrix* unpack_matrices(char* buffer, int buffer_size, int* num_submatrices);
char* pack_matrix(Matrix mat);
Matrix unpack_matrix(char* buffer, int buffer_size);
void mpi_error_check(int errorcode,const char* msg);
void mpi_null_check(void* ptr, const char* msg);


#endif