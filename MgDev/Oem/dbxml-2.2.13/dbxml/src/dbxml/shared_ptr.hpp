#ifndef __SHARED_PTR_HPP
#define	__SHARED_PTR_HPP

//
//  shared_ptr.hpp
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//
//  See http://www.boost.org/libs/smart_ptr/shared_ptr.htm for documentation.
//

namespace DbXml
{

//
//  shared_ptr
//
//  An enhanced relative of scoped_ptr with reference counted copy semantics.
//  The object pointed to is deleted when the last shared_ptr pointing to it
//  is destroyed or reset.
//

template <typename T>
class shared_ptr
{
public:
	explicit shared_ptr(T *p = 0): px(p), count(new long(1))
	{}

	shared_ptr(const shared_ptr &r): px(r.px), count(r.count)
	{
		++*count;
	}

	~shared_ptr()
	{
		if (--*count == 0) {
			delete count;
			delete px;
		}
	}

	shared_ptr &operator=(const shared_ptr &r)
	{
		shared_ptr(r).swap(*this);
		return *this;
	}

	void reset(T *p = 0)
	{
		shared_ptr(p).swap(*this);
	}

	void swap(shared_ptr<T> & other) // never throws
	{
		std::swap(px, other.px);
		std::swap(count, other.count);
	}

	T &operator* () const
	{
		return *px;
	}

	T *operator-> () const
	{
		return px;
	}

	T * get() const
	{
		return px;
	}

	operator bool() const // never throws
	{
		return px != 0;
	}

	bool unique() const
	{
		return *count == 1;
	}

	long use_count() const
	{
		return *count;
	}

private:
	T * px;					 // contained pointer
	long *count;
}
; // shared_ptr

template <typename T>
inline bool operator==(shared_ptr<T> const & a, shared_ptr<T> const & b)
{
	return a.get() == b.get();
}

template <typename T>
inline bool operator!=(shared_ptr<T> const & a, shared_ptr<T> const & b)
{
	return a.get() != b.get();
}

} // namespace

#endif  // #ifndef __SHARED_PTR_HPP
