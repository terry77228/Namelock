#ifndef Name_Lock_h
#define Name_Lock_h

#include <semaphore.h>
#include <string>

class lock_exception : std::exception{
public:
    lock_exception(const std::string& _errorInfo, int _errorCode):m_ErrorInfo(_errorInfo),m_ErrorCode(_errorCode){
    }
    ~lock_exception(){
    }
    const char * what(){
        return m_ErrorInfo.c_str();
    }
    int errorCode(){
        return m_ErrorCode;
    }
    
private:
    std::string m_ErrorInfo;
    int m_ErrorCode;
};

class NameLock{
    
public:
    NameLock(const std::string& _name);
    ~NameLock();
    int Wait(int * _errno = nullptr);
    int Release(int * _errno = nullptr);
    int TryWait(int * _errno = nullptr);
    int TimedWait(int _timeout, int * _errno = nullptr);
private:
    NameLock();
    sem_t * m_sem;
    std::string m_name;
    bool m_locked;
};
#endif
