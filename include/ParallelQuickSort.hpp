#pragma once

#include <vector>
#include <string>
#include "ThreadWrapper.hpp"

class ParallelQuickSort {
private:
    int max_threads;
    std::vector<int> array;
        
    struct ThreadData {
        ParallelQuickSort* sorter;
        int left;
        int right;
        ThreadData(ParallelQuickSort* s, int l, int r);
    };

    static void thread_function(ThreadData* data);
    
    int partition(int left, int right);
    void sequential_quicksort(int left, int right);
    void merge_sorted_parts();
public:
    ParallelQuickSort(int max_threads);
    ~ParallelQuickSort() = default;
    
    void sort(std::vector<int>& arr);
    void print_array() const;
    int get_max_threads() const { return max_threads; }

    static std::vector<int> generate_random_array(int size);
    static void parse_arguments(int argc, char* argv[], int& thread_count, int& array_size);
};