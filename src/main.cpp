#include "../include/ParallelQuickSort.hpp"
#include <iostream>
#include <chrono>
#include <algorithm>

double measure_std_sort_time(std::vector<int> arr) {
    auto start = std::chrono::high_resolution_clock::now();
    std::sort(arr.begin(), arr.end()); 
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

double measure_our_sequential_time(std::vector<int> arr) {
    ParallelQuickSort sorter(1); 
    auto start = std::chrono::high_resolution_clock::now();
    sorter.sort(arr);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main(int argc, char* argv[]) {
    int thread_count, array_size;
    ParallelQuickSort::parse_arguments(argc, argv, thread_count, array_size);
    
    std::cout << "=== Параллельная Быстрая Сортировка ===" << std::endl;
    std::cout << "Максимум потоков: " << thread_count << std::endl;
    std::cout << "Размер массива: " << array_size << std::endl;
    
    std::vector<int> arr = ParallelQuickSort::generate_random_array(array_size);
    std::vector<int> arr_copy = arr;
    std::vector<int> arr_copy2 = arr; 
    
    std::cout << "\n--- Последовательная сортировка (std::sort) ---" << std::endl;
    double seq_time_std = measure_std_sort_time(arr_copy);
    std::cout << "Время std::sort: " << seq_time_std << " мс" << std::endl;
    
    std::cout << "\n--- Последовательная сортировка (наша реализация) ---" << std::endl;
    double seq_time_our = measure_our_sequential_time(arr_copy2);
    std::cout << "Время нашей последовательной сортировки: " << seq_time_our << " мс" << std::endl;
    
    std::cout << "\n--- Параллельная сортировка ---" << std::endl;
    ParallelQuickSort sorter(thread_count);
    
    auto parallel_start = std::chrono::high_resolution_clock::now();
    sorter.sort(arr);
    auto parallel_end = std::chrono::high_resolution_clock::now();
    double parallel_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        parallel_end - parallel_start).count();
    
    bool is_sorted = std::is_sorted(arr.begin(), arr.end());
    std::cout << "Массив " << (is_sorted ? "отсортирован корректно" : "НЕ отсортирован") << std::endl;
    
    double speedup = (parallel_time > 0) ? seq_time_our / parallel_time : 0;
    double efficiency = (thread_count > 0) ? (speedup / thread_count) * 100 : 0;
    
    std::cout << "\n--- Результаты ---" << std::endl;
    std::cout << "Время нашей последовательной: " << seq_time_our << " мс" << std::endl;
    std::cout << "Время параллельной: " << parallel_time << " мс" << std::endl;
    std::cout << "Время std::sort: " << seq_time_std << " мс" << std::endl;
    std::cout << "Ускорение: " << speedup << "x" << std::endl;
    std::cout << "Эффективность: " << efficiency << "%" << std::endl;

    std::cout << "\n--- Проверка ---" << std::endl;
    sorter.print_array();
    
    if (speedup > 1.0) {
        std::cout << "✓ Параллельная версия быстрее!" << std::endl;
    } else if (speedup > 0) {
        std::cout << "⚠ Параллельная версия медленнее (накладные расходы)" << std::endl;
    }
    
    return 0;
}