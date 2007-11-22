//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
//   $Id: StdInInputStream.hpp,v 1.5 2006/10/30 17:45:53 bostic Exp $
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
