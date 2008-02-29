//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004,2006 Oracle.  All rights reserved.
//
//   $Id: MemBufInputStream.hpp,v 1.9 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __MEMBUFINPUTSTREAM_HPP
#define	__MEMBUFINPUTSTREAM_HPP

#include "BaseInputStream.hpp"
#include <string>

namespace DbXml
{

class Document;

/**
 * Concrete implementation of a memory buffer streaming interface.
 */
class MemBufInputStream : public BaseInputStream
{
public:

	MemBufInputStream(const char *srcDocBytes,
			  const unsigned int byteCount,
			  const char *const bufId,
			  const bool adoptBuffer = false);

	virtual ~MemBufInputStream();
};

}

#endif
