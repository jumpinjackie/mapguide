//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
//   $Id: PullEventInputStream.hpp,v 1.1 2005/05/25 16:02:51 jsnelson Exp $
//

#ifndef __PULLEVENTINPUTSTREAM_HPP
#define	__PULLEVENTINPUTSTREAM_HPP

#include <dbxml/XmlInputStream.hpp>
#include "NsEvent.hpp"
#include "NsWriter.hpp"

namespace DbXml
{

/**
 * Takes an NsPullEventSource8, and turns it into an XmlInputStream,
 * using NsWriter to write out the XML.
 */
	class PullEventInputStream : public XmlInputStream, private NsStream8
{
public:
	PullEventInputStream(NsPullEventSource8 *events);
	virtual ~PullEventInputStream();

	virtual unsigned int curPos() const;
	virtual unsigned int readBytes(char *toFill, 
		const unsigned int maxToRead);

private:
	virtual void write(const xmlbyte_t *data);
	virtual void write(const xmlbyte_t *data, uint32_t len);

private:
	NsPullEventSource8 *events_;
	NsWriter writer_;
	unsigned int cursorPos_;

	Buffer buffer_;
	size_t bufferOffset_;

	char *toFill_;
	unsigned int bytesLeft_;
};

}

#endif
