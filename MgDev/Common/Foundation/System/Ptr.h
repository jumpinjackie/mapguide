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

#ifndef PTR_H_
#define PTR_H_

#ifdef _WIN32
#include <crtdbg.h>
#else
#define _ASSERT(x)
#endif

#define SAFE_RELEASE(x) {if (x) (x)->Release(); (x) = NULL;}
#define SAFE_ADDREF(x)  ((x != NULL)? ((x)->AddRef(), (x)) : NULL)

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
    if (lp)
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

    Ptr() throw()
    {
        p = NULL;
    }

    Ptr(T* lp) throw()
    {
        p = lp;
        if (p)
            p->SetRefCountFlag();
    }

    Ptr(const Ptr<T>& lp) throw()
    {
        p = lp.p;
        if (p)
            p->AddRef();
    }

    template <class U>
    Ptr(const Ptr<U>& lp) throw()
    {
        p = lp.p;
        if (p)
            p->AddRef();
    }

    ~Ptr() throw()
    {
        if (p)
            p->Release();
    }

    operator T*() const throw()
    {
        return p;
    }

    T& operator*() const throw()
    {
        return *p;
    }

    T** operator&() throw()
    {
        _ASSERT(p == NULL);
        return &p;
    }

    _NoAddRefReleaseOnPtr<T>* operator->() const throw()
    {
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
        if (p)
            p->SetRefCountFlag();

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
            p->Release();
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
            p->AddRef();
        return true;
    }

    T* p;
};
/// \endcond

#endif
