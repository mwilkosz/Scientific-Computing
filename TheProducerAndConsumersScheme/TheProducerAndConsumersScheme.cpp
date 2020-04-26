#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <array>
#include <algorithm>
#include <string>
#include <mutex>
#include <windows.h>
#include <atomic>
#include <chrono>


#define ARRSIZE 100000
std::mutex mtx;
static std::atomic<int> atomic_global_variable;

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
        std::cout << "Queue size is : " << std::queue< T >::size()<< std::endl;
        std::cout << "Queue was destructed" << std::endl;
    }

    void push(T &array )
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
    Producer(int n, Queue<std::array<int, ARRSIZE>> &head) : num_of_arrays(n), queue(head) {}
    ~Producer()
    {
        std::cout << "Producer has produced: " <<push << std::endl;
        std::cout << "Producer was destructed" << std::endl;
    }
    void MakeArrays()
    {   
        atomic_global_variable = num_of_arrays;

        while (push < num_of_arrays) 
        {
            if (queue.current_length == queue.length)
            {
                std::this_thread::yield();
            }
            else
            {
                for (int i = 0; i < arrsize; i++)
                {
                    arr[i] = rand() % 100;
                }
                std::unique_lock<std::mutex> ul(mtx);
                queue.push(arr);
                push++;
            }
        }
    }
 
private:
    Queue<std::array<int, ARRSIZE>>& queue;
    std::array<int, ARRSIZE> arr;
    const int num_of_arrays = 0;
    const int arrsize = ARRSIZE;
    int push = 0;
};

class Consumer
{
public:
    Consumer(Queue<std::array<int, ARRSIZE>> &head) : queue(head) {}
    ~Consumer()
    {
        std::cout << "Consumer was destructed" << std::endl;
    }
    void FetchArrays()
    {
        while (!state)
        {
            if (atomic_global_variable == 0)
            {
                state = true;
                std::lock_guard<std::mutex> lock(mtx);
                break;
            }

            std::lock_guard<std::mutex> ul(mtx);
            if (!queue.empty())
            {
                //std::cout << "thread working" << std::endl;
                sum = 0;
                checksum = 0;
                std::array<int, ARRSIZE> arr;
                arr = queue.front();
                std::sort(arr.begin(), arr.end());
                for (int i = 0; i < arrsize; i++)
                {
                    sum += arr[i];
                }
                checksum = sum / arrsize;
                std::cout << "Check sum is: " << checksum << std::endl;
                tables_sorted++;
                atomic_global_variable--;
                queue.pop();
            }
            else
            {
                std::this_thread::yield();
            }
        }
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Thread "<< std::this_thread::get_id() <<" sorted: "<< tables_sorted <<" tables"<< std::endl;
    }
    
private:
    Queue<std::array<int, ARRSIZE>> &queue;
    const int arrsize = ARRSIZE;
    int tables_sorted = 0;
    float checksum=0;
    float sum=0;
    bool state = false;
}; 

void SeveralThreads(int thread_vector_number,Consumer &cons)
{
    std::vector<std::thread> thread_vector;
    thread_vector.reserve(thread_vector_number);
    for (int i = 0; i < thread_vector_number; i++)
    {      
        thread_vector.push_back(std::thread(&Consumer::FetchArrays,cons));
    }
    for (auto &entry : thread_vector)
    {   
        entry.join();
    }
 }

struct Timer
{
    std::chrono::time_point<std::chrono::steady_clock>start, end;
    std::chrono::duration<float>duration;

    Timer()
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        std::cout <<"Execution time: "<< duration.count() <<'s'<< std::endl;
    }
};

int main(void)
{
    Timer time;
    std::cout << "Number of logical cores: " << std::thread::hardware_concurrency() << std::endl;
    Queue<std::array<int, ARRSIZE>> que(1000);
    Producer obj(5000, que);
    Consumer obj1(que);
    std::thread producer_thread(&Producer::MakeArrays, obj);
    SeveralThreads(10, obj1);
    producer_thread.join();

    return 0;
}

//Experiment
//int main(void)
//{
//    //Experiment
//    const int consumer_num[10] = { 2,4,6,8,10,16,20,40,50,100 };
//    const int runs = 5;
//    float times[10];
//    float time_sum = 0;
//
//    for (int i = 0; i < 10; i++)
//    {
//        time_sum = 0;
//        for (int j = 0; j < runs; j++)
//        {
//            std::chrono::time_point<std::chrono::steady_clock>start, end;
//            std::chrono::duration<float>duration;
//            start = std::chrono::high_resolution_clock::now();
//            Queue<std::array<int, ARRSIZE>> que(1000);
//            Producer obj(5000, que);
//            Consumer obj1(que);
//            std::thread producer_thread(&Producer::MakeArrays, obj);
//            SeveralThreads(consumer_num[i], obj1);
//            producer_thread.join();
//            end = std::chrono::high_resolution_clock::now();
//            duration = end - start;
//            time_sum = time_sum + duration.count();
//        }
//        times[i] = time_sum / runs;
//    }
//
//    for (int i = 0; i < 10; i++)
//    {
//        std::cout << times[i] << std::endl;
//    }
//
//    return 0;
//}