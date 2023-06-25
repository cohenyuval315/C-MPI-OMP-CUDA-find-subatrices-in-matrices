#ifndef C_FUNC
#define C_FUNC

#include "struct.h"

Position* search_submatrices_in_matrix(int* total_num_positions,Matrix mat, Matrix* submatrices, int *num_submatrices, double* matching_value);

void pointer_null_check(void* ptr, const char* msg);
void input_check(int scan_val, const char* msg);
void read_input_file(const char* fileName, double* matching_value, Matrix** matrices, Matrix** submatrices, int* num_matrices, int* num_submatrices);
void create_output_file(const char *fileName,Position** positions,int* total_num_positions,int num_matrices);


Matrix* read_matrices(FILE* fp,const int* num_matrices);
void read_matrix(FILE* fp, Matrix* mat);
void free_matrices(Matrix* matrices, int num_matrices);


void print_double_array(double* arr, int size);
void print_hex(char* buffer, int size);
void print_positions(Position* positions , int *size);
void print_matrix(Matrix mat,bool printmat);
void print_matrices(Matrix *matrices, int num_matrices,bool printmat);

#endif