//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef _GWSTHREADS_H
#define _GWSTHREADS_H

#define THREAD_STACKSIZE    4096
#define INVALID_HANDLE      0

#ifdef _WIN32
#include <winbase.h>
#else
#include <semaphore.h>
#endif

#ifndef INFINITE
#define INFINITE            0xFFFFFFFF  // Infinite timeout
#endif

//This class is intended for use as a base class for thread-aware classes
//It's a generic mutex class based on the Posix thread
//protocol and implemented for Win32. This classes API should, therefore
//be implementable on *nix as well.
class GWSMutexBase
{
public:
    GWSMutexBase() {}
    virtual ~GWSMutexBase() {}
    virtual void Lock() const {}
    virtual void Unlock() const {}
};

class GWSMutex : public GWSMutexBase {
private:
#ifdef _WIN32
    mutable CRITICAL_SECTION lock;
public:
    GWSMutex() {
        InitializeCriticalSection(&lock);
    }

    virtual ~GWSMutex() {
        DeleteCriticalSection(&lock);
    }

    virtual void Lock() const {
        EnterCriticalSection(&lock);
    }

    virtual void Unlock() const {
        LeaveCriticalSection(&lock);
    }
#else
private:
   mutable pthread_mutex_t m_CritSect;
   mutable pthread_mutexattr_t m_attributes;
public:
    GWSMutex() {
        pthread_mutexattr_init(&m_attributes);
        pthread_mutexattr_settype(&m_attributes, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_CritSect, &m_attributes);
        pthread_mutexattr_destroy(&m_attributes);
   }

    virtual ~GWSMutex() {
       pthread_mutex_destroy(&m_CritSect);
   }

    virtual void Lock() const {
       pthread_mutex_lock(&m_CritSect);
   }

    virtual void Unlock() const {
       pthread_mutex_unlock(&m_CritSect);
   }

#endif
};

//This is a generic semaphore class based on the Posix thread
//protocol and implemented for Win32. This classes API should, therefore
//be implementable on *nix as well.
class GWSSemaphore {
#ifdef _WIN32
private:
        HANDLE semaphore;

    public:
    GWSSemaphore() {
        semaphore = CreateSemaphore(NULL, 0, 0x7ffffff, NULL);
    }

    GWSSemaphore(int available) {
        semaphore = CreateSemaphore(NULL, available, 0x7ffffff, NULL);
    }

    ~GWSSemaphore() {
        CloseHandle(semaphore);
    }

    void Wait(unsigned long msToWait = INFINITE) const {
        WaitForSingleObject(semaphore, msToWait);
    }

    void Post() const {
        ReleaseSemaphore(semaphore, 1, NULL);
    }

    void Post(int how_many) const {
        ReleaseSemaphore(semaphore, how_many, NULL);
    }
#else
    private:
        sem_t semaphore;

    public:
    GWSSemaphore() {
        sem_init(&semaphore, 0, 0);
    }

    GWSSemaphore(int available) {
        sem_init(&semaphore, 0, available);
    }

    ~GWSSemaphore() {
        sem_destroy(&semaphore);
    }

    void Wait(unsigned long msToWait = INFINITE) {
        sem_wait(&semaphore);
    }

    void Post() {
        sem_post(&semaphore);
    }

    void Post(int how_many) {
        sem_post(&semaphore);
    }
#endif
};

//This is the basic message that is queued. Note the only
//requrement is that the message have by value copy semantics.
template <class T> class GWSQueueEntry {
public:
    T* value;
    GWSQueueEntry* next;

    GWSQueueEntry(T* item) {
        value = item;
        next = NULL;
    }
};

//Any number of threads (but in our case maybe just one) can call Wait() on the queue,
//and as entries are added by other threads, that the calling thread is woken up and returns
//with the value. If no threads are calling Wait(), the next time it's called it will
//just pick up the value immediatly (assuming Add() is not being called simultaneously).
#ifdef _WIN32
template <class T> class GWSMessageQueue : private GWSSemaphore, private GWSMutex {
private:
    GWSQueueEntry<T>* head;
    GWSQueueEntry<T>* tail;

public:
    GWSMessageQueue() {
        head = tail = NULL;
    }

    void AddMessage(T* pItem) {
        Lock();
        if (tail == (GWSQueueEntry<T>*)0)
            head = tail = new GWSQueueEntry<T>(pItem);
        else {
            tail->next = new GWSQueueEntry<T>(pItem);
            tail = tail->next;
        }
        Unlock();
        Post(); // wake up any waiting threads
    }

    //GetMessage doesn't wait unless you tell it to
    T* GetMessage(bool bWait = false, unsigned long bWaitTimeMS = IGNORE) {
        if(bWait)
            GWSSemaphore::Wait(bWaitTimeMS); // wait for something to show up
        Lock();
        T* value = NULL;
        if(head)
            value = head->value;
        else
        {
            Unlock();
            return value;
        }
        GWSQueueEntry<T>* old = head;
        head = head->next;
        delete old;
        if (head == NULL)
            tail = NULL;
        Unlock();
        return value;
    }
};
#endif

