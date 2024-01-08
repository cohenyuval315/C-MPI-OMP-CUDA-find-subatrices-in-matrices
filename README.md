# Sub-Matrix Object Detection with Parallel Processing

Detecting object positions in image matrices is accomplished by utilizing a threshold average value - </br>
The process involves implementing MPI for coordinating master and worker nodes across distributed processes, supporting multiprocessing. Additionally, threading is employed through OpenMP to enhance parallel execution. The system is further optimized with GPU support using CUDA for efficient object detection within the matrices.


###  for the flow of the program please click here:
[Flow of the program](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/blob/main/Flow.md)

# requirments:
- Unix
- gpu that supports cuda , or cuda emulater(i does not recommend you to go though that, it does not worth it)
- mpi libs
- omp libs
- cuda libs
  
# Setup Master
1. MPI libs:
  ```bash
  sudo apt install mpich
  sudo apt install libopenmpi-dev
  ```
  ```bash
  sudo apt install libopenmpi-dev
  ```
  
2. OMP libs:   
  ```bash
  sudo apt install libomp-dev
  ```
3. CUDA libraries: [Nividia Website](https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html)


# Setup With Workers
Avoid using WSL/2 or Docker when using multiple nodes, as MPI is not compatible with these platforms.

## creating hosts file:
Define hosts in hosts.txt for running on multiple nodes. Retrieve host names using the command:
```
  hostname
```
Ensure new lines in the hosts file are in LF (/n) format, not CRLF (/r/n). Avoid adding extra spaces.

## connection setup:

## SSH:
Passwordless SSH protocol is essential for MPI.
To facilitate MPI commands between computers, install and configure OpenSSH Server. Follow these steps:

Step 1: 
- Install OpenSSH Server:
```
sudo apt-get install openssh-server
```
- Start SSH Server Service
```
  sudo service ssh start
```

Step 2: Generate SSH Key Pairs and Exchange Public Keys:
On each computer involved in MPI communication, generate SSH key pairs and exchange public keys between nodes. The keys will be associated with your current user.
```
  ssh-keygen -t rsa
```
Enter the desired filename and password when prompted.
Take note of the <strong>filepath</strong> and proceed to the next steps.

Ensure that the following files are present in the .ssh folder:
<strong>filename</strong>.pub: Public key to be shared with other nodes.
<strong>filename</strong>: Private key (do not expose).

Adjust Permissions if Necessary
- 700 - read, write, excute for owner only.

<strong>filepath</strong> - unix default is /root/.ssh  (Interesting fact: In WSL2 it might appear as /root/.ssh ,but its actually in mnt/'disk'/Users/'user'/.ssh/)
```
 chmod 700 <filepath>
```

Step 4: Copy Public Key to Other Nodes
```
  ssh-copy-id -i ~/.ssh/<filename>.pub other_node_username@other_node_ip
```
Alternatively use default:
```
  ssh-copy-id other_node_username@other_node_ip
```

## Worker Envrioment variables
Ensure the system environment has LD_LIBRARY_PATH and nessessary dependancies.
```
export PATH=$PATH:/path/to/mpi/bin
export PATH=$PATH:/path/to/openmpi/bin
export PATH=$PATH:/path/to/cuda/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<mpi_lib_directory>:<openmp_lib_directory>:<cuda_lib_directory> # one liner
```


## firewall:
Ensure the firewall allows SSH traffic on the desired port (default is 22).
```
sudo ufw allow 22
sudo ufw enable
sudo ufw status  
```

# Program
## input
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
  
## output:
[example](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/blob/main/output.txt)

## Init
- config.h - configuration file - maximum sub matrices to find in matrix and etc.
    
- make clean , make   
![WhatsApp Image 2023-06-29 at 19 44 35](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/b3c6330c-5e9e-44da-a7e7-c72bc089723d)
    
- make run (with 2 processes) or  mpiexec -np x ./bin/program (x = num of processes)
![WhatsApp Image 2023-06-29 at 19 45 04](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/59bf4fa5-83f4-4e9f-9630-70c60aff4427)
  

![WhatsApp Image 2023-06-29 at 19 46 03](https://github.com/cohenyuval315/C-MPI-OMP-CUDA-find-subatrices-in-matrices/assets/61754002/e0ab3d4c-e85a-46be-ac69-a00a961391d1)
