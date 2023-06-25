#include <omp.h>
#include "omp_functions.h"
#include "struct.h"
#include "config.h"
#include "cuda_functions.h"

double* omp_search_matrix_im_matrix(double* devMatrixPtr,Matrix mat, Matrix submatrix){
    double* devSubmatrixPtr;
    CUDA_matrix_to_GPU(submatrix,&devSubmatrixPtr);
    CUDA_check_matrix_in_GPU(submatrix,devSubmatrixPtr);
    double* arr = CUDA_get_matrix_matching_array_in_matrix(devMatrixPtr,devSubmatrixPtr, mat,submatrix);
    CUDA_free_matrix_from_GPU(&devSubmatrixPtr);
    return arr;

}

void om_search_matrix_positions(double** matching_values_arrays, double* devMatrixPtr,Matrix mat,Matrix* submatrices,int* num_submatrices){
    #pragma omp parallel num_threads(OMP_NUM_THREADS) shared(matching_values_arrays, devMatrixPtr,mat,submatrices)
    {
    #pragma omp for schedule(dynamic,1)
    for(int tid = 0; tid < *num_submatrices; tid++){
        int match_dim_rows = mat.rows - submatrices[tid].rows + 1;
        int match_dim_cols = mat.cols - submatrices[tid].cols + 1;         
        double* matching_value_array = omp_search_matrix_im_matrix(devMatrixPtr,mat,submatrices[tid]);
        matching_values_arrays[tid]  = matching_value_array;
    } 
    }        
}

void omp_matrix_num_positions(int* total_num_positions,double** matching_values_arrays,Matrix mat,  Matrix* submatrices, int* num_submatrices, double* matching_value){
    int total = 0;
    #pragma omp parallel num_threads(OMP_NUM_THREADS)  shared(mat, matching_values_arrays,total)
    {
        #pragma omp for schedule(dynamic)
        for(int tid = 0; tid < *num_submatrices; tid++){
            if(mat.rows * mat.cols  < submatrices[tid].rows * submatrices[tid].cols){
                continue;
            }            
            double* matching_arr = matching_values_arrays[tid];
            int match_dim_rows = mat.rows - submatrices[tid].rows + 1;
            int match_dim_cols = mat.cols - submatrices[tid].cols + 1; 
            for (int i = 0; i < match_dim_rows * match_dim_cols; i++){
                double match = matching_arr[i];
                if(match < *matching_value){
                    #pragma omp critical
                    {
                        total++;
                    }
                }
            }
        }
    }
    *total_num_positions = total;
}



void omp_create_matrix_positions(int* total_num_positions,Position* positions,double** matching_values_arrays,Matrix mat,  Matrix* submatrices, int* num_submatrices, double* matching_value){
    int pos_index = 0;
    #pragma omp parallel num_threads(OMP_NUM_THREADS)  shared(positions, mat, matching_values_arrays, pos_index)
    {
        #pragma omp for schedule(dynamic)
        for(int tid = 0; tid < *num_submatrices ; tid++){
            int num_pos_per_submatrix = 0;
            if(mat.rows * mat.cols < submatrices[tid].rows * submatrices[tid].cols){
                continue;
            }        
            double* matching_arr = matching_values_arrays[tid];
            int match_dim_rows = mat.rows - submatrices[tid].rows + 1;
            int match_dim_cols = mat.cols - submatrices[tid].cols + 1; 
            for (int i = 0; i < match_dim_rows * match_dim_cols; i++){
                double match = matching_arr[i];
                if(match < *matching_value){
                    Position p;
                    int x =  i % match_dim_rows;
                    int y =  i / match_dim_cols;
                    p.j = x;
                    p.i = y;
                    p.matrixID = mat.ID;
                    p.submatrixID = submatrices[tid].ID;
                    p.ID = num_pos_per_submatrix;
                    positions[pos_index] = p;
                    num_pos_per_submatrix++;
                    #pragma omp critical
                    {
                        pos_index++;
                    }

                }
            }
        }
        #pragma omp barrier
    }
}



