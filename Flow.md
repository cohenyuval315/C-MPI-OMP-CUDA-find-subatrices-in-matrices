
# The main flow of the program
- input file consist of all matrixes and submatrixes
- ouput file consist of all sub matrix mathcing positions in all matrixes with a maixmum value of matching positions
- the submatrix matching p ossition is calculated as the average differences of the entire object sub matix in the image matrix


# MPI 
### broadcasting the sub matrices data
- Using MPI master broad cast all objects sub matrices between N work processes
![Screenshot from 2023-06-29 17-29-39](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/0879cbe8-fa4b-4e82-85ad-d760eb576534)

## master and workers architecture - centrelized dynamic load balancing:
- each task is consist of 1 image matrix from input file
- Master queue up the images matrix and sends one image work to each free process at the start, if the master recv a processed result he sends another job or terminate tag in case of no more work
![Screenshot from 2023-06-29 17-33-49](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/2ba0226c-b16a-40e9-bb03-b1a278259955)
  
# OMP
- Worker recv a work , and split the load to OMP threads
- for each worker creating all possible positions of the object sub matric in the image matrix.
![Screenshot from 2023-06-29 20-00-28](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/86fc14e9-18c4-4c62-ba44-4c0f2289275d)


# CUDA
- Worker thread uses pre defined sizes CUDA blocks in gpu , the num blocks in the grid are dynamicly allocated by the size of the image , in order to fill it all without overriding the blocks in order to save resources.
- the cuda kernel spread the calculation in the blocks and calculated each position matrix to add up each difference to its fit position in the matching positions matrix  , the block does not correspond to the object sub matrix position in the image matrix ,trying to do that will raise the complexity of the algorithm by quite a lot.
![Screenshot from 2023-06-29 17-39-57](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/8dfa2830-3068-437c-8481-2bdbbe163ba4)


- all the positions is accumulated to one matrix of all possible positions values in way that each position represent the average difference of that object to the image ,relative to all possible positions of an object image in an image matrix , which is preparing for threshold checking later on in the program.
![Screenshot from 2023-06-29 17-40-37](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/26f934e6-3396-4173-b890-1ac5a8fcf648)



once the all jobs are done, the master using omp to search each object matching position array a valid matching value bellow the matching value threshold.
the master write the positions in output file (note: there is a maximun number of position per image  on the config.h file)
the master shut down the rest of the program.

