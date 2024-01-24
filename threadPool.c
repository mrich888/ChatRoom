#include "threadPool.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include <stdio.h>
#include <pthread.h>


#define NUMBER 2
enum STSTUS_CODE
{
    ON_SUCCESS,
    NULL_PTR,
    MALLOC_ERROR,
    INVALID_ACCESS,
    THREAD_CREATE_ERR,
    UNKNOWN_ERROR,
};

#define DEFAULT_MIN_THREADS 5
#define DEFAULT_MAX_THREADS 100
#define DEFAULT_MAX_QUEUES 100

/* 静态函数前置声明 */
static void * thread_Hander(void *arg);
static void * manager_Hander(void *arg);
static void * pthreadExitClearThreadIndex(threadpool_t *pool);

static void * pthreadExitClearThreadIndex(threadpool_t *pool)
{
    int ret = 0;

    /* 判断当前的线程在数组threads对应的是哪一个索引 */
    for (int idx = 0; idx < pool->maxThreads; idx++)
    {
        if (pool->threadId[idx] == pthread_self())
        {
            /*  将需要退出的线程 对应的索引清空*/
            pool->threadId[idx] = 0;
            break;
        }

    }
    pthread_exit(NULL);
    return ret;
}


static void * thread_Hander(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;

    while (1)
    {
        pthread_mutex_lock(&(pool->mutexpool));
        while (pool->queueSize == 0 && pool->shutDown == 0)
        {
            pthread_cond_wait(&(pool->notEmpty), &(pool->mutexpool));

            if (pool->exitThreadNums > 0)
            {
                /* 需要销毁的线程数-1 */
                pool->exitThreadNums--;

                if (pool->liveThreadNums > pool->minThreads)
                {
                    /* 活着的线程数-1 */
                    pool->liveThreadNums--;
                    /* 解锁 */
                    pthread_mutex_unlock(&(pool->mutexpool));
                    pthreadExitClearThreadIndex(pool);
                }
            }

        }
        /*  销毁线程池 标识位置 */
        if (pool->shutDown == 1)
        {
            /* 解锁 */
            pthread_mutex_unlock(&(pool->mutexpool));
            /* 线程退出 */
            pthreadExitClearThreadIndex(pool);

        }

        /* 从任务队列中取数据 */
        task_t task = pool->taskQueue[pool->queueFront];
        /* 任务队列的任务数-1 */
        pool->queueSize--;
        /* front（头部）向后移动 */
        pool->queueFront = (pool->queueFront + 1) % pool->queueCapacity;
        pthread_mutex_unlock(&pool->mutexpool);
        pthread_cond_signal(&pool->notFull);

        pthread_mutex_lock(&pool->mutexBusy);
        /* 忙碌的线程+1 */
        pool->busyThreadNums++;
        pthread_mutex_unlock(&pool->mutexBusy);

        /* 执行处理函数 */
        task.worker_hander(task.arg);

        pthread_mutex_lock(&(pool->mutexBusy));
        /* 忙碌的线程数+1 */
        pool->busyThreadNums--;
        pthread_mutex_unlock(&pool->mutexBusy);

    }

}
static void * manager_Hander(void *arg)
{
    threadpool_t *pool = (threadpool_t *)arg;

    while (pool->shutDown != 1)
    {
        sleep(3);
        pthread_mutex_lock(&pool->mutexpool);
        int liveThreadNums = pool->liveThreadNums;
        int queueSize = pool->queueSize;
        pthread_mutex_unlock(&pool->mutexpool);

        pthread_mutex_lock(&pool->mutexBusy);
        int busyThreadNums = pool->busyThreadNums;
        pthread_mutex_unlock(&pool->mutexBusy);

        /* 扩大线程池中线程的容量 */
        /* 任务的个数 > 存货的线程个数 && 存货的线程数 < 最大的线程数 */
        if (queueSize > liveThreadNums && liveThreadNums < pool->maxThreads)
        {
            int count = 0;
            /* 加锁 */
            pthread_mutex_lock(&pool->mutexpool);
            for (int idx = 0; idx < pool->maxThreads && count < NUMBER && pool->liveThreadNums < pool->maxThreads; idx++)
            {
                if (pool->threadId[idx] == 0)
                {
                    pthread_create(&pool->threadId[idx], NULL, thread_Hander, pool);
                    count++;
                    /* 更新线程池存货的线程数 */
                    pool->liveThreadNums++;
                }
            }
            /* 解锁 */
            pthread_mutex_unlock(&pool->mutexpool);
        }

        /* 缩小线程池中的线程容量 */
        /* 忙的线程 * 2 < 存活的线程数 && 存活的线程 > 最小线程数 */
        if (busyThreadNums * 2 < liveThreadNums && liveThreadNums > pool->minThreads)
        {
            pthread_mutex_lock(&pool->mutexpool);
            pool->exitThreadNums = NUMBER;
            pthread_mutex_unlock(&pool->mutexpool);

            /* 唤醒等待工作的线程 等待工作的线程是空闲的线程 */
            for (int idx = 0; idx < NUMBER; idx++)
            {
                /* 发送信号 */
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    pthread_exit(NULL);
}


/* 线程池的初始化 */
int threadPoolInit(threadpool_t *pool, int minThreads, int maxThreads, int taskQueueCapacity)
{
    /* 对线程池判空 */
    if (pool == NULL)
    {
        return NULL_PTR;
    }

    do
    {
        /* 判断线程的合法性 */
        if (minThreads < 0 || maxThreads < 0 || minThreads >= maxThreads)
        {
            minThreads = DEFAULT_MIN_THREADS;
            maxThreads = DEFAULT_MAX_THREADS;
        }
        /* 更新线程池的属性 */
        pool->minThreads = minThreads;
        pool->maxThreads = maxThreads;

        /* 初始化的时候 忙碌的线程为0 */
        pool->busyThreadNums = 0;
        /* 队列的容量 */
        pool->queueCapacity = taskQueueCapacity;
        /* 队列任务数大小 */
        pool->queueSize = 0;

        /* 任务队列 */
        pool->taskQueue = (struct task_t *)malloc(sizeof(struct task_t) * (pool->queueSize));
        if (pool->taskQueue == NULL)
        {
            perror("malloc error");
            break;
        }
        /* 循环队列头位置 */
        pool->queueFront = 0;
        /* 循环队列队尾位置 */
        pool->queueRear = 0;

        pool->threadId = (pthread_t *)malloc(sizeof(pthread_t) * pool->maxThreads);
        if (pool->threadId == NULL)
        {
            perror("malloc error");
            break;
        }
        memset(pool->threadId, 0, sizeof(pthread_t) * pool->maxThreads);

        /* 创建线程 创建最少数量的线程 */
        int ret = 0;
        for (int idx = 0; idx < pool->minThreads; idx++)
        {
            ret = pthread_create(&(pool->threadId[idx]), NULL, thread_Hander, (void *)pool);
            if (ret != 0)
            {
                perror("pthread create error");
                break;
            }
        }
        /* 管理者线程的创建 */
        ret = pthread_create(&pool->managerThread, NULL, manager_Hander, (void *)pool);
        if (ret != 0)
        {
            perror("pthread create error");
            break;
        }
        pool->liveThreadNums = minThreads;
        pool->busyThreadNums = 0;

        if (pthread_mutex_init(&(pool->mutexpool), NULL) != 0 || pthread_mutex_init(&(pool->mutexBusy), NULL) != 0)
        {
            perror("mutex error");
            break;
        }
        /* 销毁线程池 标识位置0 */
        pool->shutDown = 0;
        return ON_SUCCESS;
    }
    while (1);

    /* 释放内存 */
    if (pool->taskQueue != NULL)
    {
        free(pool->taskQueue);
        pool->taskQueue = NULL;
    }
    if (pool->threadId)
    {
        /* 阻塞i回收工作线程资源 */
        for (int idx = 0; idx < pool->minThreads; idx++)
        {
            if (pool->threadId[idx] != 0)
            {
                pthread_join(pool->threadId[idx], NULL);
            }
        }
        /* 释放内存 */
        free(pool->threadId);
        pool->threadId = NULL;
    }

    /* 释放回收管理者线程资源 */
    if (pool->managerThread != 0)
    {
        pthread_join(pool->managerThread, NULL);
    }

    /* 释放锁和条件变量 */
    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_mutex_destroy(&pool->mutexpool);
    pthread_cond_destroy(&pool->notEmpty);
    pthread_cond_destroy(&pool->notFull);

    return UNKNOWN_ERROR;
}

/* 线程池添加任务 */
int threadPoolAddTask(threadpool_t *pool, void *(worker_hander)(void *arg), void *arg)
{
    if (pool == NULL)
    {
        return NULL_PTR;
    }

    /* 加锁 */
    pthread_mutex_lock(&pool->mutexpool);
    /* 任务队列满了 */
    while (pool->queueSize == pool->queueCapacity && pool->shutDown == 0)
    {
        /* 等待条件变量 */
        pthread_cond_wait(&pool->notFull, &pool->mutexpool);
    }
    if (pool->shutDown != 0)
    {
        pthread_mutex_unlock(&pool->mutexpool);
        return ON_SUCCESS;
    }

    /* 将新的任务 添加到任务队列中 */
    pool->taskQueue[pool->queueRear].worker_hander = worker_hander;
    pool->taskQueue[pool->queueRear].arg = arg;
    /* 任务个数加1 */
    pool->queueSize++;
    /* 该队列是循环队列 要让此索引循环起来 */
    pool->queueRear = (pool->queueRear + 1) % pool->queueCapacity;
    pthread_mutex_unlock(&pool->mutexpool);
    pthread_cond_signal(&pool->notEmpty);

    return ON_SUCCESS;

}

/* 线程池的销毁 */
int threadPoolDestory(threadpool_t *pool)
{
    if (pool == NULL)
    {
        return NULL_PTR;
    }
    pool->shutDown = 1;
    /* 阻塞回收管理者线程 */
    pthread_join(pool->managerThread, NULL);
    /* 唤醒阻塞消费者线程 */
    for (int idx = 0; idx < pool->liveThreadNums; idx++)
    {
        pthread_cond_signal(&pool->notEmpty);
    }

    /* 释放堆空间 */
    if (pool->taskQueue)
    {
        free(pool->taskQueue);
        pool->taskQueue = NULL;
    }
    if (pool->threadId)
    {
        free(pool->threadId);
        pool->threadId = NULL;
    }

    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_mutex_destroy(&pool->mutexpool);
    pthread_cond_destroy(&pool->notEmpty);
    pthread_cond_destroy(&pool->notFull);

    return ON_SUCCESS;

}


