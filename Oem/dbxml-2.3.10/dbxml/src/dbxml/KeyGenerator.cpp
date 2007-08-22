//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// Some of the transcoding code is from Xerces, and is
// under the Apache license:
//
// The Apache Software License, Version 1.1
//
// Copyright (c) 1999-2004 The Apache Software Foundation.  All rights
//  reserved.
//
// $Id: KeyGenerator.cpp,v 1.2 2006/10/30 17:45:52 bostic Exp $
//

#include "KeyGenerator.hpp"
#include "nodeStore/NsUtil.hpp"

#include <cctype>

using namespace DbXml;
using namespace std;

SubstringKeyGenerator::SubstringKeyGenerator(const char *s, size_t l)
{
	set(s, l);
}

void SubstringKeyGenerator::set(const char *s, size_t l)
{
	buf_.set(0, l + 1);
	c_ = 0;

	// Case fold and remove diacritics from the value
	CaseFoldTransform caseFold(this);
	RemoveDiacriticsTransform diacritics(&caseFold);
	NormalizeTransform normalize(true, false, &diacritics);

	uint32_t ch;
	const char *end = s + l;
	while(s < end) {
		s += NsUtil::UTF8ToCodepoint((const xmlbyte_t*)s, ch);

		// Filter out common whitespace and punctuation
		if(ch >= 128 || isalnum(ch)) {
			normalize.pushChar(ch);
		}
	}
	normalize.pushChar(0);

	p_ = (const char*)buf_.getBuffer();
}

bool SubstringKeyGenerator::next(const char *&p, size_t &pl)
{
	if(p_ == 0) return false;

	p = p_;

	// Check the first character
	if(*p_ == 0) {
		p_ = 0;
		return false;
	}
	p_ += NsUtil::nsCharSizeUTF8(*p_);
	// Check the second character
	if(*p_ == 0) {
		p_ = 0;
		return false;
	}
	const char *tmp = p_;
	tmp += NsUtil::nsCharSizeUTF8(*tmp);
	// Check the third character
	if(*tmp == 0) {
		p_ = 0;
		return false;
	}
	tmp += NsUtil::nsCharSizeUTF8(*tmp);

	// Calculate the length of the three char block
	pl = (tmp - p);
	return true;
}

size_t SubstringKeyGenerator::noOfKeys()
{
	// c_ includes the count for the null terminator
	return c_ < 4 ? 0 : c_ - 3;
}

void SubstringKeyGenerator::pushChar(unsigned int ch)
{
	xmlbyte_t dest[6];
	int size = NsUtil::codepointToUTF8(ch, dest);
	buf_.write(dest, size);
	++c_; // Count the characters as they are written
}
