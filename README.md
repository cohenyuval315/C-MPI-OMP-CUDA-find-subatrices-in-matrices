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


- Worker thread uses pre defined CUDA blocks in gpu , the num blocks in the grid are dynamicly allocated by the size of the image , in order to fill it all without overrideing the blocks in order to save resources.
- all possible object positions in image matrix is creat ed
- the cuda kernel spread the calculation in the blocks and calculated each position matrix to add up each difference to its fit position in the matching positions matrix  , the block does not correspond to the object sub matrix position in the image matrix ,trying to do that will raise the complexity of the algorithm by quite a lot.
![Screenshot from 2023-06-29 17-39-57](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/8dfa2830-3068-437c-8481-2bdbbe163ba4)


- all the position is accumelated to one matrix of all possible positions values  of an object image in an image matrix , which is preparing  for threshold checking later on in the program.
![Screenshot from 2023-06-29 17-40-37](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/26f934e6-3396-4173-b890-1ac5a8fcf648)

the worker pass the object matching positions in image matrix to the master with result tag
once the all jobs are done, the master using omp to search each object matching position array a valid matching value bellow the matching threshold.
the master write the positions in output file (there is a maximun number of position per image  on the config.h file)
the master shut down the rest of the program.
