//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
//   $Id: PullEventInputStream.hpp,v 1.4 2006/10/30 17:45:58 bostic Exp $
//

#ifndef __PULLEVENTINPUTSTREAM_HPP
#define	__PULLEVENTINPUTSTREAM_HPP

#include <dbxml/XmlInputStream.hpp>
#include "NsEvent.hpp"
#include "NsWriter.hpp"

namespace DbXml
{

/**
 * Takes an NsPullEventSource, and turns it into an XmlInputStream,
 * using NsWriter to write out the XML.
 */
class PullEventInputStream : public XmlInputStream, private NsStream
{
public:
	PullEventInputStream(NsPullEventSource *events);
	virtual ~PullEventInputStream();

	virtual unsigned int curPos() const;
	virtual unsigned int readBytes(char *toFill, 
		const unsigned int maxToRead);

private:
	virtual void write(const xmlbyte_t *data);
	virtual void write(const xmlbyte_t *data, uint32_t len);

private:
	NsPullEventSource *events_;
	NsWriter writer_;
	unsigned int cursorPos_;

	Buffer buffer_;
	size_t bufferOffset_;

	char *toFill_;
	unsigned int bytesLeft_;
};

}

#endif
