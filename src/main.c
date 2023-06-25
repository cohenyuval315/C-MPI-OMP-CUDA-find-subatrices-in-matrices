#include <mpi.h>
#include <stdlib.h>
#include "config.h"
#include "mpi_functions.h"
#include "omp_functions.h"
#include "cuda_functions.h"
#include "c_functions.h"


void check_num_processes(int num_processes){
    if (num_processes < 2){
        printf("please run the program with more then 1 process \n");
        MPI_Abort(MPI_COMM_WORLD,__LINE__);
        exit(0);
    }
}

void parallel_search_submatrices_in_matrix(int argc, char *argv[]){
    MPI_Status status;
    int mpi_error;
    int num_processes;
    int rank;
    int num_matrices;
    int num_submatrices;
    double matching_value;
    Matrix* matrices;    
    Matrix* submatrices;
    Position** all_positions;
    int* num_positions_per_matrix;
    double start_time;
    double end_time;

    int max_num_positions_per_matrix; 

    MPI_Init(&argc, &argv);
    MPI_Comm_set_errhandler(MPI_COMM_WORLD, MPI_ERRORS_RETURN);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Datatype positionType;
    createPositionDatatype(&positionType);

    if (num_processes < 2){
        printf("please run the program with more then 1 process \n");
        MPI_Abort(MPI_COMM_WORLD,__LINE__);
        exit(0);
    }
    
    if (rank == 0){
        printf("-num of processes: %d \n", num_processes);
        read_input_file(INPUT_FILENAME,&matching_value,&matrices,&submatrices,&num_matrices,&num_submatrices);

        if(num_processes > num_matrices){
            printf("-NOTE: centeralized dynamic load balancing with %d number of matrices , with %d number of processes , will have %d idle processes\n",num_matrices,num_processes, num_matrices - num_processes);
        }
        printf("---------\n");

       all_positions = (Position**)malloc(num_matrices * sizeof(Position*));
       mpi_null_check(all_positions,"fail to allocate memory to all positions array");
       num_positions_per_matrix = (int*)malloc(num_matrices * sizeof(int));
       mpi_null_check(num_positions_per_matrix,"fail to allocate memory to number of position array");
        //print_matrices(matrices,num_matrices,false);
        //print_matrices(submatrices,num_submatrices,false);
    }
    start_time = MPI_Wtime(); 

    // <-- broadcast values (objects, matching value , number of objects) : start -->
    char* buffer;
    int buffer_size;    
    mpi_error = MPI_Bcast(&matching_value,1,MPI_FLOAT,0,MPI_COMM_WORLD);
    mpi_error_check(mpi_error,"bcast matching value");
    mpi_error = MPI_Bcast(&num_submatrices,1,MPI_INT,0,MPI_COMM_WORLD);
    mpi_error_check(mpi_error,"bcast num submatrices");
    
    // if(rank == 0){
    //     buffer = pack_matrices(submatrices,&num_submatrices);
    //     buffer_size = ((int*)buffer)[0];
    // }
    if (rank == 0){
        buffer = pack_matrices(submatrices,&num_submatrices,&buffer_size);
    }
    mpi_error = MPI_Bcast(&buffer_size,1,MPI_INT,0,MPI_COMM_WORLD);
    mpi_error_check(mpi_error,"bcast buffer size");
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank != 0){
        buffer = (char*)malloc(buffer_size);
        mpi_null_check(buffer,"fail to allocate memory for buffer size of objects");
    }

    mpi_error = MPI_Bcast(buffer,buffer_size,MPI_CHAR,0,MPI_COMM_WORLD);
    mpi_error_check(mpi_error,"bcast buffer data");
    if(rank != 0){
        submatrices = unpack_matrices(buffer,buffer_size,&num_submatrices);
        //print_matrices(submatrices,num_submatrices,false);
    }
    
    free(buffer);
    // <-- broadcast values : end -->



    // <-- work routine: start -->
    if(rank == 0){
        master_routine(num_matrices,num_submatrices,all_positions,num_positions_per_matrix,matrices,positionType,status);
    }else{
        worker_routine(rank,submatrices,num_submatrices,matching_value,positionType,status);
    }
    // <-- work routine: end -->

    

    end_time = MPI_Wtime();
    printf("process rank : %d runtime : %f\n",rank,end_time-start_time);
    mpi_error = MPI_Barrier(MPI_COMM_WORLD);
    mpi_error_check(mpi_error,"mpi barrier end");
    end_time = MPI_Wtime();

    if (rank == 0) { 
        create_output_file(OUTPUT_FILENAME,all_positions,num_positions_per_matrix,num_matrices);
        printf("global runtime : %f, please checkout output file which named : %s \n",end_time-start_time, OUTPUT_FILENAME);
        free_matrices(matrices,num_matrices);
    }
    free_matrices(submatrices,num_submatrices);
    mpi_error = MPI_Type_free(&positionType);
    mpi_error_check(mpi_error,"error mpi type free");
    MPI_Finalize();
}


int main(int argc, char *argv[]) {
    parallel_search_submatrices_in_matrix(argc,argv);
    return 0;
}