//stuff which we know is Win32 specific
#ifdef _WIN32
# define THREAD_CALL                WINAPI
# define THREAD_HANDLE              HANDLE
# define THREAD_RET_T               DWORD
# define CREATE_THREAD_FAILED       (0L)
# define CREATE_THREAD_ERROR        GetLastError()
# define CREATE_THREAD(_S,_F,_P)    ((THREAD_HANDLE)CreateThread(0,_S,(DWORD (WINAPI *)(void *))_F,(void *)_P,0,0))
# define EXIT_THREAD                ExitThread(0)
# define CLOSE_HANDLE(x)            CloseHandle(x)
# define THREAD_RETURN(x)           return(x)
#else
//TODO:Posix versions, whatever
# define THREAD_CALL
#endif

//This is a generic thread class based on the Posix thread
//protocol and implemented for Win32 - derive from this class and override
//ThreadMain() passing whatever context you need in GWSThreadParam
#ifdef _WIN32
template <typename GWSThreadParam> class GWSThread
{
protected:
    GWSThread() {}
    virtual GWSThreadParam* GetThreadParam() = 0;
    virtual void ThreadMain( GWSThreadParam* param ) = 0;
    static void Exit(){ ExitThread(0); }
    static void TestCancel(){ Sleep(0); }
    static THREAD_HANDLE Self()
    {
        // only a pseudo-handle!
        return (THREAD_HANDLE)GetCurrentThread();
    }

public:
    typedef void (* FuncPtr)( GWSThreadParam* );

    int Create(GWSThreadParam* ThreadParam,
                THREAD_HANDLE* const& tHandle   = 0,
                const bool& bCreateDetached      = false,
                const unsigned int& StackSize   = 0,
                const bool& bCancelEnable        = false,   // UNUSED
                const bool& bCancelAsync         = false    // UNUSED
    ) const
    {
        //While we are actively creating the thread, we need to lock our local data
        StaticMutex().Lock();
        //just a tricky way to pass these parameters on the stack to
        //the new thread, that way we don't have to allocate memory from
        //the heap
        Instance temp(ThreadParam,const_cast<GWSThread *>(this));
        const GWSSemaphore& semaphore = StaticSemaphore();
        THREAD_HANDLE handle(CREATE_THREAD(StackSize,ThreadMainHandler,&temp));

        if ( handle == CREATE_THREAD_FAILED )
        {
            if ( tHandle )
                *tHandle = INVALID_HANDLE;
            StaticMutex().Unlock();
            return CREATE_THREAD_ERROR;
        }

        if ( tHandle )
            *tHandle = handle;
        //Now wait for the newly created thread to signal the semaphore
        semaphore.Wait();
        StaticMutex().Unlock();

        if ( bCreateDetached )
            CLOSE_HANDLE(handle);
        return 0;
    }

    // Wait for thread with handle H to complete
    // returns EAGAIN, EINVAL on error or zero on success.
    static int Join( const THREAD_HANDLE& H )
    {
      DWORD R = WaitForSingleObject((THREAD_HANDLE)H,INFINITE);

      if ( (R == WAIT_OBJECT_0) || (R == WAIT_ABANDONED) )
      {
        CLOSE_HANDLE(H);
        return 0;
      }

      if ( R == WAIT_TIMEOUT )
          return EAGAIN;
      return EINVAL;
    }

    static int Kill( const THREAD_HANDLE& H )
        { return TerminateThread((HANDLE)H,0) ? 0 : EINVAL; }

    static int Detach( const THREAD_HANDLE& H )
        { return (CLOSE_HANDLE(H)?0:EINVAL); }

private:
    struct Instance
    {
        Instance( GWSThreadParam* P, GWSThread<GWSThreadParam>*const& O, const typename GWSThread<GWSThreadParam>::FuncPtr &pH = 0 )
        : pFN(pH), pData(P), pOwner(O) {}

        typename GWSThreadParam*                    pData;
        GWSThread<GWSThreadParam>*                  pOwner;
        typename GWSThread<GWSThreadParam>::FuncPtr pFN;
    };

    static const GWSMutex& StaticMutex()      { static GWSMutex M; return M; }
    static const GWSSemaphore& StaticSemaphore()  { static GWSSemaphore S; return S; }

    static THREAD_RET_T THREAD_CALL ThreadMainHandler( Instance *Param )
    {
      Instance  I(*Param);
      GWSThreadParam*  pData = I.pData;

      StaticSemaphore().Post(); //Causes the Create() function to stop waiting

      if ( I.pOwner )
        I.pOwner->ThreadMain(pData);
      else
        I.pFN(pData);

      Exit();
      THREAD_RETURN(0);
    }

};

