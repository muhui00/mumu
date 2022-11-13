/*****************************************
 * Copyright (C) 2022 * Ltd. All rights reserved.
 *
 * File name   : thread.cc
 * Author      : muhui
 * Created date: 2022-11-11 14:27:59
 * Description : 
 *
 *******************************************/

#define LOG_TAG "THREAD"
#include "thread.h"
#include "log.h"
#include "util.h"
namespace muhui
{
//线程局部变量 t_thread指向当前线程
static thread_local Thread* t_thread = nullptr;
//线程名
static thread_local std::string t_thread_name = "UNKNOW";

static muhui::Logger::ptr g_logger = MUHUI_LOG_NAME("system");
Semaphore:: Semaphore(uint32_t count)
{
    /**
     * @brief 创建信号量，并为信号量值赋初值
     * @param[in/out] m_semaphore 信号量对象
     * @param[in] pshared 指明信号量的类型。当为0时，用于进程；当为0时，用于线程
     * @param[in] count 信号量值的大小
     */
    // if(0) 不返回错误
    if(sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}
Semaphore::~Semaphore()
{
    //清理信号量占有的资源，当调用该函数，而有线程等待此信号量时，将会返回错信息
    sem_destroy(&m_semaphore);
}

//获取信号量
void Semaphore::wait()
{
    //以阻塞的方式等待信号量，当信号量的值大于零时，执行该函数信号量减一，当信号量为零时，调用该函数的线程将会阻塞
    if(sem_wait(&m_semaphore)) {
        throw std::logic_error("sem_wait error");
    }
}
//释放信号量
void Semaphore::notify()
{
    //以原子操作的方式为将信号量增加1
    if(sem_post(&m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}

//构造函数
Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_cb(cb)
    , m_name(name)
{
    if(name.empty()) {
        m_name = "UNKNOW";
    }
    int ret = pthread_create(&m_thread, nullptr, run, this);
    if(ret) {
        MUHUI_LOG_ERROR(g_logger) << "pthread_create thread fail, ret = " << ret
            << " name = " << name;
        throw std::logic_error("pthread_create error");
    }
    //保证创建的线程会执行，不执行就阻塞在构造函数
    m_semaphore.wait();
}

//析构
Thread::~Thread()
{
    if(m_thread) {
        //设置线程分离
        pthread_detach(m_thread);
    }
}

//等待线程执行完成
void Thread::join()
{
    //std::cout << m_thread << std::endl;
    if(m_thread){
        int ret = pthread_join(m_thread, nullptr);
        if(ret) {
         MUHUI_LOG_ERROR(g_logger) << "pthread_join thread fail, ret = " << ret
            << " name = " << m_name;
        throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
        //pthread_exit(nullptr);
    }
}

//获取当前按线程指针
Thread* Thread::GetThis()
{
    return t_thread;
}

//获取当前线程名称
const std::string& Thread::GetName()
{
    return t_thread_name;
}

//设置当前线程名称
void Thread::setName(const std::string& name)
{
    if(name.empty()) {
        return;
    }
    if(t_thread){
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

void* Thread::run(void* args)
{
    Thread* thread = (Thread*)args;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = muhui::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());

    std::function<void()> cb;
    //当函数里有智能指针的时候，防止它的引用被释放掉
    cb.swap(thread->m_cb);
    //thread->m_cb();
    //唤醒线程
    thread->m_semaphore.notify();
    cb();

    return 0;
}

}
