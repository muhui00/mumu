/*****************************************
 * Copyright (C) 2022 * Ltd. All rights reserved.
 *
 * File name   : thread.h
 * Author      : muhui
 * Created date: 2022-11-11 14:27:46
 * Description : 线程模块
 *
 *******************************************/

//#pragma once
#ifndef __THREAD_H__
#define __THREAD_H__

#include <thread>
#include <pthread.h>
#include <memory>
#include <functional>
#include <semaphore.h>
#include <atomic>
#include <iostream>
namespace muhui
{
//信号量类
class Semaphore
{
public:
    /**
     * @brief 构造函数
     * @param[in] count 信号量值的大小
     */
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    //获取信号量
    void wait();
    //释放信号量
    void notify();
private:
    //禁止默认构造
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;
private:
    sem_t m_semaphore;
};

//线程类
class Thread
{
public:
    //智能指针对象
    typedef std::shared_ptr<Thread> ptr;

    //构造函数
    Thread(std::function<void()> cb, const std::string& name);

    //析构
    ~Thread();

    //获取线程id
    pid_t getId() const { return m_id; }

    //获取线程名称
    const std::string getName() const { return m_name; }

    //等待线程执行完成
    void join();

    //获取当前按线程指针
    static Thread* GetThis();

    //获取当前线程名称
    static const std::string& GetName();

    //设置当前线程名称
    static void setName(const std::string& name);

    //线程执行函数
    static void* run(void* args);
private:
    //禁止默认构造
    Thread(const Thread&) = delete; //左值
    Thread(const Thread&&) = delete; //右值
    Thread& operator=(const Thread&) = delete;

private:
    pid_t m_id = -1; //线程id 全局唯一
    pthread_t m_thread = 0; //线程结构
    std::function<void()> m_cb; //线程执行函数
    std::string m_name; //线程名

    Semaphore m_semaphore; //信号量
};
//局部锁的模板实现
template<class T>
class ScopedLockTmpl
{
public:
    ScopedLockTmpl(T& mutex)
        : m_mutex(mutex)
    {
        //初始化时加锁
        m_mutex.lock();
        m_locked = true;
    }
    ~ScopedLockTmpl() {
        //析构时解锁
        m_mutex.unlock();
        m_locked = false;
    }

    void lock() {
        //防止死锁
        if(!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    //互斥量
    T& m_mutex;
    //是否已经上锁
    bool m_locked;
};
class NullMutex
{
public:
    typedef ScopedLockTmpl<NullMutex> Lock;
    NullMutex() {}
    ~NullMutex() {}
    void lock() {}
    void unlock() {}
};
class Mutex
{
public:
    typedef ScopedLockTmpl<Mutex> Lock;
    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }
    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }
    
    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};
//局部读锁的模板实现
template<class T>
class ReadScopedLockTmpl
{
public:
    ReadScopedLockTmpl(T& mutex)
        : m_mutex(mutex)
    {
        //初始化时加锁
        m_mutex.rdlock();
        m_locked = true;
    }
    ~ReadScopedLockTmpl() {
        //析构时解锁
        m_mutex.unlock();
        m_locked = false;
    }

    void rdlock() {
        //防止死锁
        if(!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    //互斥量
    T& m_mutex;
    //是否已经上锁
    bool m_locked;
};

//局部锁的模板实现
template<class T>
class WriteScopedLockTmpl
{
public:
    WriteScopedLockTmpl(T& mutex)
        : m_mutex(mutex)
    {
        //初始化时加锁
        m_mutex.wrlock();
        m_locked = true;
    }
    ~WriteScopedLockTmpl() {
        //析构时解锁
        m_mutex.unlock();
        m_locked = false;
    }

    void wrlock() {
        //防止死锁
        if(!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock() {
        if(m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    //互斥量
    T& m_mutex;
    //是否已经上锁
    bool m_locked;
};

class NullRWMutex
{
public:
    typedef ReadScopedLockTmpl<NullRWMutex> ReadLock;
    typedef WriteScopedLockTmpl<NullRWMutex> WriteLock;

    NullRWMutex() {}
    ~NullRWMutex() {}
    void rdlock() {}
    void wrlock() {}
    void unlock() {}

};
class RWMutex
{
public:
    typedef ReadScopedLockTmpl<RWMutex> ReadLock;
    typedef WriteScopedLockTmpl<RWMutex> WriteLock;
    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }
    //加读锁
    void rdlock() {
        pthread_rwlock_rdlock(&m_lock);
    }
    //加写锁
    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }
private:
    pthread_rwlock_t m_lock;
};

//自旋锁
class SpinLock
{
public:
    typedef ScopedLockTmpl<SpinLock> Lock;
    SpinLock() {
        pthread_spin_init(&m_mutex, 0);
    }

    ~SpinLock() {
        pthread_spin_destroy(&m_mutex);
    }

    void lock() {
        pthread_spin_lock(&m_mutex);
    }

    void unlock() {
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};
#if 1
//原子锁
class CASLock
{
public:
    typedef ScopedLockTmpl<CASLock> Lock;
    CASLock() {
        m_mutex.clear();
    }

    ~CASLock() {
    }

    void lock() {
        while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    void unlock() {
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }
private:
    volatile std::atomic_flag m_mutex;
};
#endif
}

#endif //__THREAD_H__