template <class Handler> class CGwsOperationHandler;

template<class Handler, class GWSMessage = CGwsMessage>
class GWSHandlerThread : public GWSThread< GWSMessageQueue<GWSMessage> >
{
public:
    GWSHandlerThread(CGwsOperationHandler<Handler>* pOwner)
    {
        m_pHandler = pOwner;
        mHandle = NULL;
        m_bRunning = false;
    }

    virtual ~GWSHandlerThread() {}

    virtual void Execute()
    {
        try
        {
            m_pHandler->Open();
            m_bRunning = true;
            Create(GetThreadParam(), mHandle, true , THREAD_STACKSIZE);
        }
        catch(...)
        {
            m_bRunning = false;
            m_pHandler->Close();
            throw IGWSException::Create(eGwsFailedToCreateThread);
        }
    }

    virtual bool IsRunning()
    {
        return m_bRunning;
    }

protected:
    virtual GWSMessageQueue<GWSMessage>* GetThreadParam()
    {
        return NULL;
    }

    virtual void ThreadMain( GWSMessageQueue<GWSMessage>* param )
    {
        try {
            assert(m_pHandler != NULL);
            m_pHandler->Execute();
            m_bRunning = false;
            m_pHandler->Close();
        } catch (...) {
            assert (false);
        }
    }

    CGwsOperationHandler<Handler>* m_pHandler;
    THREAD_HANDLE*                 mHandle;
    bool                           m_bRunning;
    FdoPtr<IGWSException>          m_completion;
};

template<class Handler, class GWSMessage>
class GWSQueuedMessageThread : public GWSHandlerThread< Handler, GWSMessage >
{
public:
    GWSQueuedMessageThread(CGwsOperationHandler<Handler>* pOwner) :
        GWSHandlerThread(pOwner)
    {
    }

    virtual ~GWSQueuedMessageThread()
    {
        GWSMessage* pMessage = NULL;
        while( (pMessage = GetMessage()) != NULL)
        {
            pMessage->Release();
        }
    }

    virtual GWSMessageQueue<GWSMessage>* GetThreadParam()
    {
        return &m_messageQueue;
    }

    virtual GWSMessage* GetMessage(bool bWait = false, unsigned long msTimeOut = IGNORE)
    {
        return m_messageQueue.GetMessage(bWait, msTimeOut);
    }

    virtual void AddMessage(GWSMessage* pMessage)
    {
        m_messageQueue.AddMessage(pMessage);
    }

protected:
    GWSMessageQueue<GWSMessage> m_messageQueue;
};

#pragma warning( disable : 4311 )
#pragma warning( disable : 4312 )
#endif

class GWSSingleThreadModel
{
public:
    static unsigned long THREAD_CALL Increment(long* p) throw() {return ++(*p);}
    static unsigned long THREAD_CALL Decrement(long* p) throw() {return --(*p);}
    static void* THREAD_CALL ExchangePointer(void** ppOrig, void* pExchange) throw()
    {
        void* ppRet = *ppOrig;
        *ppOrig = pExchange;
        return ppRet;
    }
    typedef GWSMutexBase CriticalSection;
};

class GWSMultiThreadModel
{
public:
    static unsigned long THREAD_CALL Increment(long* p) throw()


    {
#ifdef _WIN32
        return InterlockedIncrement(p);
#else
        CriticalSection crit;
        crit.Lock();
        ++(*p);
        crit.Unlock();

        return (*p);
#endif
    }

    static unsigned long THREAD_CALL Decrement(long* p) throw()
    {
#ifdef _WIN32
        return InterlockedDecrement(p);
#else
        CriticalSection crit;
        crit.Lock();
        --(*p);
        crit.Unlock();

        return (*p);
#endif
    }

    static void* THREAD_CALL ExchangePointer(void** ppOrig, void* pExchange) throw()
    {
#ifdef _WIN32
        return InterlockedExchangePointer(ppOrig,pExchange);
#else
        CriticalSection crit;
        crit.Lock();
        void* ppRet = *ppOrig;
        *ppOrig = pExchange;
        crit.Unlock();

        return ppRet;
#endif
    }

    typedef GWSMutex CriticalSection;};

#endif //_GWSTHREADS_H
