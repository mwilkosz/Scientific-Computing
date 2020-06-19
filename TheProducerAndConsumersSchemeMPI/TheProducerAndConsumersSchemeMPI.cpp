#include <iostream>
#include <mpi.h>
#include <math.h>
#include <thread>
#include <vector>
#include <queue>
#include <array>


#define ARRSIZE 100000

template< typename T >

class Queue : public std::queue< T >
{
public:
    Queue(int size) : length(size)
    {
        std::cout << "Queue size is : " << std::queue< T >::size() << std::endl;
    }
    ~Queue()
    {
        std::cout << "Queue size is : " << std::queue< T >::size() << std::endl;
        std::cout << "Queue was destructed" << std::endl;
    }

    void push(T& array)
    {
        if (current_length == length)
        {
            std::cout << "Queue is full!" << std::endl;
        }
        else
        {
            std::queue< T >::push(array);
            current_length++;
        }
    }

    void pop()
    {
        if (!std::queue< T >::empty())
        {
            std::queue< T >::pop();
            --current_length;
        }
    }

    const int length;
    int current_length = 0;
};

class Producer
{
public:
    Producer(int n, Queue<std::array<int, ARRSIZE>>& head) : num_of_arrays(n), queue(head) 
    { 
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    }
    ~Producer()
    {
        std::cout << "Producer has produced: " << push << std::endl;
        std::cout << "Producer was destructed" << std::endl;
    }

    void MakeArrays()
    {
        while (push < num_of_arrays)
        {
            MPI_Status status;
            MPI_Recv(&flag, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Send(&finish, 1, MPI_C_BOOL, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
            for (int i = 0; i < arrsize; i++)
            {
                arr[i] = rand() % 100;
            }
            queue.push(arr);
            push++;
            MPI_Send(&queue.front(), ARRSIZE, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
            queue.pop(); 
        }
        while (true)
        {
            finish = 1;
            MPI_Status status;
            MPI_Recv(&flag, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Send(&finish, 1, MPI_C_BOOL, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
            send++;
            if (send == size - 1)
            {
                break;
            }
        }
    }

private:
    Queue<std::array<int, ARRSIZE>>& queue;
    std::array<int, ARRSIZE> arr;
    const int num_of_arrays = 0;
    const int arrsize = ARRSIZE;
    int finish = 0;
    int push = 0;
    int flag = 0;
    int send = 0;
    int size = 0;
};

class Consumer
{
public:
    Consumer()
    {
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    }
    ~Consumer()
    {
        std::cout << "Consumer was destructed" << std::endl;
    }
    void FetchArrays()
    {
        while (true)
        {
            MPI_Send(&flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&finish, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (finish == 1)
            {
                break;
            }
            std::array<int, ARRSIZE> queuefront;
            MPI_Recv(&queuefront, ARRSIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum = 0;
            checksum = 0;
            std::sort(queuefront.begin(), queuefront.end());
            for (int i = 0; i < arrsize; i++)
            {
                sum += queuefront[i];
            }
            checksum = sum / arrsize;
            std::cout << "Check sum is: " << checksum << std::endl;
            tables_sorted++;
        }
        std::cout << "Consumer: " << rank << " sorted: " << tables_sorted << std::endl;
    }

private:
    const int arrsize = ARRSIZE;
    int tables_sorted = 0;
    float checksum = 0;
    float sum = 0;
    int flag = 0;
    int finish = 0;
    int rank;
};

void mpi_producer_consumer(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    int size, rank;
    float start_time, end_time;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    start_time = MPI_Wtime();
    if (rank == 0)
    {
        Queue<std::array<int, ARRSIZE>> que(1000);
        Producer obj(5000,que);
        obj.MakeArrays();
        end_time = MPI_Wtime();
        std::cout << "Execution time: " << end_time - start_time << std::endl;;
    }
    if (rank > 0)
    {
        Consumer obj1;
        obj1.FetchArrays();
    }

    MPI_Finalize();
}

int main(int argc, char* argv[])
{
    mpi_producer_consumer(argc, argv);
    return  0;
}
