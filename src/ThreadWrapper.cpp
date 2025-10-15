#include "ThreadWrapper.hpp"
#include <iostream>

Mutex::Mutex() {
#ifdef _WIN32
    InitializeCriticalSection(&cs);
#else
    pthread_mutex_init(&mutex, nullptr);
#endif
}

Mutex::~Mutex() {
#ifdef _WIN32
    DeleteCriticalSection(&cs);
#else
    pthread_mutex_destroy(&mutex);
#endif
}

void Mutex::lock() {
#ifdef _WIN32
    EnterCriticalSection(&cs);
#else
    pthread_mutex_lock(&mutex);
#endif
}

void Mutex::unlock() {
#ifdef _WIN32
    LeaveCriticalSection(&cs);
#else
    pthread_mutex_unlock(&mutex);
#endif
}

ConditionVariable::ConditionVariable() {
#ifdef _WIN32
    InitializeConditionVariable(&cv);
#else
    pthread_cond_init(&cond, nullptr);
#endif
}

ConditionVariable::~ConditionVariable() {
#ifndef _WIN32
    pthread_cond_destroy(&cond);
#endif
}

void ConditionVariable::wait(Mutex& mutex) {
#ifdef _WIN32
    SleepConditionVariableCS(&cv, mutex.get_critical_section(), INFINITE);
#else
    pthread_cond_wait(&cond, mutex.get_mutex());
#endif
}

void ConditionVariable::notify_one() {
#ifdef _WIN32
    WakeConditionVariable(&cv);
#else
    pthread_cond_signal(&cond);
#endif
}

void ConditionVariable::notify_all() {
#ifdef _WIN32
    WakeAllConditionVariable(&cv);
#else
    pthread_cond_broadcast(&cond);
#endif
}

#ifdef _WIN32
DWORD WINAPI Thread::threadStarter(void* arg) {}
#else
void* Thread::threadStarter(void* arg) {
#endif
    Thread* self = static_cast<Thread*>(arg);
    if (self && self->function) {
        self->function();
    }
#ifdef _WIN32
    return 0;
#else
    return nullptr;
#endif
}

Thread::Thread(std::function<void()> func) 
    : function(func), joined(false) {
#ifdef _WIN32
    thread = CreateThread(nullptr, 0, threadStarter, this, 0, &threadId);
    if (thread == nullptr) {
        std::cerr << "Ошибка создания потока" << std::endl;
    }
#else
    if (pthread_create(&thread, nullptr, threadStarter, this) != 0) {
        std::cerr << "Ошибка создания потока" << std::endl;
    }
#endif
}

Thread::~Thread() {
    if (joinable()) {
#ifdef _WIN32
        CloseHandle(thread);
#else
        pthread_detach(thread);
#endif
    }
}

void Thread::join() {
    if (!joined && joinable()) {
#ifdef _WIN32
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
#else
        pthread_join(thread, nullptr);
#endif
        joined = true;
    }
}

bool Thread::joinable() const {
#ifdef _WIN32
    return thread != nullptr;
#else
    return thread != 0;
#endif
}