#pragma once

#include <functional>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif

class Mutex {
private:
#ifdef _WIN32
    CRITICAL_SECTION cs;
#else
    pthread_mutex_t mutex;
#endif

public:
    Mutex();
    ~Mutex();
    void lock();
    void unlock();
    
#ifdef _WIN32
    CRITICAL_SECTION* get_critical_section() { return &cs; }
#else
    pthread_mutex_t* get_mutex() { return &mutex; }
#endif
};

class ConditionVariable {
private:
#ifdef _WIN32
    CONDITION_VARIABLE cv;
#else
    pthread_cond_t cond;
#endif

public:
    ConditionVariable();
    ~ConditionVariable();
    void wait(Mutex& mutex);
    void notify_one();
    void notify_all();
};

class Thread {
private:
#ifdef _WIN32
    HANDLE thread;
    DWORD threadId;
#else
    pthread_t thread;
#endif
    std::function<void()> function;
    bool joined;

    static
#ifdef _WIN32
    DWORD WINAPI 
#else
    void*
#endif
    threadStarter(void* arg);

public:
    Thread(std::function<void()> func);
    ~Thread();
    void join();
    bool joinable() const;
};