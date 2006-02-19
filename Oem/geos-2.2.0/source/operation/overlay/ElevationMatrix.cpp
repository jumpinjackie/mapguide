/**********************************************************************
 * $Id: ElevationMatrix.cpp,v 1.5 2004/12/08 13:54:44 strk Exp $
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
 **********************************************************************/

#include <sstream>
#include <geos/geom.h>
#include <geos/opOverlay.h>

#define DEBUG 0
#define ASSERT 1

namespace geos
{

ElevationMatrix::ElevationMatrix(const Envelope &newEnv,
		unsigned int newRows, unsigned int newCols):
	env(newEnv), cols(newCols), rows(newRows),
	avgElevationComputed(false),
	avgElevation(DoubleNotANumber),
 	cells(newRows*newCols)
{
	cellwidth=env.getWidth()/cols;
	cellheight=env.getHeight()/rows;
	if ( ! cellwidth ) cols=1;
	if ( ! cellheight ) rows=1;
}

ElevationMatrix::~ElevationMatrix()
{
}

void
ElevationMatrix::add(const Geometry *geom)
{
#if DEBUG
	cerr<<"ElevationMatrix::add(Geometry *) called"<<endl;
#endif // DEBUG

#if ASSERT
	Assert::isTrue(!avgElevationComputed, "Cannot add Geometries to an ElevationMatrix after it's average elevation has been computed");
#endif

	// Todo: optimize so to not require a CoordinateSequence copy
	CoordinateSequence *cs = geom->getCoordinates();
	add(cs);
	delete cs;
}

void
ElevationMatrix::add(const CoordinateSequence *cs)
{
	unsigned int ncoords = cs->getSize();
	for (unsigned int i=0; i<ncoords; i++)
	{
		add(cs->getAt(i));
	}
}

void
ElevationMatrix::add(const Coordinate &c)
{
	if ( ISNAN(c.z) ) return;
	try {
		ElevationMatrixCell &emc = getCell(c);
		emc.add(c);
	} catch (IllegalArgumentException *exp) {
		// coordinate do not overlap matrix
		cerr<<"ElevationMatrix::add("<<c.toString()<<"): Coordinate does not overlap grid extent: "<<exp->toString()<<endl;
		delete exp;
		return;
	}
}

ElevationMatrixCell &
ElevationMatrix::getCell(const Coordinate &c) 
{
	int col, row;

	if ( ! cellwidth ) col=0;
	else
	{
		double xoffset = c.x - env.getMinX();
		col = (int)(xoffset/cellwidth);
		if ( col == (int)cols ) col = cols-1;
	}
	if ( ! cellheight ) row=0;
	else
	{
		double yoffset = c.y - env.getMinY();
		row = (int)(yoffset/cellheight);
		if ( row == (int)rows ) row = rows-1;
	}
	int celloffset = (cols*row)+col;

	if  (celloffset<0 || celloffset >= (int)(cols*rows))
	{
		ostringstream s;
		s<<"ElevationMatrix::getCell got a Coordinate out of grid extent ("<<env.toString()<<") - cols:"<<cols<<" rows:"<<rows;
		throw new IllegalArgumentException(s.str());
	}

	return cells[celloffset];
}

const ElevationMatrixCell &
ElevationMatrix::getCell(const Coordinate &c) const
{
	return (const ElevationMatrixCell &)
		((ElevationMatrix *)this)->getCell(c);
}

double
ElevationMatrix::getAvgElevation() const
{
	if ( avgElevationComputed ) return avgElevation;
	double ztot=0;
	int zvals=0;
	for (unsigned int r=0; r<rows; r++)
	{
		for(unsigned int c=0; c<cols; c++)
		{
			const ElevationMatrixCell &cell = cells[(r*cols)+c];
			double e = cell.getAvg();
			if ( !ISNAN(e) )
			{
				zvals++;
				ztot+=e;
			}
		}
	}
	if ( zvals ) avgElevation = ztot/zvals;
	return avgElevation;
}

string
ElevationMatrix::print() const
{
	ostringstream ret;
	ret << "Cols:"<<cols<<" Rows:"<<rows<<" AvgElevation:"<<getAvgElevation()<<endl;
	for (unsigned int r=0; r<rows; r++)
	{
		for (unsigned int c=0; c<cols; c++)
		{
			ret << cells[(r*cols)+c].print() << '\t';
		}
		ret << endl;
	}
	return ret.str();
}

void
ElevationMatrix::elevate(Geometry *g) const
{
	ElevationMatrixFilter filter(this);
	g->apply_rw(&filter);
}

} // namespace geos;

/**********************************************************************
 * $Log: ElevationMatrix.cpp,v $
 * Revision 1.5  2004/12/08 13:54:44  strk
 * gcc warnings checked and fixed, general cleanups.
 *
 * Revision 1.4  2004/11/29 16:05:33  strk
 * Fixed a bug in LineIntersector::interpolateZ causing NaN values
 * to come out.
 * Handled dimensional collapses in ElevationMatrix.
 * Added ISNAN macro and changed ISNAN/FINITE macros to avoid
 * dispendious isnan() and finite() calls.
 *
 * Revision 1.3  2004/11/26 09:22:50  strk
 * Added FINITE(x) macro and its use.
 * Made input geoms average Z computation optional in OverlayOp.
 *
 * Revision 1.2  2004/11/24 12:29:36  strk
 * Handled boundary cases in ::getCell
 *
 * Revision 1.1  2004/11/23 16:22:49  strk
 * Added ElevationMatrix class and components to do post-processing draping of overlayed geometries.
 *
 **********************************************************************/
