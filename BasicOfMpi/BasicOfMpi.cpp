#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <iostream>
#include <Windows.h>

//SUB-TASK 1
void point_to_point(int argc, char* argv[])
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    int out_number = 1;
    int in_number;

    if (rank == 0)
    {
        MPI_Send(&out_number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(&in_number, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process: " << rank << " send: " << out_number << " and receive: "<< in_number << std::endl;
    }
    else if(rank == 1)
    {
        MPI_Recv(&in_number, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&out_number, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        std::cout << "Process: " << rank << " send: " << out_number << " and receive: " << in_number << std::endl;
    }
}

//SUB-TASK 2
void non_blocking(int argc, char* argv[])
{
    int rank;
    int size; 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Request request;
    MPI_Status  status;
  
    int request_finished;
    int number = 1;

    if (rank == 0) 
    {
        for (int i = 1; i < size; i++) 
        {
            number = number * i;
            MPI_Isend(&number, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, &status);
            MPI_Test(&request, &request_finished, &status);
            std::cout << "Process: " << rank << " send: " << number << std::endl;
        }
    }
    else 
    {
        MPI_Irecv(&number, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, &status);
        std::cout << "Process: " << rank << " receive: " << number << std::endl;
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    point_to_point(argc, argv);
    non_blocking(argc, argv);
    MPI_Finalize();
    return 0;
}
