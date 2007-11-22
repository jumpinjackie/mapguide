//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: EventReaderToWriter.hpp,v 1.3 2006/10/30 17:45:57 bostic Exp $
//

#ifndef __EVENTREADERTOWRITER_HPP
#define __EVENTREADERTOWRITER_HPP

#include "NsEvent.hpp"
#include "EventWriter.hpp"
#include "../ReferenceCounted.hpp"
/*
 * EventReaderToWriter
 *
 * Internal implementation of XmlEventReaderToWriter that
 * knows about internal interfaces
 */
namespace DbXml
{

class XmlEventReader;
class XmlEventWriter;
class EventReader;
class EventWriter;
	
class EventReaderToWriter : public NsPullEventSource, public NsPushEventSource, public ReferenceCounted
{
public:
	EventReaderToWriter(XmlEventReader &reader,
			    XmlEventWriter &writer,
			    bool ownsReader);
	EventReaderToWriter(EventReader &reader,
			    EventWriter &writer,
			    bool ownsReader);
	EventReaderToWriter(XmlEventReader &reader,
			    EventWriter &writer,
			    bool ownsReader);
	EventReaderToWriter(EventReader &reader,
			    XmlEventWriter &writer,
			    bool ownsReader);
	EventReaderToWriter(XmlEventReader &reader,
			    bool ownsReader,
			    bool isInternal);
	~EventReaderToWriter();
	XmlEventWriter *getWriter() const {
		return writer_;
	}
	void reset();

	// NsPushEventSource
	virtual void start();
	virtual EventWriter *getEventWriter() const {
		DBXML_ASSERT(isInternalWriter_);
		return (EventWriter *)writer_;
	}
	virtual void setEventWriter(EventWriter *writer) {
		writer_ = writer;
		isInternalWriter_ = true;
	}
	
	// NsPullEventSource
	virtual bool nextEvent(EventWriter *writer);
private:
	void doEvent(XmlEventWriter *writer, bool isInternal);
	void doStartElement(EventWriter *writer);
private:
	XmlEventReader &reader_;
	XmlEventWriter *writer_;
	bool ownsReader_;
	bool isInternalReader_;
	bool isInternalWriter_;
};
}
	
#endif

