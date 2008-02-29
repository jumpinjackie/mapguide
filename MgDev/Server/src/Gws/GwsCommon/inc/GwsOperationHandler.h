//
// Copyright (C) 2004-2008 by Autodesk, Inc.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of version 2.1 of the GNU Lesser
// General Public License as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "GwsObjectImpl.h"

// template class for the operation handlers
template <class T> class CGwsOperationHandler: public GWSObject<T, GWSMultiThreadModel>,
                                               public CGwsObject
{
public:
    CGwsOperationHandler ()
    {
        m_status = eGwsOk;
        m_bCanceled = false;
        m_measure = 0;
        m_scale = 0;
    }

    virtual ~CGwsOperationHandler () {}

    //method is called to Execute the operation from the worker thread
    virtual void Execute() = 0;
    //called just before Execute()
    virtual void Open() = 0;
    //called after Execute completes.
    virtual void Close() = 0;
    //Calls Execute() on the thread class, which creates the thread
    virtual void Start() = 0;

    virtual EGwsStatus  Cancel ()
    {
        ThisLock lock(this);

        if (! IsInProgress ())
            return eGwsOperationIsNotRunning;
        m_bCanceled = true;

        GwsOperationHandler::GwsSetProgressMessage (
                                this,
                                IDS_PROGRESS_OPERATIONCANCELED,
                                0, 0);
        return eGwsOk;
    }

    void SetCompletionStatus (EGwsStatus stat, IGWSException * ex)
    {
        ThisLock lock (this);
        m_status = stat;
        m_exception = ex;
        if (ex != NULL)
            ex->AddRef ();

        if (IGWSException::IsError (stat)) {
            GwsOperationHandler::GwsSetProgressMessage (
                                this,
                                IDS_PROGRESS_OPERATIONFAILED,
                                0, 0);

        } else if (IGWSException::IsWarning (stat)) {
            GwsOperationHandler::GwsSetProgressMessage (
                                this,
                                IDS_PROGRESS_OPERATIONWITHWARNING,
                                0, 0);
        } else {
            GwsOperationHandler::GwsSetProgressMessage (
                                this,
                                IDS_PROGRESS_OPERATIONSUCESSFUL,
                                0, 0);
        }

        SetMeasure (0);
        SetScale (0);
    }

    EGwsStatus GetCompletionStatus (IGWSException ** exception)
    {
        ThisLock lock (this);

        if (exception != NULL)
            * exception = NULL;

        if (IsInProgress ()) {
            return eGwsOperationInProgress;
        }
        if (exception != NULL) {
            * exception = m_exception;
            if (* exception)
                (* exception)->AddRef ();
        }
        return m_status;
    }

    virtual bool IsCanceled ()
    {
        ThisLock lock(this);
        return m_bCanceled;
    }

    virtual wchar_t * GetProgressMessage (wchar_t * msgbuff, int len)
    {
        ThisLock lock (this);
        if (msgbuff != NULL) {
            int csize = min (len - 1, (int)m_pmsg.size ());
            if (csize > 0) {
                wcsncpy (msgbuff, m_pmsg.c_str (), csize);
                msgbuff [csize] = 0;
            }
        }
        return msgbuff;
    }

    virtual long GetMeasure ()
    {
        ThisLock lock (this);
        return m_measure;
    }

    virtual long GetScale ()
    {
        ThisLock lock (this);
        return m_scale;
    }

    // set progress message
    virtual void SetProgressMessage (const wchar_t * msg)
    {
        ThisLock lock (this);
        if (msg != NULL)
            m_pmsg = msg;
        else
            m_pmsg.clear ();
    }

    virtual void SetMeasure (long measure)
    {
        ThisLock lock (this);
        m_measure = measure;
    }

    virtual void SetScale (long scale)
    {
        ThisLock lock (this);
        m_scale = scale;
    }

protected:
    EGwsStatus                m_status;
    FdoPtr<IGWSException>     m_exception;
    bool                      m_bCanceled;   // cancel flag

    // progress message
    WSTR                      m_pmsg;
    long                      m_measure;
    long                      m_scale;
};

//dummy message base class
class CGwsMessage : public GWSObject<IGWSObject>
{
public:
    CGwsMessage() {}
    virtual ~CGwsMessage() {}
};

#endif
