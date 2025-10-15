#include "ParallelQuickSort.hpp"
#include <iostream>
#include <ctime>
#include <cstring>
#include <algorithm>
#include <random>
#include <vector>
#include <climits>

ParallelQuickSort::ThreadData::ThreadData(ParallelQuickSort* s, int l, int r) 
    : sorter(s), left(l), right(r) {}

ParallelQuickSort::ParallelQuickSort(int max_threads)
    : max_threads(max_threads) {}

void ParallelQuickSort::thread_function(ThreadData* data) {
    data->sorter->sequential_quicksort(data->left, data->right);
    delete data;
}

int ParallelQuickSort::partition(int left, int right) {
    if (left >= right) return left;
    int pivot = array[right];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
        if (array[j] <= pivot) {
            i++;
            std::swap(array[i], array[j]);
        }
    }
    std::swap(array[i + 1], array[right]);
    return i + 1;
}

void ParallelQuickSort::sequential_quicksort(int left, int right) {
    if (left >= right) return;

    int pivot_index = partition(left, right);
    
    if (pivot_index < left || pivot_index > right) return;
    
    sequential_quicksort(left, pivot_index - 1);
    sequential_quicksort(pivot_index + 1, right);
}

void ParallelQuickSort::merge_sorted_parts() {
    if (max_threads <= 1) return;
    
    std::vector<int> temp(array.size());
    int part_size = array.size() / max_threads;
    
    std::vector<std::vector<int>> parts;
    for (int i = 0; i < max_threads; i++) {
        int start = i * part_size;
        int end = (i == max_threads - 1) ? array.size() - 1 : start + part_size - 1;
        parts.emplace_back(array.begin() + start, array.begin() + end + 1);
    }
    
    std::vector<size_t> indices(max_threads, 0);
    
    for (size_t pos = 0; pos < array.size(); pos++) {
        int min_value = INT_MAX;
        int min_part = -1;
        
        for (int i = 0; i < max_threads; i++) {
            if (indices[i] < parts[i].size() && parts[i][indices[i]] < min_value) {
                min_value = parts[i][indices[i]];
                min_part = i;
            }
        }
        
        if (min_part != -1) {
            temp[pos] = min_value;
            indices[min_part]++;
        }
    }
    
    array = temp;
}

void ParallelQuickSort::sort(std::vector<int>& arr) {
    array = arr;
    if (array.size() <= 1) return;

    std::cout << "Начало сортировки с " << max_threads << " потоками" << std::endl;
    std::cout << "Размер массива: " << array.size() << std::endl;

    std::cout << "Исходный массив (первые 10 элементов): ";
    for (size_t i = 0; i < std::min(array.size(), size_t(10)); i++) {
        std::cout << array[i] << " ";
    }
    if (array.size() > 10) std::cout << "...";
    std::cout << std::endl;

    if (max_threads <= 1) {
        sequential_quicksort(0, array.size() - 1);
    } else {
        std::vector<Thread> threads;
        int part_size = array.size() / max_threads;
        
        std::cout << "Разделение на " << max_threads << " частей по " << part_size << " элементов" << std::endl;
        
        for (int i = 0; i < max_threads; i++) {
            int start = i * part_size;
            int end = (i == max_threads - 1) ? array.size() - 1 : start + part_size - 1;
            
            std::cout << "Поток " << i << ": [" << start << "-" << end << "]" << std::endl;
            
            ThreadData* data = new ThreadData(this, start, end);
            threads.emplace_back([data]() {
                ParallelQuickSort::thread_function(data);
            });
        }

        std::cout << "Ожидание завершения сортировки..." << std::endl;
        for (auto& thread : threads) {
            thread.join();
        }
        std::cout << "Все части отсортированы" << std::endl;

        std::cout << "Слияние отсортированных частей..." << std::endl;
        merge_sorted_parts();
        std::cout << "Слияние завершено" << std::endl;
    }

    arr = array;
}

void ParallelQuickSort::print_array() const {
    std::cout << "Отсортированный массив (первые 10 элементов): ";
    for (size_t i = 0; i < std::min(array.size(), size_t(10)); i++) {
        std::cout << array[i] << " ";
    }
    if (array.size() > 10) std::cout << "...";
    std::cout << std::endl;
}

std::vector<int> ParallelQuickSort::generate_random_array(int size) {
    std::vector<int> arr(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 10000);
    
    for (int i = 0; i < size; i++) {
        arr[i] = dis(gen);
    }
    
    return arr;
}

void ParallelQuickSort::parse_arguments(int argc, char* argv[], int& thread_count, int& array_size) {
    thread_count = 4;
    array_size = 10000;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--threads") == 0 && i + 1 < argc) {
            thread_count = std::atoi(argv[++i]);
        } else if (strcmp(argv[i], "--size") == 0 && i + 1 < argc) {
            array_size = std::atoi(argv[++i]);
        }
    }
    
    if (thread_count <= 0) thread_count = 1;
    if (array_size <= 0) array_size = 1000;
}