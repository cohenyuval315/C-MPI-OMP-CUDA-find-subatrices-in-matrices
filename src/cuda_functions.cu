#include <stdio.h>
#include "cuda_functions.h"
#include "config.h"
#include "struct.h"

void cuda_null_check(void* ptr , const char* msg){
    if(ptr == NULL){
        printf("%s\n",msg);
        exit(0);
    }
}

void CUDA_error_check(cudaError_t error, const char* msg){
    if (error != cudaSuccess)
	{
		printf("%s; Cuda Error:= %s (%d)\n",msg, cudaGetErrorString(error), error);
    	exit(0);
  	}
}

__device__ double CUDA_diff(double matrix_element, double submatrix_element){
    return abs((matrix_element - submatrix_element) /matrix_element);
}

__host__ void CUDA_matrix_to_GPU(Matrix mat, double **devMatrixPtr) 
{
	int size = mat.rows * mat.cols;
	cudaError_t error = cudaSuccess;
	error = cudaMalloc(devMatrixPtr, size * sizeof(double));
    CUDA_error_check(error,"fail to allocate memory for matrix ptr in GPU");
  	error = cudaMemcpy(*devMatrixPtr, mat.data, size * sizeof(double), cudaMemcpyHostToDevice);
    CUDA_error_check(error,"fail to copy memory for matrix ptr in GPU");
}

void CUDA_check_matrix_in_GPU(Matrix mat, double* devMatrixPtr){
    cudaError_t error = cudaSuccess;
    int size = mat.cols * mat.rows;
    double arr[size];
    error = cudaMemcpy(arr,devMatrixPtr,size * sizeof(double),cudaMemcpyDeviceToHost);
    CUDA_error_check(error,"fail to copy matrix from device to host");
}

double*  CUDA_get_matrix_from_device(double* arr , Matrix mat, double* devMatrixPtr){
    cudaError_t error = cudaSuccess;
    int size = mat.cols * mat.rows;
    arr = (double*)malloc(size* sizeof(double));
    cuda_null_check(arr,"fail to allocate memory for matrix");
    CUDA_check_matrix_in_GPU(mat,devMatrixPtr);
    error = cudaMemcpy(arr,devMatrixPtr,size * sizeof(double),cudaMemcpyDeviceToHost);
    CUDA_error_check(error,"fail to copy matrix from device to host");
    return arr;
}

void CUDA_check_array_in_GPU(double* devMatrixPtr, int size){
    cudaError_t error = cudaSuccess;
    double arr[size];
    error = cudaMemcpy(arr,devMatrixPtr,size * sizeof(double),cudaMemcpyDeviceToHost);
    CUDA_error_check(error,"fail to copy matrix from device to host");
}

double*  CUDA_get_array_from_device(double* devMatrixPtr, int size){
    cudaError_t error = cudaSuccess;
    double* arr = (double*)malloc(size* sizeof(double));
    cuda_null_check(arr,"fail to allocate memory for arr");
    CUDA_check_array_in_GPU(devMatrixPtr, size);
    error = cudaMemcpy(arr,devMatrixPtr,size * sizeof(double),cudaMemcpyDeviceToHost);
    CUDA_error_check(error,"fail to copy arr from device to host");
    return arr;
}

__host__ void CUDA_free_matrix_from_GPU(double **devMatrixPtr) 
{
	cudaError_t error = cudaSuccess;
	error = cudaFree(*devMatrixPtr);
    CUDA_error_check(error,"fail to free matrix ptr memory from GPU");
}

__global__ void CUDA_calculate_matching_array_kernel(double* devMatchingValueArrayPtr, double* devMatrixPtr, double* devSubmatrixPtr, int matrix_rows, int matrix_cols, int submatrix_rows, int submatrix_cols) {
    int block_x = blockIdx.x;
    int block_y = blockIdx.y;
    int thread_x = threadIdx.x;
    int thread_y = threadIdx.y;
    int stride_x = blockDim.x * gridDim.x;
    int stride_y = blockDim.y * gridDim.y;
    int match_rows = matrix_rows - submatrix_rows + 1;
    int match_cols = matrix_cols - submatrix_cols + 1;

    for (int i = block_y * blockDim.y + thread_y; i < match_rows; i += stride_y) {
        for (int j = block_x * blockDim.x + thread_x; j < match_cols; j += stride_x) {
            int index = i * match_cols + j;
            double matching_value = 0.0;
            for (int k = 0; k < submatrix_rows; k++) {
                for (int l = 0; l < submatrix_cols; l++) {
                    int matrix_index = (i + k) * matrix_cols + j + l;
                    int submatrix_index = k * submatrix_cols + l;
                    double diff = CUDA_diff(devMatrixPtr[matrix_index], devSubmatrixPtr[submatrix_index]);
                    matching_value += diff;
                }
            }
            devMatchingValueArrayPtr[index] = matching_value;
        }
    }
}

double* CUDA_get_matrix_matching_array_in_matrix(double* devMatrixPtr,double *devSubmatrixPtr, Matrix mat,Matrix submatrix){
   
    if( submatrix.rows * submatrix.cols > mat.rows * mat.cols){
        return nullptr;
    }
    
    cudaError_t error = cudaSuccess;
    double *devMatchingValueArrayPtr;
    int num_threads_rows = mat.rows;
    int num_threads_cols = mat.cols;
    int grid_block_rows = num_threads_rows / CUDA_THREADS_DIM + ((num_threads_rows % CUDA_THREADS_DIM) != 0); 
    int grid_block_cols = num_threads_cols / CUDA_THREADS_DIM + ((num_threads_cols % CUDA_THREADS_DIM) != 0); 
    int match_value_arr_rows_size = mat.rows - submatrix.rows + 1;
    int match_value_arr_cols_size = mat.cols - submatrix.cols + 1;
    int total_num_positions = match_value_arr_rows_size * match_value_arr_cols_size;


    // host to device

    error = cudaMalloc(&devMatchingValueArrayPtr, total_num_positions * sizeof(double));
    CUDA_error_check(error, "fail to allocate memory for matching value array pointer in GPU");

    dim3 dimGrid(grid_block_cols,grid_block_rows);
    dim3 dimBlock(CUDA_THREADS_DIM,CUDA_THREADS_DIM);

    CUDA_calculate_matching_array_kernel<<<dimGrid,dimBlock>>>(devMatchingValueArrayPtr,devMatrixPtr,devSubmatrixPtr,mat.rows,mat.cols,submatrix.rows,submatrix.cols);
    double* arr = CUDA_get_array_from_device(devMatchingValueArrayPtr,total_num_positions);
    error = cudaFree(devMatchingValueArrayPtr);
    CUDA_error_check(error, "failed to free matching value array pointer memory from GPU");
    return arr;
}


