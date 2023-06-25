#include <stdlib.h>
#include <mpi.h>
#include "mpi_functions.h"
#include "struct.h"
#include "config.h"
#include "c_functions.h"

void mpi_null_check(void* ptr, const char* msg){
    if(ptr == NULL){
        printf("%s\n", msg);
        MPI_Abort(MPI_COMM_WORLD,__LINE__);
        exit(0);
    }

}

void mpi_error_check(int errorcode, const char* msg){
    if (errorcode == 0){
        if (PRINT_MPI_SUCCESS){
            return;
        }
    }
    int eclass,len;
    char estring[MPI_MAX_ERROR_STRING];
    MPI_Error_class(errorcode, &eclass);
    MPI_Error_string(errorcode, estring, &len);
    printf("Error %d: %s\n", eclass, estring);fflush(stdout);
    if (errorcode != 0){
        printf("%s\n",msg);
        if(MPI_EXIT_ON_ERROR){
            MPI_Abort(MPI_COMM_WORLD,__LINE__);
            exit(0);
        }
    }

}


void master_routine(int num_matrices, int num_submatrices,Position** all_positions,int* num_positions_per_matrix, Matrix* matrices,MPI_Datatype positionType ,MPI_Status status){
    /*
    master process send to each process available jobs until no more jobs available, and wait until result is coming from each process and then allocate another job to that process.
    */
    int error;
    int current_matrix = 0;
    int num_completed = 0;
    char* buffer;
    int buffer_size;
    while (num_completed < num_matrices) {      
        error = MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        mpi_error_check(error,"master mpi recv any");
        int worker_rank = status.MPI_SOURCE;
        printf("completd matrices:%d total matrices:%d, current matrix:%d, sending to rank %d\n", num_completed,num_matrices,current_matrix,worker_rank);
        if(status.MPI_TAG == FREE_TAG){ // process free, send work
            if (current_matrix < num_matrices) {
                error = MPI_Send(NULL, 0, MPI_INT, worker_rank, WORK_TAG, MPI_COMM_WORLD);
                mpi_error_check(error,"master mpi send work tag");
                buffer = pack_matrix(matrices[current_matrix]);
                buffer_size = ((int*)buffer)[0];
                error = MPI_Send(&buffer_size, 1, MPI_INT, worker_rank, WORK_SIZE_TAG, MPI_COMM_WORLD);
                mpi_error_check(error,"master send work size");
                error = MPI_Send(buffer, buffer_size + sizeof(int), MPI_CHAR, worker_rank, WORK_DATA_TAG, MPI_COMM_WORLD);
                mpi_error_check(error,"master send work");
                free(buffer);
                current_matrix++;
            }
        }
        if(status.MPI_TAG == RESULT_TAG){ // process finish work,  recv result
            int num_positions;
            error = MPI_Recv(&num_positions, 1, MPI_INT, worker_rank, RESULT_SIZE_TAG, MPI_COMM_WORLD, &status);
            mpi_error_check(error,"master recv result size");
            Position* matrix_positions = (Position*)malloc(num_positions * sizeof(Position));
            error = MPI_Recv(matrix_positions, num_positions, positionType, worker_rank, RESULT_DATA_TAG, MPI_COMM_WORLD, &status);
            mpi_error_check(error,"master recv result data");
            num_positions_per_matrix[num_completed] = num_positions;
            all_positions[num_completed] = matrix_positions;
            num_completed++;
        }
        if(status.MPI_TAG == NO_RESULT_TAG){
            num_positions_per_matrix[num_completed] = 0;
            num_completed++;
        }        
        
        if (current_matrix + 1 > num_matrices) { // no more work , send termination to worker
            error = MPI_Send(NULL, 0, MPI_INT, worker_rank, DIE_TAG, MPI_COMM_WORLD);          
            mpi_error_check(error,"master terminate workers");
        }
    }
}

