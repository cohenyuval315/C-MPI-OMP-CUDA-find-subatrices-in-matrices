# C-MPI-OMP-CUDA-find-subatrices-in-matrices
C MPI Omp CUDA
###  for the flow of the program please click here:
[Flow of the program](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/blob/main/Flow.md)

# requirments:
- Unix
- gpu that supports cuda , or cuda emulater(i does not recommend you to go though that, it does not worth it)
- mpi libs
- omp libs
- cuda libs
  
# Setup
1. MPI libs:
  ```bash
  sudo apt install mpich
  ```
  ```bash
  sudo apt install libopenmpi-dev
  ```
  
2. OMP libs:   
  ```bash
  sudo apt install libomp-dev
  ```
3. CUDA libs: [Nividia Website](https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html)



# input
The input file should follow this format, where each line represents a row in the file:  

1.Matching value: This is a double value that represents the threshold for matching positions.  
2.Number of matrices: This is an integer indicating the total number of matrices in the file.  

For each matrix:

- Matrix ID: This is an integer representing the ID of the matrix.
- Number of matrix rows: This is an integer indicating the number of rows in the matrix.
- Number of matrix columns: This is an integer indicating the number of columns in the matrix.
- Matrix data: This line contains the matrix data itself, represented as a series of double values.
   
3. Number of sub-matrices: This is an integer indicating the total number of sub-matrices in the file.

For each sub-matrix:

- Sub-matrix ID: This is an integer representing the ID of the sub-matrix.
- Number of sub-matrix rows: This is an integer indicating the number of rows in the sub-matrix.
- Number of sub-matrix columns: This is an integer indicating the number of columns in the sub-matrix.
- Sub-matrix data: This line contains the sub-matrix data itself, represented as a series of double values.
  
You can repeat the above steps for each matrix and sub-matrix in your input file, adding the corresponding data for each one.
  
Please note that the format assumes that the matrix and sub-matrix data are represented as a series of double values. 
  
# output:
[example](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/blob/main/output.txt)

# Init
- config.h - configuration file - maximum sub matrices to find in matrix and etc.
    
- make clean , make   
![WhatsApp Image 2023-06-29 at 19 44 35](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/b3c6330c-5e9e-44da-a7e7-c72bc089723d)
    
- make run (with 2 processes) or  mpiexec -np x ./bin/program (x = num of processes)
![WhatsApp Image 2023-06-29 at 19 45 04](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/59bf4fa5-83f4-4e9f-9630-70c60aff4427)
  

![WhatsApp Image 2023-06-29 at 19 46 03](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/e0ab3d4c-e85a-46be-ac69-a00a961391d1)
