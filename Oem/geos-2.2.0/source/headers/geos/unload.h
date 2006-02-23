/**********************************************************************
 * $Id: unload.h,v 1.1 2004/07/02 13:20:42 strk Exp $
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
 * $Log: unload.h,v $
 * Revision 1.1  2004/07/02 13:20:42  strk
 * Header files moved under geos/ dir.
 *
 * Revision 1.2  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/


#ifndef GEOS_UNLOAD_H
#define GEOS_UNLOAD_H
//xie add for realse static memory 2003,10,06
namespace geos {

class Unload
{
public:
	Unload(void);
	~Unload(void);
	static void Release();
};

}
#endif
