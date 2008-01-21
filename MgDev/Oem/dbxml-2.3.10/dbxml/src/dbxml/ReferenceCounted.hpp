//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: ReferenceCounted.hpp,v 1.19 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __REFERENCECOUNTED_HPP
#define	__REFERENCECOUNTED_HPP

namespace DbXml
{

class ReferenceCounted
{
public:
	ReferenceCounted() : count_(0) {}
	virtual ~ReferenceCounted() {}
	void acquire() { ++count_; }
	void release() { if (--count_ == 0) delete this; }
	int count() const { return count_; }
private:
	int count_;
};

template<class T> class RefCountJanitor
{
public:
	RefCountJanitor(T *p = 0) : p_(p) {
		if(p_) p_->acquire();
	}
	
	RefCountJanitor(const RefCountJanitor<T> &o) : p_(o.p_) {
		if(p_) p_->acquire();
	}

	RefCountJanitor &operator=(const RefCountJanitor<T> &o) {
		if(p_ != o.p_) {
			if(p_) p_->release();
			p_ = o.p_;
			if(p_) p_->acquire();
		}
		return *this;
	}

	~RefCountJanitor() {
		if(p_) p_->release();
	}

	void reset(T *p = 0) {
		if(p_ != p) {
			if(p_) p_->release();
			p_ = p;
			if(p_) p_->acquire();
		}
	}

	T &operator*() const {
		return *p_;
	}

	T *operator->() const {
		return p_;
	}
	
	T *get() const {
		return p_;
	}

	operator bool() const {
		return p_ != 0;
	}
	
private:
	T *p_;
};

// ReferenceCountedProtected	
// a lock-protected instance of ReferenceCounted, that
// protects the count for free-threaded objects
//
class ReferenceCountedProtected
{
public:
	ReferenceCountedProtected();
	virtual ~ReferenceCountedProtected();
	void acquire();
	void release();
	void lock();
	void unlock();
private:
	int count_;
	void *mutex_;
};

class MutexLock
{
public:
	MutexLock(void *mutex);
	~MutexLock();
private:
	MutexLock(const MutexLock&);
	MutexLock &operator=(const MutexLock&);
	
	void *toLock_;
};

}

#endif

