#ifndef CUDA_FUNC
#define CUDA_FUNC

#include "struct.h"

extern double* CUDA_get_matrix_matching_array_in_matrix(double* devMatrixPtr,double *devSubmatrixPtr, Matrix mat,Matrix submatrix);
extern void CUDA_matrix_to_GPU(Matrix mat, double** devMatrixPtr);
extern void CUDA_free_matrix_from_GPU(double **devMatrixPtr);
void CUDA_check_matrix_in_GPU(Matrix mat, double* devMatrixPtr);
double*  CUDAgetMatrixFromDevice(double* arr , Matrix mat, double* devMatrixPtr);

#endif