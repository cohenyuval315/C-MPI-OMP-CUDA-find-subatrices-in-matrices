# C-MPI-OMP-CUDA-find-subatrices-in-matrices
C MPI Omp CUDA

# The main flow of the program
- input file consist of all matrixes and submatrixes
- ouput file consist of all sub matrix mathcing positions in all matrixes with a maixmum value of matching positions
- the submatrix matching p ossition is calculated as the average differences of the entire object sub matix in the image matrix

- Using MPI master broad cast all objects sub matrices between N work processes
![Screenshot from 2023-06-29 17-29-39](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/0879cbe8-fa4b-4e82-85ad-d760eb576534)


- Master designed architecture is Centrelized Dynamic Load Balancing.
- each task is consist of 1 image matrix from input file
- Master queue up the images matrix and sends one image work to each free process , if the master recv a processed result he sends another job or terminate tag in case of no more work
![Screenshot from 2023-06-29 17-33-49](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/2ba0226c-b16a-40e9-bb03-b1a278259955)
  

- Worker recv a work , and split the load to (number of sub matrices) OMP threads 
![Screenshot from 2023-06-29 17-36-14](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/551670ce-3e09-45fe-887d-7e3007662929)


- Worker thread uses CUDA gpu to parallel 
![Screenshot from 2023-06-29 17-39-57](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/8dfa2830-3068-437c-8481-2bdbbe163ba4)



![Screenshot from 2023-06-29 17-40-37](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/26f934e6-3396-4173-b890-1ac5a8fcf648)

