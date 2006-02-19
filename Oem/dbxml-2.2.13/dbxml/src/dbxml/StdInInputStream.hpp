//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
//   $Id: StdInInputStream.hpp,v 1.3 2005/04/05 16:44:03 bostic Exp $
//

#ifndef __STDININPUTSTREAM_HPP
#define	__STDININPUTSTREAM_HPP

#include "BaseInputStream.hpp"

namespace DbXml
{

/**
 * Concrete implementation of a stdin streaming interface
 */
class StdInInputStream : public BaseInputStream
{
public:
	StdInInputStream();
	virtual ~StdInInputStream();

};

}

#endif
