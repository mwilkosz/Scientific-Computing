#include "mpi.h"
#include <math.h>
#include <iostream>
#include <chrono>

int is_prime(int nr) 
{
    if (nr < 2)
        return 0;
    for (int i = 2; i <= sqrt(int(nr)); i++)
        if ((nr % i) == 0)
                return 0;
    return 1;
}

void prime_numbers(int argc, char* argv[],int number)
{
    MPI_Init(&argc, &argv);

    int size, rank, start, step, tmp_result, result;
    float start_time, end_time;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    start = (rank * 2) + 1;  
    step = size * 2;          
    tmp_result = 0;

    start_time = MPI_Wtime();
    if (rank == 0) 
    {
        for (int i = start; i <= number; i = i + step) 
        {
            if (is_prime(i)) 
            {
                tmp_result++;
            }
        }
        MPI_Reduce(&tmp_result, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        result++;
        std::cout << "In given number there is: " << result << " prime numbers" << std::endl;
        end_time = MPI_Wtime();
        std::cout << "Execution time: " << end_time-start_time << std::endl;;
    }

    if (rank > 0) 
    {
        for (int i = start ; i <= number; i = i + step) 
        {
            if (is_prime(i)) 
            {
                tmp_result++;
            }
        }
        MPI_Reduce(&tmp_result, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
}


int main(int argc, char* argv[])
{   
    int number = 100000;
    prime_numbers(argc, argv, number);
    return 0;
}
