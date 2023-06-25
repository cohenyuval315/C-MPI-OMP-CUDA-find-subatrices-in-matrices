#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <stdbool.h> 
#include <sys/stat.h> 
#include "c_functions.h"
#include "struct.h" 
#include "config.h"
#include "cuda_functions.h"
#include "omp_functions.h"
#include "ctype.h"

Position* search_submatrices_in_matrix(int* total_num_positions, Matrix mat, Matrix* submatrices, int *num_submatrices, double* matching_value){
    double* devMatrixPtr;
    CUDA_matrix_to_GPU(mat, &devMatrixPtr);
    CUDA_check_matrix_in_GPU(mat,devMatrixPtr);
    double** matching_values_arrays = (double**)malloc((*num_submatrices) * sizeof(double*));
    pointer_null_check(matching_values_arrays,"fail to allocate memory for matching values arrays");
    om_search_matrix_positions(matching_values_arrays,devMatrixPtr,mat,submatrices,num_submatrices);
    omp_matrix_num_positions(total_num_positions,matching_values_arrays,mat,submatrices,num_submatrices,matching_value);
    Position* positions = (Position*)malloc((*total_num_positions) * sizeof(Position));
    pointer_null_check(positions,"fail to allocate memory for positions of matrix");
    omp_create_matrix_positions(total_num_positions,positions,matching_values_arrays,mat,submatrices,num_submatrices,matching_value);
    CUDA_free_matrix_from_GPU(&devMatrixPtr);
    return positions;
}

Matrix* read_matrices(FILE *fp, const int *num_matrices){
    int n = *num_matrices;
    Matrix* matrices = (Matrix*) malloc(n * sizeof(Matrix));
    pointer_null_check(matrices,"matrices memory allocation failed");
    for(int i= 0; i < n; i ++){
        read_matrix(fp, &matrices[i]);
    }

    return matrices;
}


void write_matrix(FILE* fp ,int id ,int matrix_row, int matrix_col, double* data){
    fprintf(fp, "%d\n", id);
    fprintf(fp, "%d\n", matrix_row);
    fprintf(fp, "%d\n", matrix_col);
    for (int i = 0; i < matrix_row; i++) {
      for (int j = 0; j < matrix_col; j++) {
        fprintf(fp, "%lf ", data[i * matrix_row + matrix_col]);
      }
      fprintf(fp, "\n");
    }
}

void write_matrices(FILE* fp ,int num_matrices,  Matrix* matrices){
    fprintf(fp, "%d\n",num_matrices);
    for(int i = 0; i < num_matrices; i++){
        write_matrix(fp, matrices[i].ID, matrices[i].rows, matrices[i].cols, matrices[i].data);
    }
}


Matrix generate_random_matrix(int id,  int min, int max ,int min_matrix_row, int max_matrix_row ,int min_matrix_col,int max_matrix_col){
    Matrix random_mat;
    random_mat.ID = id;
    int matrix_rows = min_matrix_row + rand() % (max_matrix_row - min_matrix_row + 1);
    int matrix_cols = min_matrix_col + rand() % (max_matrix_col - min_matrix_col + 1);
    random_mat.rows = matrix_rows;
    random_mat.cols = matrix_cols;
    random_mat.data = (double*)malloc(matrix_rows * matrix_cols * sizeof(double));
    pointer_null_check(random_mat.data,"fail to allocate memory for random matrix");
    for (int i = 0; i < matrix_rows; i++) {
      for (int j = 0; j < matrix_cols; j++) {
        double num = min +  (max - min + 1) * (double)rand() / RAND_MAX;
        random_mat.data[i * matrix_rows + j] = num;
      }
    }    
    return random_mat;
}

Matrix* generate_random_matrices(int num_matrices , int min, int max ,int min_matrix_row, int max_matrix_row ,int min_matrix_col,int max_matrix_col){
    Matrix* mats = (Matrix*)malloc(num_matrices * sizeof(Matrix));
    pointer_null_check(mats,"fail to allocate memory for random matrices");
    for (int i = 0; i < num_matrices; i++){
        mats[i] = generate_random_matrix(i + 1,min,max,min_matrix_row,max_matrix_row,min_matrix_col,max_matrix_col);
    }
    return mats;
}

void create_random_input_file(int* num_matrices ,int* num_submatrices,int min, int max,double matching_value,int min_num_matrices, int max_num_matrices,int min_matrix_row, int max_matrix_row ,int min_matrix_col,int max_matrix_col,int min_num_submatrices,int max_num_submatrices ,int min_submatrix_row , int max_submatrix_row,int min_submatrix_col,int max_submatrix_col) {
    FILE *file = fopen(RANDOM_INPUT_FILE, "w");
    pointer_null_check(file,"Error opening file!");
    srand(time(NULL));
    fprintf(file, "%lf\n",matching_value);
    *num_matrices = min_num_matrices + rand() % (max_num_matrices - min_num_matrices + 1);
    *num_submatrices = min_num_submatrices + rand() % (max_num_submatrices - min_num_submatrices + 1);
    Matrix* matrices = generate_random_matrices(*num_matrices,min,max,min_matrix_row,max_matrix_row,min_matrix_col,max_matrix_col);
    write_matrices(file,*num_matrices,matrices);
    Matrix* submatrices = generate_random_matrices(*num_submatrices,min,max,min_submatrix_row,max_submatrix_row,min_submatrix_col,max_submatrix_col);
    write_matrices(file,*num_submatrices,submatrices);
    fclose(file);
}

