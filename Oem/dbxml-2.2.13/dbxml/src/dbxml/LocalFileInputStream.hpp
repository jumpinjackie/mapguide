//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
//   $Id: LocalFileInputStream.hpp,v 1.4 2005/04/05 16:44:02 bostic Exp $
//

#ifndef __LOCALFILEINPUTSTREAM_HPP
#define	__LOCALFILEINPUTSTREAM_HPP

#include "BaseInputStream.hpp"
#include <string>

namespace DbXml
{

/**
 * Concrete implementation of a local file streaming interface
 */
class LocalFileInputStream : public BaseInputStream
{
public:
	LocalFileInputStream(const std::string &filename);
	virtual ~LocalFileInputStream();
};

}

#endif
