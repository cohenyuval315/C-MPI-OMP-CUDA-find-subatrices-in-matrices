#ifndef OMP_FUNC
#define OMP_FUNC
#include "struct.h"

double* omp_search_matrix_im_matrix(double* devMatrixPtr,Matrix mat, Matrix submatrix);
void om_search_matrix_positions(double** matching_values_arrays, double* devMatrixPtr,Matrix mat, Matrix* submatrices,int* num_submatrices);
void omp_matrix_num_positions(int* total_num_positions,double** matching_values_arrays,Matrix mat,  Matrix* submatrices, int* num_submatrices, double* matching_value);
void omp_create_matrix_positions(int* total_num_positions,Position* positions,double** matching_values_arrays,Matrix mat,  Matrix* submatrices, int* num_submatrices, double* matching_value);


#endif