//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
//   $Id: URLInputStream.hpp,v 1.7 2006/10/30 17:45:53 bostic Exp $
//

#ifndef __URLINPUTSTREAM_HPP
#define	__URLINPUTSTREAM_HPP

#include "BaseInputStream.hpp"
#include <string>

namespace DbXml
{

/**
 * Concrete implementation of a URL streaming interface
 */
class URLInputStream : public BaseInputStream
{
public:

	URLInputStream(const std::string &baseId, 
		       const std::string &systemId, 
		       const std::string &publicId);

	URLInputStream(const std::string &baseId, 
		       const std::string &systemId);

	virtual ~URLInputStream();
};

}

#endif
