//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: KeyGenerator.hpp,v 1.11 2005/10/03 14:05:21 jsnelson Exp $
//

#ifndef __KEYGENERATOR_HPP
#define	__KEYGENERATOR_HPP

#include <cctype>
#include "nodeStore/NsUtil.hpp"

namespace DbXml
{

class KeyGenerator
{
public:
	typedef shared_ptr<KeyGenerator> Ptr; ///< A reference counted pointer to a Key Generator
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
	virtual ~SingleKeyGenerator()
	{
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

class SubstringKeyGenerator : public KeyGenerator
{
public:
	SubstringKeyGenerator(const char *s, size_t l)
	{
		set(s, l);
	}
	virtual ~SubstringKeyGenerator()
	{
	}
	virtual void set(const char *s, size_t l)
	{
		i_= 0;
		c_= 0;
		v_.reserve(l);
		const char *p= s;
		while(p<s+l)
		{
			unsigned int c= *((const unsigned char *)p);
			size_t cl= NsUtil::nsCharSizeUTF8(c);
			if(cl==1)
			{
				if(isCharacter(c))
				{
#if (_MSC_VER==1200) // Microsoft Visual Studio 6
					v_.push_back(tolower(c));
#else
					v_.push_back(std::tolower((int)c));
#endif
				}
			}
			else
			{
				v_.insert(v_.end(), p, p+cl);
			}
			p+= cl;
			c_++;
		}
	}
	virtual bool next(const char *&p, size_t &pl)
	{
		bool r= false;
		if(i_!=v_.size())
		{
			p= &v_[i_];
			pl= 0;
			size_t i= i_;
			int n;
			for(n=0;n<3 && i<v_.size();++n)
			{
				size_t cl= NsUtil::nsCharSizeUTF8(v_[i]);
				i+= cl;
				pl+= cl;
				if (n==0) i_+= cl;
			}
			r= (n==3);
		}
		return r;
	}
	virtual size_t noOfKeys()
	{
		return c_ < 2 ? 0 : c_ - 2;
	}
private:
	bool isCharacter(int c) const
	{
		return (c>=0x7f || (c<0x7f && isalnum(c)));
	}
	std::vector<char> v_;
	size_t i_;
	size_t c_;
};

}

#endif
