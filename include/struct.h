#ifndef STRUCT_H
#define STRUCT_H


typedef struct PositionStruct {
    int ID;
    int matrixID;
    int submatrixID;
    int i;
    int j;
} Position;


typedef struct MatrixStruct {
    int ID;
    int rows; 
    int cols;
    double* data; 
} Matrix;



#endif 