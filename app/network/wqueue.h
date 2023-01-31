/*
   wqueue.h
   Worker thread queue based on the Standard C++ library list
   template class.
   ------------------------------------------
   Copyright (c) 2013 Vic Hargrave
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __wqueue_h__
#define __wqueue_h__

#include <pthread.h>
#include <list>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

using namespace std;

template <typename T> class wqueue
{
    list<T>          m_queue;
    pthread_mutex_t  m_mutex;
    pthread_cond_t   m_condv;

  public:
    wqueue() {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_condv, NULL);
    }
    ~wqueue() {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_condv);
    }
    void add(T item) {
        pthread_mutex_lock(&m_mutex);
        m_queue.push_back(item);
        pthread_cond_signal(&m_condv);
        pthread_mutex_unlock(&m_mutex);
    }
    T front() {
        pthread_mutex_lock(&m_mutex);
        T item = m_queue.front();
        pthread_mutex_unlock(&m_mutex);
        return item;
    }
    T remove() {
        pthread_mutex_lock(&m_mutex);
        while (m_queue.size() == 0) {
            pthread_cond_wait(&m_condv, &m_mutex);
        }
        T item = m_queue.front();
        m_queue.pop_front();
        pthread_mutex_unlock(&m_mutex);
        return item;
    }
    T removetimed(int timeInMs) {
        struct timespec timeToWait;
        //struct timeval now;
        int rt = 0;
        pthread_mutex_lock(&m_mutex);
        while (m_queue.size() == 0 && rt == 0)
        {
            int addSec = timeInMs / 1000UL;
            int addMSec = timeInMs % 1000UL;
            clock_gettime(CLOCK_REALTIME, &timeToWait);
            timeToWait.tv_sec += addSec;
            timeToWait.tv_nsec += 1000UL*addMSec;

            rt = pthread_cond_timedwait(&m_condv, &m_mutex, &timeToWait);
        }
        //printf("wqueue::removetimed() rt=%d\n", rt);

        T item = NULL;
        if (rt != 0)
        {
            /*
            if ( rt == ETIMEDOUT )
                printf("wqueue::removetimed() ETIMEDOUT rt=%d\n", rt);
            if ( rt == EINVAL )
                printf("wqueue::removetimed() EINVAL rt=%d\n", rt);
            if ( rt == EPERM )
                printf("wqueue::removetimed() EPERM rt=%d\n", rt);
            */
        }
        else
        {
            item = m_queue.front();
            m_queue.pop_front();
        }
        pthread_mutex_unlock(&m_mutex);
        return item;
    }
    int size() {
        pthread_mutex_lock(&m_mutex);
        int size = (int)m_queue.size();
        pthread_mutex_unlock(&m_mutex);
        return size;
    }
};

#endif
