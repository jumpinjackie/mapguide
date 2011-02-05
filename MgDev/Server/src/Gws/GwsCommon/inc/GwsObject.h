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

#ifndef GWSOBJECT_H
#define GWSOBJECT_H

#include <map>
#include <assert.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "GwsException.h"

//forward declaration
template <class Interface, class ThreadModel>
class GWSObject;

//helper class to lock and unlock the base mutex
template <class Interface, class ThreadModel = GWSThreadModel>
class GWSObjectLock
{
public:
    GWSObjectLock(GWSObject< Interface,ThreadModel > * pObj)
    {
        if (pObj)
            pObj->Lock();
        m_Obj = pObj;
    }
    virtual ~GWSObjectLock()
    {
        if (m_Obj)
            m_Obj->Unlock();
    }
    GWSObject<Interface, ThreadModel>* m_Obj;
};

//forward decl
template <class ChildInterface, class ChildClass, class Interface,  class ThreadModel>
class GWSObjectCollection;

template <class Interface, class ThreadModel = GWSThreadModel>
class GWSObject : public Interface
{
protected:
    GWSObject() : m_dwRef(0L), m_pOwner(NULL) {}
    // Set refcount to -(LONG_MAX/2) to protect destruction and
    // also catch mismatched Release in debug builds
    virtual ~GWSObject() throw()
    {
        m_dwRef = -(LONG_MAX/2); //set it to some big negative number
        SetOperationHandler(NULL);
    }

public:
    //template <class ChildInterface, class ChildClass, class Interface, class ThreadModel >
    //friend class GWSObjectCollection;
    typedef typename ThreadModel::CriticalSection _CritSec;
    typedef GWSObjectLock< Interface, ThreadModel >  ThisLock;

    virtual FdoInt32 AddRef()
    {
        unsigned long l = InternalAddRef();
        return l;
    }

    virtual FdoInt32 Release()
    {
        unsigned long l = InternalRelease();
        if (l == 0)
            Dispose();
        return l;
    }

    virtual IGWSObject* GetOwner()
    {
        OwnerAddRef();
        return m_pOwner;
    }

    void Lock() {m_critsec.Lock();}
    void Unlock() {m_critsec.Unlock();}

    template <class Class>
    static Interface* CreateInstance(IGWSObject* pParent = NULL)
    {
        Class* p = new Class();
        p->AddRef();
        if(pParent)
            p->SetOwner(pParent);
        return p;
    }
    //just a place to hold IGWSOperationHandlers.
    virtual void SetOperationHandler(IGWSOperationHandler* pHandler)
    {
        if (m_opHandler != NULL) {
            if (m_opHandler->IsInProgress ())
            {
                m_opHandler->Cancel ();

                // wait for
                while (m_opHandler->IsInProgress ())
                {
#ifdef _WIN32
                    Sleep (100);
#else
                    sleep (1);
#endif
                }
            }
            {
                ThisLock lock(this);
                m_opHandler = NULL;
            }
        }
        {
            ThisLock lock(this);
            m_opHandler = pHandler;
            FDO_SAFE_ADDREF(pHandler);
        }
    }

protected:
    virtual void Dispose()
    {
        assert(m_pOwner == NULL);
        delete this;
    }

    virtual void SetOwner(IGWSObject* pOwner)
    {
        if(m_pOwner != NULL &&
           pOwner != m_pOwner)
        {
            //ThisLock lock(this);
            ThreadModel::ExchangePointer((void**)&m_pOwner,pOwner);
        }
        else
            m_pOwner = pOwner;
    }

    FdoInt32 OwnerAddRef()
    {
        //ThisLock lock(this);
        if (m_pOwner != NULL)
            return m_pOwner->AddRef();
        else
            return 0;
    }

    FdoInt32 OwnerRelease()
    {
        //ThisLock lock(this);
        if (m_pOwner != NULL)
            return m_pOwner->Release();
        else
            return 0;
    }
    //Note: this implementation AddRef's and Release's the owner
    unsigned long InternalAddRef()
    {
        OwnerAddRef();
        assert(m_dwRef != -1L);
        return ThreadModel::Increment(&m_dwRef);
    }

    unsigned long InternalRefCount() const
    {
        return m_dwRef;
    }

    unsigned long InternalRelease()
    {
#ifdef _DEBUG
        if(m_dwRef == 0)
        {
            //Release called on object with 0 ref count,
            //did you forget to AddRef this when you created it?
            assert(0);
        }
        long nRef = ThreadModel::Decrement(&m_dwRef);
        if(nRef > 0)
            OwnerRelease();
        else
            SetOwner(NULL);
        if (nRef < -(LONG_MAX / 2))
        {
            //Release called on a pointer that has already been released!
            assert(0);
        }
        return nRef;
#else
        long nRef = ThreadModel::Decrement(&m_dwRef);
        if(nRef > 0)
            OwnerRelease();
        else
            SetOwner(NULL);
        return nRef;
#endif
    }

protected:
    long                            m_dwRef;
    IGWSObject*                     m_pOwner;
    _CritSec                        m_critsec;
    FdoPtr<IGWSOperationHandler>    m_opHandler;
};

