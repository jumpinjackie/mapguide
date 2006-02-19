/**********************************************************************
 * $Id: ParseException.cpp,v 1.9.2.1 2005/05/23 18:41:51 strk Exp $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************
 * $Log: ParseException.cpp,v $
 * Revision 1.9.2.1  2005/05/23 18:41:51  strk
 * Replaced sprintf uses with ostringstream
 *
 * Revision 1.9  2004/07/02 13:28:27  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.8  2004/05/17 12:36:39  strk
 * ParseException message made more readable
 *
 * Revision 1.7  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/


#include <sstream>
#include <geos/io.h>

namespace geos {

ParseException::ParseException(){
	GEOSException();
	setName("ParseException");
}

ParseException::ParseException(string msg){
	setName("ParseException");
	setMessage(msg);
}
ParseException::ParseException(string msg, string var){
	setName("ParseException");
	setMessage(msg+": '"+var+"'");
}
ParseException::ParseException(string msg, double num){
	setName("ParseException");
	ostringstream s;
	s<<msg<<": "<<num;
	setMessage(s.str());
}

ParseException::~ParseException(){}

} // namespace geos

