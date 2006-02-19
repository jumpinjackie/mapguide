/**********************************************************************
 * $Id: XMLTester.h,v 1.1.2.1 2005/06/21 12:22:29 strk Exp $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifndef __XMLTESTER_H__
#define __XMLTESTER_H__

using namespace geos;

class XMLTester {

public:
	XMLTester(const char *);
	~XMLTester();
	void run();
	int failed;
	int succeeded;
	int caseCount;
	int testCount;

private:
	enum {
		TEST_DESCR=1,
		GEOM_A_IN=2,
		GEOM_A_OUT=4,
		GEOM_B_IN=8,
		GEOM_B_OUT=16,
		TEST_OP=32,
		TEST_RESULT=64,
		PRED=128
	};

	void parsePrecisionModel();
	void parseCase();
	void parseTest();

	Geometry *gA;
	Geometry *gB;
	Geometry *gT;
	PrecisionModel *pm;

	GeometryFactory *factory;
	WKTReader *r;
	WKTWriter *w;
	CMarkupSTL xml;

	int out;

};


#endif 
