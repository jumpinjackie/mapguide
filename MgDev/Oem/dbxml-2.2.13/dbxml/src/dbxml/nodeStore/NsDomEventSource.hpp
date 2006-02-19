//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//      Sleepycat Software.  All rights reserved.
//
// $Id: NsDomEventSource.hpp,v 1.5 2005/05/26 12:40:31 jsnelson Exp $
//

#ifndef __DBXMLNSDOMEVENTSOURCE_HPP
#define __DBXMLNSDOMEVENTSOURCE_HPP
	
#include "NsEvent.hpp"

namespace DbXml
{

class NsDomElement;
class NsDomEventNodeList;

/**
 * NsDomEventSource8
 *
 */
class NsDomEventSource8 : public NsPullEventSource8 {
public:
	NsDomEventSource8(NsDomElement *node);
	virtual ~NsDomEventSource8();

	virtual bool nextEvent(NsEventHandler8 *handler);
private:
	bool startEvent(NsEventHandler8 *handler);
	bool endEvent(NsEventHandler8 *handler);
	bool xmlDeclEvent(NsEventHandler8 *handler);
	bool childEvent(NsEventHandler8 *handler);	

private:
	NsDomEventNodeList *current_;
};

}

#endif