void worker_routine(int rank ,Matrix* submatrices, int num_submatrices,double matching_value,MPI_Datatype positionType,MPI_Status status){
    /*
    worker process send to master that he got no job, then process the job he recieved from the master , and send result back.
    */
    int error;
    char* recv_buffer;
    int buffer_size;
    Matrix mat;
    while (1) {
        error = MPI_Send(NULL, 0, MPI_INT, 0, FREE_TAG, MPI_COMM_WORLD);
        mpi_error_check(error,"worker send free");
        error = MPI_Recv(NULL, 0 ,MPI_INT, 0, MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        mpi_error_check(error,"worker recv any");
        if(status.MPI_TAG == DIE_TAG){
            break;
        }            
        if(status.MPI_TAG == WORK_TAG){
            error = MPI_Recv(&buffer_size,1,MPI_INT,0,WORK_SIZE_TAG,MPI_COMM_WORLD,&status);
            mpi_error_check(error,"worker recv work tag");
            recv_buffer = (char*)malloc(buffer_size + sizeof(int));
            error = MPI_Recv(recv_buffer,buffer_size + sizeof(int),MPI_CHAR,0,WORK_DATA_TAG,MPI_COMM_WORLD, &status);                
            mpi_error_check(error,"worker recv work");
            mat = unpack_matrix(recv_buffer + sizeof(int),buffer_size + sizeof(int));

            int total_num_positions = 0;
            Position* matrix_positions = search_submatrices_in_matrix(&total_num_positions,mat,submatrices,&num_submatrices,&matching_value);      
            if(total_num_positions != 0){
                
                error = MPI_Send(NULL, 0, MPI_INT,0 , RESULT_TAG, MPI_COMM_WORLD);
                mpi_error_check(error, "worker send result tag");
                error = MPI_Send(&total_num_positions, 1, MPI_INT, 0, RESULT_SIZE_TAG, MPI_COMM_WORLD);
                mpi_error_check(error ,"worker send result size");
                error = MPI_Send(matrix_positions, total_num_positions , positionType, 0, RESULT_DATA_TAG, MPI_COMM_WORLD);
                mpi_error_check(error, "worker send result data");
            }else{
                error = MPI_Send(NULL, 0, MPI_INT,0 , NO_RESULT_TAG, MPI_COMM_WORLD);
                mpi_error_check(error,"worker send no result tag");
            }
            free(recv_buffer);
            free(matrix_positions);                
        }
    }
}

void createPositionDatatype(MPI_Datatype *positionType) {
    // MPI DATA TYPE for easy passing positions between processes.
    int blocklengths[5] = {1, 1, 1, 1, 1};
    MPI_Aint offsets[5];
    MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint startAddress, address1, address2, address3, address4;
    MPI_Get_address(&((Position*)0)->ID, &startAddress);
    MPI_Get_address(&((Position*)0)->matrixID, &address1);
    MPI_Get_address(&((Position*)0)->submatrixID, &address2);
    MPI_Get_address(&((Position*)0)->i, &address3);
    MPI_Get_address(&((Position*)0)->j, &address4);
    offsets[0] = 0;
    offsets[1] = address1 - startAddress;
    offsets[2] = address2 - startAddress;
    offsets[3] = address3 - startAddress;
    offsets[4] = address4 - startAddress;
    MPI_Type_create_struct(5, blocklengths, offsets, types, positionType);
    MPI_Type_commit(positionType);
}

char* pack_matrices(Matrix* matrices, int *num_matrices, int* n_buffer_size){
    int error;
    char* buffer;
    int pos = 0;
    int buffer_size = 0;
    int size;
    int mat_buffer_size;

    for(int i=0; i<*num_matrices; i++){
        size = matrices[i].rows * matrices[i].cols;
        mat_buffer_size = 3 * sizeof(int) + size * sizeof(double);
        buffer_size += mat_buffer_size;
    }
    *n_buffer_size = buffer_size;
    buffer = (char*) malloc(buffer_size);
    mpi_null_check(buffer,"fail to pack");
    
    for(int i=0; i<*num_matrices; i++){
        Matrix mat = matrices[i]; 
        int ID = mat.ID;
        int rows = mat.rows;
        int cols = mat.cols;
        double* mat_data = mat.data;
        size = rows * cols;
        error = MPI_Pack(&ID, 1, MPI_INT, buffer, buffer_size, &pos, MPI_COMM_WORLD);
        mpi_error_check(error,"pack matrices - pack matrix buffer size");
        error = MPI_Pack(&rows, 1, MPI_INT, buffer, buffer_size, &pos, MPI_COMM_WORLD);
        mpi_error_check(error,"pack matrices - pack number rows ");
        error = MPI_Pack(&cols, 1, MPI_INT, buffer, buffer_size, &pos, MPI_COMM_WORLD);
        mpi_error_check(error,"pack matrices - pack number cols");
        
        // int expected_size;
        // error = MPI_Pack_size(size, MPI_DOUBLE, MPI_COMM_WORLD, &expected_size);
        // printf("---\nmatrix %d size=%d,pos = %d ,expected = %d\n---\n",i,size, pos,expected_size);

        error = MPI_Pack(mat_data, size, MPI_DOUBLE, buffer, buffer_size, &pos, MPI_COMM_WORLD);
        mpi_error_check(error,"pack matrices - pack matrix data");
    }
    
    return buffer;
}

Matrix* unpack_matrices(char* buffer, int buffer_size, int* num_matrices){
    //print_hex(buffer,buffer_size);
    int error;
    int pos = 0;
    Matrix* matrices = (Matrix*)malloc(sizeof(Matrix) * (*num_matrices));
    mpi_null_check(matrices,"fail to unpack matrices");

    for(int i=0; i<(*num_matrices); i++){
        error = MPI_Unpack(buffer, buffer_size, &pos, &(matrices[i].ID), 1, MPI_INT, MPI_COMM_WORLD);
        mpi_error_check(error,"unpack matrices - unpack buffer size");
        error = MPI_Unpack(buffer, buffer_size, &pos, &(matrices[i].rows), 1, MPI_INT, MPI_COMM_WORLD);
        mpi_error_check(error,"unpack matrices - unpack number rows");
        error = MPI_Unpack(buffer, buffer_size, &pos, &(matrices[i].cols), 1, MPI_INT, MPI_COMM_WORLD);
        mpi_error_check(error,"unpack matrices - unpack number cols");
        int data_size = matrices[i].rows * matrices[i].cols;
        matrices[i].data = (double*)malloc(data_size * sizeof(double));
        mpi_null_check(matrices[i].data,"fail to unpack matrix");
        error = MPI_Unpack(buffer, buffer_size, &pos, matrices[i].data, data_size, MPI_DOUBLE, MPI_COMM_WORLD);
        mpi_error_check(error,"unpack matrices - unpack matrix data");
    }
    return matrices;
}

char* pack_matrix(Matrix mat){
    int error;
    char* buffer;
    int pos = 0;
    int data_size = mat.rows * mat.cols;
    int buffer_size = 3 * sizeof(int) +  data_size * sizeof(double);
    buffer = (char*) malloc(buffer_size + sizeof(int));
    mpi_null_check(buffer,"fail to pack");
    ((int*)buffer)[0] = buffer_size; // buffer size
    error = MPI_Pack(&(mat.ID), 1, MPI_INT, buffer + sizeof(int), buffer_size, &pos, MPI_COMM_WORLD);
    mpi_error_check(error,"pack matrix - pack buffer size");
    error = MPI_Pack(&(mat.rows), 1, MPI_INT, buffer + sizeof(int), buffer_size, &pos, MPI_COMM_WORLD);
    mpi_error_check(error,"pack matrix - pack num rows");
    error = MPI_Pack(&(mat.cols), 1, MPI_INT, buffer + sizeof(int), buffer_size, &pos, MPI_COMM_WORLD);
    mpi_error_check(error,"pack matrix - pack num cols");
    error = MPI_Pack(mat.data, data_size, MPI_DOUBLE, buffer + sizeof(int), buffer_size, &pos, MPI_COMM_WORLD);
    mpi_error_check(error,"pack matrix - pack data");
    return buffer;
    
}

Matrix unpack_matrix(char* buffer, int buffer_size){
    Matrix mat;
    int error;
    int pos = 0;
    error = MPI_Unpack(buffer, buffer_size, &pos, &(mat.ID), 1, MPI_INT, MPI_COMM_WORLD);
    mpi_error_check(error,"unpack matrix - unpack buffer size");
    error = MPI_Unpack(buffer, buffer_size, &pos, &(mat.rows) , 1, MPI_INT, MPI_COMM_WORLD);
    mpi_error_check(error,"unpack matrix - unpack num rows");
    error = MPI_Unpack(buffer, buffer_size, &pos, &(mat.cols) , 1, MPI_INT, MPI_COMM_WORLD);
    mpi_error_check(error,"unpack matrix - unpack num cols");
    int data_size = mat.rows * mat.cols;
    mat.data = (double*) malloc(data_size * sizeof(double));
    mpi_null_check(mat.data,"fail to unpack");
    error = MPI_Unpack(buffer, buffer_size, &pos, mat.data, data_size, MPI_DOUBLE, MPI_COMM_WORLD);
    mpi_error_check(error,"unpack matrix - unpack data");
    return mat;
}
