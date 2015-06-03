#include "NameLock.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

NameLock::NameLock(const std::string& _name):m_name(_name),Locked(false){
    m_sem = sem_open(_name.c_str(), O_CREAT | O_EXCL, S_IRWXU | S_IRWXG| S_IRWXO, 1);

    if (errno == EEXIST) {
        m_sem = sem_open(_name.c_str(), O_EXLOCK);
        if (m_sem == SEM_FAILED) {
            throw lock_exception("Open/Create lock failed",errno);
        }
    }
    else{
        throw lock_exception("Open/Create lock failed",errno);
    }

};

NameLock::~NameLock(){
    if (m_locked) {
        Release();
    }
    sem_unlink(m_name.c_str());
    sem_close(m_sem);
}

int NameLock::Wait(int * _errno){

    if (!locked) {
        int l_res = sem_wait(m_sem);
        if (l_res != 0) {
            if(_errno != nullptr)
                *_errno = errno;
            return -1;
        }
        m_locked = true;
    }
    
    return 0;

}

int NameLock::TryWait(int * _errno){

    if (!locked) {
        int l_res = sem_trywait(m_sem);
        if (l_res != 0) {
            if(_errno != nullptr)
                *_errno = errno;
            return -1;
        }
        m_locked = true;
    }
    return 0;

}

int NameLock::TimedWait(int _timeout,int * _errno){

    if (!locked) {
        int l_res =  -1;
        time_t l_BeginTime = time(nullptr);
        
        do{
            l_res = sem_trywait(m_sem);
            if (l_res == 0 || (time(nullptr) - l_BeginTime > _timeout_sec)) {
                break;
            }
            usleep(100);
        }while (1);
        if (l_res != 0) {
            if(_errno != nullptr)
                *_errno = errno;
            return -1;
        }
        
        m_locked = true;
    }
    return 0;


}
int NameLock::Release(int * _errno){
    int l_num = 0;
    sem_getvalue(m_sem, &l_num);

    if(l_num < 1){
        int l_res = sem_post(m_sem);
        if (l_res != 0) {
            if(_errno != nullptr)
                *_errno = errno;
            return -1;
        }
    }
    m_locked = false;
    return 0;
}