template <class ChildInterface, class ChildClass, class Interface, class ThreadModel = GWSThreadModel>
class GWSObjectCollection : public GWSObject< Interface, ThreadModel >
{
protected:
    class ChildInfo
    {
    public:
        ChildInfo(): mbIsRemoved(false) {}
        ChildInfo( ChildInterface* pChild, bool bIsRemoved )
            : mChild(pChild), mbIsRemoved(bIsRemoved) {}
        ChildInfo( const ChildInfo& other ){*this = other;}
        ChildInfo& operator=(const ChildInfo& other)
        {
            mChild=other.mChild;
            mbIsRemoved=other.mbIsRemoved;
            return *this;
        }
        ChildInterface* Child() const { return mChild; }
        bool            IsRemoved() const { return mbIsRemoved; }
        void            SetRemoved() { mbIsRemoved = true; }
    private:
        ChildInterface* mChild;
        bool            mbIsRemoved;
    };
    //case insensitive comparison
    typedef struct less_ignore_case : std::less <const std::wstring>
    {
        bool operator()(const std::wstring& s1, const std::wstring& s2) const
        {
            return _wcsicmp(s1.c_str(), s2.c_str()) < 0;
        };
    } ChildIgnoreCase;

public:
    typedef ChildInfo                           GWSChild;
    typedef std::map<std::wstring,GWSChild, ChildIgnoreCase>     GWSChildren;
    typedef typename GWSChildren::iterator               GWSChildIterator;

    template <class Derived>
    void Add(FdoString * name, ChildInterface** pChild)
    {
        if(pChild == NULL)
            throw CGwsException::Create(eGwsNullPointer);

        typename GWSObject< Interface, ThreadModel >::ThisLock lock(this);
        if(Contains(name))
            throw CGwsException::Create(eGwsItemAlreadyAdded);
        //class Derived must be derived from ChildInterface
        *pChild = typename Derived::template CreateInstance<Derived>(this);
        //*pChild = ChildClass::CreateInstance<ChildClass>(this);
        return AddInternal(name, *pChild);
    }

    virtual void AddInternal(FdoString * name, ChildInterface* pChild)
    {
        //true indicates the child has been removed from the
        //official list of children
        GWSChild newChild(pChild, false);
        typename GWSChildren::value_type val(name, newChild);
        typename std::pair<GWSChildIterator, bool> pair =
            mChildren.insert(val);
        if(!pair.second) //was it already there?
        {
            GWSChildIterator iter = pair.first;
            //should have been marked removed
            assert((*iter).second.IsRemoved());
            mChildren.erase(iter);
            mChildren.insert(val);
        }
#ifdef _DEBUG
        IGWSObject* pOwner = pChild->GetOwner();
        assert(pOwner == this);
        pOwner->Release();
#endif
        //AddRef's everything up the chain
        pChild->AddRef();
        mCount++;
    }

    virtual void Remove(FdoString * name)
    {
        typename GWSObject< Interface, ThreadModel >::ThisLock lock(this);
        GWSChildIterator iter = mChildren.find(name);
        if(iter == mChildren.end() || (*iter).second.IsRemoved())
            throw CGwsException::Create(eGwsItemNotFound);

        ChildInterface* pChildI = (*iter).second.Child();
        //In this case, unless the child is about to be
        //deleted with this Release(),
        //we just mark it as removed without actually
        //removing the pointer from the list. That way
        //later, the orphaned child can continue to reference
        //it's parent, thereby keeping the parents alive.
        (*iter).second.SetRemoved();
        //Check to see if this child would be deleted
        //by calling Release() on it. If so, just break the
        //circular dependency
        ChildClass* pChild = static_cast<ChildClass*>(pChildI);
        if(pChild->InternalRefCount() == 1)
        {
            //remove our record of it
            mChildren.erase(iter);
        }
        //release the child
        pChild->Release();
        mCount--;
    }

    virtual ChildInterface* Find(FdoString * name) const
    {
        typename GWSChildren::const_iterator iter = mChildren.find(name);
        if(iter != mChildren.end())
        {
            if(!(*iter).second.IsRemoved())
            {
                ChildInterface* pChild = (*iter).second.Child();
                pChild->AddRef();
                return pChild;
            }
            return NULL;
        }
        return NULL;
    }

    virtual bool Contains(FdoString * name) const
    {
        typename GWSChildren::const_iterator iter = mChildren.find(name);
        if(iter != mChildren.end() && !(*iter).second.IsRemoved())
            return true;
        return false;
    }

    virtual void Rename(FdoString* oldName, FdoString* newName)
    {
        typename GWSObject< Interface, ThreadModel >::ThisLock lock(this);
        GWSChildIterator iter = mChildren.find(oldName);
        if(iter == mChildren.end() || (*iter).second.IsRemoved())
            throw CGwsException::Create(eGwsItemNotFound);

        GWSChild child((*iter).second);
        mChildren.erase(iter);
        typename GWSChildren::value_type val(newName, child);
        mChildren.insert(val);
    }

    virtual int Names(FdoStringCollection** ppNames) const
    {
        if(ppNames == NULL)
            return 0;

        if(*ppNames == NULL)
            *ppNames = FdoStringCollection::Create();

        typename GWSChildren::const_iterator it;
        int i = 0;
        for (it = mChildren.begin(); it != mChildren.end(); it++)
        {
            if((*it).second.IsRemoved())
                continue;
            (*ppNames)->Add((*it).first.c_str());
            i++;
        }
        assert(mCount == i);
        return (*ppNames)->GetCount();
    }

    virtual bool IsEmpty() const
    {
        return (mCount == 0) ? true : false;
    }

protected:
    GWSObjectCollection() : mCount(0) {}
    virtual ~GWSObjectCollection() throw() {}

    virtual void Dispose()
    {
        //release children, if they have not already been officially removed
        GWSChildIterator it;
        for (it = mChildren.begin(); it != mChildren.end(); it = mChildren.begin())
        {
            ChildInterface* pChild = (*it).second.Child();
            bool isRemoved = (*it).second.IsRemoved();
            mChildren.erase(it);
            //Don't call release on the child if it was already called in Remove()
            if(!isRemoved)
                pChild->Release();
        }
        GWSObject<Interface, ThreadModel>::Dispose();
    }

protected:
    GWSChildren mChildren;
    int mCount;
};

#endif /*GWSOBJECT_H*/