bool check_input(double* number, int min_threshold,int max_threshold){
    if(max_threshold < min_threshold){
        return false;
    }
    if(*number < min_threshold){
        return false;
    }
    if(*number > max_threshold){
        return false;
    }
    return true;
}

void read_integer(int* number,int min_threshold,int max_threshold,const char* msg){
    printf("%s | current: %d\nplease write new value: ",msg, *number);
    int temp;
    double temp2;
    if(scanf("%d", &temp) == 1){
        temp2 = (double)temp;
        if(check_input(&temp2,min_threshold,max_threshold)){
            *number = temp;
        }
        return;
    }else{
        printf("bad input\n");
    }

    
    
}

void read_double(double* number, int min_threshold,int max_threshold,const char* msg){
    printf("%s | current: %lf \nplease write new value: ",msg, *number);
    double temp;
    if(scanf("%lf", &temp) == 1){
        if(check_input(&temp,min_threshold,max_threshold)){
            *number = temp;
        }
        return;
    }else{
        printf("bad input\n");
    }

    
    
}

void free_matrices(Matrix* matrices, int num_matrices) {
    for (int i = 0; i < num_matrices; i++) {
        free(matrices[i].data);
    }
    free( matrices );
}

void read_input_file(const char* fileName, double* matching_value, Matrix** matrices, Matrix** submatrices, int* num_matrices, int* num_submatrices) {
    FILE* fp = fopen(fileName, "r");
    pointer_null_check(fp,"Error opening file");
    input_check(fscanf(fp, "%lf", matching_value),"bda matching value input");
    input_check(fscanf(fp, "%d", num_matrices),"bad num of matrices to search on"); 
    *matrices = read_matrices(fp,num_matrices);
    input_check(fscanf(fp, "%d", num_submatrices),"bad num of matrices to search with");
    *submatrices = read_matrices(fp,num_submatrices);
    fclose(fp);
    printf("-matching value: %f\n-number of pictures: %d\n-number of objects: %d\n", *matching_value,*num_matrices,*num_submatrices);    
}

void create_output_file(const char *fileName,Position** positions,int* total_num_positions, int num_matrices){
    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        printf("Error creating file!\n");
        exit(0);
    }    
    int found;
    for(int n = 0; n < num_matrices; n++){
        Position* p = positions[n];
        int num_positions = total_num_positions[n];
        found = 0;
        char result_line[100] = "Matrix ";
        char buffer[100];
        
        if(num_positions == 0){
            fprintf(file,"Matrix %d: No Submatrices were found;\n", n + 1);
        }else{
            sprintf(buffer, "%d: found Submatrices: \n", n + 1);
            strcat(result_line, buffer);
            for(int i =0; i < num_positions; i++){
                if(found == MAX_SUBMATRICES_PER_MATRIX){
                    break;
                }                
                sprintf(buffer,"-SubmatrixID:%d Position (%d,%d);\n",p[i].submatrixID,p[i].i,p[i].j);
                strcat(result_line,buffer);                
                found++;
            }
            fprintf(file,"%s\n",result_line);
        }
    }
    fclose(file);
}

void read_matrix(FILE* fp, Matrix* mat){
    double* data;
    int id;
    int rows;
    int cols;
    int size;
    double element;
    input_check(fscanf(fp, "%d", &id),"fail to read ID of matrix");
    input_check(fscanf(fp, "%d", &rows),"fail to read rows of matrix");
    input_check(fscanf(fp, "%d", &cols),"fail to read cols of matrix");
    size = rows * cols;
    data = (double*)malloc(size * sizeof(double));
    pointer_null_check(data,"fail to allocate memory for matrix");
    for(int i =0; i < size; i++){
        input_check(fscanf(fp, "%lf", &element),"input is not number");
        data[i] = element;
    }
    mat->ID = id;
    mat->rows = rows;
    mat->cols = cols;
    mat->data = data;
}

void input_check(int scan_val, const char* msg){
    if(scan_val != 1){
        printf("%s\n",msg);
        exit(0);
    }
}

void pointer_null_check(void* ptr, const char* msg){
    if(ptr == NULL){
        printf("%s\n", msg);     
        exit(0);
    }

}

void print_double_array(double* arr, int size){
    for(int i = 0; i < size; i++){
        printf("%.2f ",arr[i]);
    }
    printf("\n");
}

void print_hex(char* buffer, int size)
{
    for (int i = 0; i < size; i++)
        printf("%02hhX ", buffer[i]);
    printf("\n\n");
}

void print_positions(Position* positions , int *size){
    for(int i = 0; i < *size; i++){
        printf("Position :%d objectID:%d ,pictureID: %d ,I:%d, J:%d\n",positions[i].ID,positions[i].submatrixID,positions[i].matrixID,positions[i].i,positions[i].j);
    }
}

void print_matrix(Matrix mat,bool print_mat) {
   printf("\n--------\nMatrix ID = %d\n", mat.ID);
   printf("Matrix Dimentions = %dx%d\n", mat.rows,mat.cols);
   if(print_mat == true){
      printf("Matrix data:\n");
      for (int i = 0; i < mat.cols; i++) {
         for (int j = 0; j < mat.rows; j++) {
               printf("%lf ", mat.data[i * mat.cols + j]);
         }
         printf("\n");
      }
   }
   printf("---------\n");
}

void print_matrices(Matrix *matrices, int num_matrices,bool print_mat) {
    for (int i = 0; i < num_matrices; i++) {
        print_matrix(matrices[i],print_mat);
    }
}

