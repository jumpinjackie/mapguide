//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: KeyGenerator.hpp,v 1.15 2006/10/30 17:45:52 bostic Exp $
//

#ifndef __KEYGENERATOR_HPP
#define	__KEYGENERATOR_HPP

#include "Buffer.hpp"
#include "SharedPtr.hpp"

#include <xqilla/utils/UCANormalizer.hpp>

namespace DbXml
{

class KeyGenerator
{
public:
	typedef SharedPtr<KeyGenerator> Ptr; ///< A reference counted pointer to a Key Generator
	KeyGenerator() {}
	virtual ~KeyGenerator() {}
	virtual void set(const char *s, size_t l) = 0;
	virtual bool next(const char *&p, size_t &pl) = 0;
	virtual size_t noOfKeys() = 0;
};

class EmptyKeyGenerator : public KeyGenerator
{
public:
	EmptyKeyGenerator() {}
	virtual ~EmptyKeyGenerator() {}
	virtual void set(const char *s, size_t l) {}
	virtual bool next(const char *&p, size_t &pl)	{ return false; }
	virtual size_t noOfKeys() { return 0; }
};

class SingleKeyGenerator : public KeyGenerator
{
public:
	SingleKeyGenerator(const char *s, size_t l)
	{
		set(s, l);
	}
	virtual void set(const char *s, size_t l)
	{
		done_= false;
		s_= s;
		l_= l;
	}
	virtual bool next(const char *&p, size_t &pl)
	{
		if(done_) {
			return false;
		} else {
			p= s_;
			pl= l_;
			done_= true;
			return true;
		}
	}
	virtual size_t noOfKeys() { return 1; }

private:
	bool done_;
	const char *s_;
	size_t l_;
};

class SubstringKeyGenerator : public KeyGenerator, private StringTransform
{
public:
	SubstringKeyGenerator(const char *s, size_t l);

	virtual void set(const char *s, size_t l);
	virtual bool next(const char *&p, size_t &pl);
	virtual size_t noOfKeys();

private:
	virtual void pushChar(unsigned int ch);

	Buffer buf_;
	const char *p_;
	size_t c_;
};

}

#endif
