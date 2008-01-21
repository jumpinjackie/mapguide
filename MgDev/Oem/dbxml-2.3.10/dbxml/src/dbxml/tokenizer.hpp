//
// See LICENSE for redistribution information
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: tokenizer.hpp,v 1.12 2006/10/30 17:45:54 bostic Exp $
//

#ifndef DBXML_TOKENIZER_HPP_INCLUDED
#define	DBXML_TOKENIZER_HPP_INCLUDED

#include <string>
#include <errno.h>

namespace DbXml
{

class tokenizer
{
public:
	// Split by whitespace ::== (#x20 | #x9 | #xD | #xA)+
	tokenizer(const std::string &data, const char *sep = " \t\015\012") :
		data_(data),
		sep_(sep),
		start_(0),
		end_(0)
	{}

	int next(std::string &tok)
	{
		if ((start_ = data_.find_first_not_of(sep_, end_)) < 0)
			return (EINVAL);
		if ((end_ = data_.find_first_of(sep_, start_)) < 0)
			end_ = data_.length();
		tok = data_.substr(start_, end_ - start_);
		return (0);
	}

private:
	const std::string &data_;
	const char *sep_;
	int start_;
	int end_;
};

} // namespace

#endif
