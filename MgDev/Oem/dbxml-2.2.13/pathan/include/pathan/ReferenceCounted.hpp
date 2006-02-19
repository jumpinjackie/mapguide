/*
 * Copyright (c) 2004, Parthenon Computing Limited, All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef _REFERENCECOUNTED_HPP
#define _REFERENCECOUNTED_HPP

#include "Pathan.hpp"
#include <xercesc/framework/MemoryManager.hpp>

// for null RefCountPointer instances
#define NULLRCP ((void *)0)

/** Super class for reference counted classes */
class PATHAN_EXPORT ReferenceCounted
{
public:
  ReferenceCounted()
    : _ref_count(0) {}
  virtual ~ReferenceCounted() {}

  /// Increment the reference count
  void incrementRefCount() const
  {
    ++const_cast<unsigned int&>(_ref_count);
  }

  /// Decrement the reference count, deleting if it becomes zero
  virtual void decrementRefCount() const
  {
    if(--const_cast<unsigned int&>(_ref_count) == 0) {
      delete this;
    }
  }

protected:
  unsigned int _ref_count; // mutable
};

/** Super class of all the reference counted wrappers for Items */
template<class T> class RefCountPointer
{
public:
  RefCountPointer(T *p = 0) : _p(p)
  {
    if(_p != 0) _p->incrementRefCount();
  }

  template<class T2> RefCountPointer(const RefCountPointer<T2> &o) : _p((T*)(T2*)o)
  {
    if(_p != 0) _p->incrementRefCount();
  }

  RefCountPointer(const RefCountPointer<T> &o) : _p(o._p)
  {
    if(_p != 0) _p->incrementRefCount();
  }

  RefCountPointer &operator=(const RefCountPointer<T> &o)
  {
    if(_p != o._p) {
      if(_p != 0) _p->decrementRefCount();
      _p = o._p;
      if(_p != 0) _p->incrementRefCount();
    }
    return *this;
  }

  ~RefCountPointer()
  {
    if(_p != 0) _p->decrementRefCount();
  }

  T *operator->() const
  {
    return _p;
  }

  operator T*() const
  {
    return _p;
  }

  T *get() const
  {
    return _p;
  }

  bool isNull() const
  {
    return (_p == 0);
  }

  bool notNull() const
  {
    return (_p != 0);
  }

protected:
  T *_p;
};

template<class T1, class T2>
inline bool operator==(const RefCountPointer<T1> &a, const RefCountPointer<T2> &b)
{
  return (void*)(T1*)a == (void*)(T2*)b;
}

template<class T1, class T2>
inline bool operator!=(const RefCountPointer<T1> &a, const RefCountPointer<T2> &b)
{
  return (void*)(T1*)a != (void*)(T2*)b;
}

template<class T>
inline bool operator==(const RefCountPointer<T> &a, void *b)
{
  return (T*)a == (T*)b;
}

template<class T>
inline bool operator!=(const RefCountPointer<T> &a, void *b)
{
  return (T*)a != (T*)b;
}

#endif
