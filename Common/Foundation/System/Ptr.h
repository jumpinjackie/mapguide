//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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


#ifndef PTR_H_
#define PTR_H_

#ifdef _WIN32
#pragma once
#endif

#define SAFE_RELEASE(x) {if (x) (x)->Release(); (x) = NULL;}
#define SAFE_ADDREF(x)  ((x != NULL) ? ((x)->AddRef(), (x)) : NULL)

class MgException;

/// \cond INTERNAL
//This class ensures T implements AddRef() and Release() and also ensures
//you can't call it through the Ptr
template <class T>
class _NoAddRefReleaseOnPtr : public T
{
    private:
        virtual INT32 AddRef()  = 0;
        virtual INT32 Release() = 0;
};
/// \endcond

template <class T>
#ifdef _WIN32
inline __declspec(nothrow) T* __stdcall PtrAssign(T** pp, T* lp)
#else
inline T* PtrAssign(T** pp, T* lp)
#endif
{
    if (lp != NULL)
        lp->AddRef();
    if (*pp)
        (*pp)->Release();
    *pp = lp;
    return lp;
}

/// \cond INTERNAL
//Ptr smart pointer
template <class T>
class Ptr
{
public:
    typedef T _PtrClass;

    Ptr()  throw()
    {
        p = NULL;
    }

    //We don't seem to need that constructor in our code
    /*
#ifdef _WIN32
    Ptr(INT32 nNull) throw(...)     //TODO inquire: what is the benefit of explicitely indicating that the method
                                    //can throw an exception? I believe none and if it's the case, have
                                    //a common declaration for Windows and Linux, without throw statement
#else
    Ptr(INT32 nNull)
#endif
    {
        if (nNull != 0)
            throw; // ::Create(MgException::NLSGetMessage(_NLSID(_2_BADPARAMETER)));
        p = NULL;
    }
    */

    Ptr(T* lp) throw()
    {
        p = lp;

        if(p)
        {
            p->SetRefCountFlag();
        }
    }

    Ptr(const Ptr<T>& lp) throw()
    {
        p = lp.p;
        if (p != NULL)
        {
            p->AddRef();
        }
    }

    ~Ptr() throw()
    {
        if (p)
        {
            p->Release();
        }
    }

    operator T*() const throw()
    {
        return p;
    }

#ifdef _WIN32
    T& operator*() const throw(...)
#else
    T& operator*() const
#endif
    {
        if (p==NULL)
            throw; // ::Create(MgException::NLSGetMessage(_NLSID(_2_BADPARAMETER)));
        return *p;
    }

#ifdef _WIN32
    T** operator&() throw(...)
#else
    T** operator&()
#endif
    {
        if (p!=NULL)
            throw; // ::Create(MgException::NLSGetMessage(_NLSID(_2_BADPARAMETER)));
        return &p;
    }

#ifdef _WIN32
    _NoAddRefReleaseOnPtr<T>* operator->() const throw(...)
#else
    _NoAddRefReleaseOnPtr<T>* operator->() const
#endif
    {
        if (p==NULL)
            throw; // ::Create(MgException::NLSGetMessage(_NLSID(_2_BADPARAMETER)));
        return (_NoAddRefReleaseOnPtr<T>*)p;
    }

    bool operator!() const throw()
    {
        return (p == NULL);
    }

    bool operator<(T* pT) const  throw()
    {
        return p < pT;
    }

    bool operator==(T* pT) const throw()
    {
        return p == pT;
    }

    T* operator=(T* lp) throw()
    {
        SAFE_RELEASE(p);

        p = lp;

        if(p)
        {
            p->SetRefCountFlag();
        }

        return p;
    }

    T* operator=(const Ptr<T>& lp) throw()
    {
        return static_cast<T*>(PtrAssign<T>(&p, lp));
    }

    template<class U>
    T* operator=(const Ptr<U>& lp) throw()
    {
        return static_cast<T*>(PtrAssign<T>(&p, lp));
    }

    /// Attach to an existing interface (does not AddRef)
    void Attach(T* p2) throw()
    {
        if (p)
        {
            p->Release();
        }
        p = p2;
    }

    /// Detach the interface (does not Release)
    T* Detach() throw()
    {
        T* pt = p;
        p = NULL;
        return pt;
    }

    bool CopyTo(T** ppT) throw()
    {
        if (ppT == NULL)
            return false;
        *ppT = p;
        if (p)
        {
            p->AddRef();
        }
        return true;
    }

    T* p;
};
/// \endcond

#endif

