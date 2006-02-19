/**********************************************************************
 * $Id: geosAlgorithm.h,v 1.9.2.1 2005/05/23 16:39:05 strk Exp $
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.refractions.net
 *
 * Copyright (C) 2001-2002 Vivid Solutions Inc.
 * Copyright (C) 2005 Refractions Research Inc.
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#ifndef GEOS_ALGORITHM_H
#define GEOS_ALGORITHM_H

#include <memory>
#include <geos/geom.h>
#include <geos/util.h>
#include <geos/platform.h>
#include <geos/indexBintree.h>
#include <geos/indexStrtree.h>
#include <geos/indexStrtree.h>
#include <geos/indexChain.h>

namespace geos {

class Coordinate;

/*
 * \class NotRepresentableException geosAlgorithm.h geos/geosAlgorithm.h
 * \brief
 * Indicates that a HCoordinate has been computed which is
 * not representable on the Cartesian plane.
 *
 * @version 1.4
 * @see HCoordinate
 */
class NotRepresentableException: public GEOSException {
public:
	NotRepresentableException();
	NotRepresentableException(string msg);
	~NotRepresentableException();
};

class PointInRing{
public:
	virtual ~PointInRing(){};
	virtual bool isInside(const Coordinate& pt)=0;
};

class CGAlgorithms {
public:
	enum {
		CLOCKWISE=-1,
		COLLINEAR,
		COUNTERCLOCKWISE
	};
	enum {
		RIGHT=-1,
		LEFT,
		STRAIGHT
	};
	CGAlgorithms(){};

	/**
	 * Test whether a point lies inside a ring.
	 * The ring may be oriented in either direction.
	 * If the point lies on the ring boundary the result
	 * of this method is unspecified.
	 * 
	 * This algorithm does not attempt to first check the
	 *point against the envelope of the ring.
	 *
	 * @param p point to check for ring inclusion
	 * @param ring assumed to have first point identical to last point
	 * @return <code>true</code> if p is inside ring
	 */
	static bool isPointInRing(const Coordinate& p, const CoordinateSequence* ring);
	/**
	* Test whether a point lies on a linestring.
	*
	* @return true true if
	* the point is a vertex of the line or lies in the interior of a line
	* segment in the linestring
	*/
	static bool isOnLine(const Coordinate& p, const CoordinateSequence* pt);

	/*
	 * Computes whether a ring defined by an array of Coordinate is
	 * oriented counter-clockwise.
	 * 
	 *  - The list of points is assumed to have the first and last
	 *    points equal.
	 *  - This will handle coordinate lists which contain repeated points.
	 *  - If the ring is invalid, the answer returned may not be correct.
	 * 
	 *
	 * @param ring an array of coordinates forming a ring
	 * @return <code>true</code> if the ring is oriented counter-clockwise.
	 */
	static bool isCCW(const CoordinateSequence* ring);

	/**
	* Computes the orientation of a point q to the directed line segment p1-p2.
	* The orientation of a point relative to a directed line segment indicates
	* which way you turn to get to q after travelling from p1 to p2.
	*
	* @return 1 if q is counter-clockwise from p1-p2
	* @return -1 if q is clockwise from p1-p2
	* @return 0 if q is collinear with p1-p2
	*/
	static int computeOrientation(const Coordinate& p1, const Coordinate& p2, const Coordinate& q);
	static double distancePointLine(const Coordinate& p,const Coordinate& A,const Coordinate& B);
	/**
	* Computes the perpendicular distance from a point p
	* to the (infinite) line containing the points AB
	*
	* @param p the point to compute the distance for
	* @param A one point of the line
	* @param B another point of the line (must be different to A)
	* @return the distance from p to line AB
	*/
	static double distancePointLinePerpendicular(const Coordinate& p,const Coordinate& A,const Coordinate& B);
	static double distanceLineLine(const Coordinate& A, const Coordinate& B, const Coordinate& C, const Coordinate& D);
	static double signedArea(const CoordinateSequence* ring);
	/**
	* Computes the length of a linestring specified by a sequence of points.
	*
	* @param pts the points specifying the linestring
	* @return the length of the linestring
	*/
	static double length(const CoordinateSequence* pts);
	/**
	* Returns the index of the direction of the point <code>q</code>
	* relative to a
	* vector specified by <code>p1-p2</code>.
	*
	* @param p1 the origin point of the vector
	* @param p2 the final point of the vector
	* @param q the point to compute the direction to
	*
	* @return 1 if q is counter-clockwise (left) from p1-p2
	* @return -1 if q is clockwise (right) from p1-p2
	* @return 0 if q is collinear with p1-p2
	*/
	static int orientationIndex(const Coordinate& p1,const Coordinate& p2,const Coordinate& q);

};

/// Represents a homogeneous coordinate for 2-D coordinates.
class HCoordinate {
public:
	static Coordinate* intersection(Coordinate& p1,Coordinate& p2,Coordinate& q1,Coordinate& q2);
	double x,y,w;
	HCoordinate();
	HCoordinate(double _x, double _y, double _w);
	HCoordinate(Coordinate& p);
	HCoordinate(HCoordinate p1, HCoordinate p2);
	double getX();
	double getY();
	Coordinate* getCoordinate();
};

class SimplePointInRing: public PointInRing {
public:
	SimplePointInRing(LinearRing *ring);
	virtual ~SimplePointInRing();
	bool isInside(const Coordinate& pt);
private:
	const CoordinateSequence* pts;
};

class LineIntersector{
public:	
	// Return a Z value being the interpolation of Z from p0 and p1 at
	// the given point p
	static double interpolateZ(const Coordinate &p, const Coordinate &p0, const Coordinate &p1);
	static double computeEdgeDistance(const Coordinate& p, const Coordinate& p0, const Coordinate& p1);
	static double nonRobustComputeEdgeDistance(const Coordinate& p,const Coordinate& p1,const Coordinate& p2);
	LineIntersector();
	virtual ~LineIntersector();

	/*
	 * Tests whether either intersection point is an interior point of
	 * one of the input segments.
	 *
	 * @return <code>true</code> if either intersection point is in
	 * the interior of one of the input segments
	 */
	virtual bool isInteriorIntersection();

	/*
	 * Tests whether either intersection point is an interior point
	 * of the specified input segment.
	 *
	 * @return <code>true</code> if either intersection point is in
	 * the interior of the input segment
	 */
	virtual bool isInteriorIntersection(int inputLineIndex);

	virtual void setMakePrecise(const PrecisionModel *newPM);

	virtual void setPrecisionModel(const PrecisionModel *newPM);

	/*
	 * Compute the intersection of a point p and the line p1-p2.
	 * This function computes the boolean value of the hasIntersection test.
	 * The actual value of the intersection (if there is one)
	 * is equal to the value of <code>p</code>.
	 */
	virtual void computeIntersection(const Coordinate& p,const Coordinate& p1,const Coordinate& p2)=0;

	enum {
		DONT_INTERSECT,
		DO_INTERSECT,
		COLLINEAR
	};

	virtual void computeIntersection(const Coordinate& p1,const Coordinate& p2,const Coordinate& p3, const Coordinate& p4);

	virtual string toString() const;

	virtual bool hasIntersection() const;

	virtual int getIntersectionNum() const;

	virtual const Coordinate& getIntersection(int intIndex) const;

	static bool isSameSignAndNonZero(double a,double b);

	virtual bool isIntersection(const Coordinate& pt) const;
	virtual bool isProper() const;
	virtual const Coordinate& getIntersectionAlongSegment(int segmentIndex,int intIndex);
	virtual int getIndexAlongSegment(int segmentIndex,int intIndex);
	virtual double getEdgeDistance(int geomIndex,int intIndex) const;
protected:
	/**
	* If makePrecise is true, computed intersection coordinates will be made precise
	* using Coordinate#makePrecise
	*/
	const PrecisionModel *precisionModel;
	int result;
	Coordinate inputLines[2][2];
	Coordinate intPt[2];
	/**
	* The indexes of the endpoints of the intersection lines, in order along
	* the corresponding line
	*/
	int intLineIndex[2][2];
	bool isProperVar;
	Coordinate pa;
	Coordinate pb;
	virtual bool isCollinear() const;
	virtual int computeIntersect(const Coordinate& p1,const Coordinate& p2,const Coordinate& q1,const Coordinate& q2)=0;
	virtual bool isEndPoint() const;
	virtual void computeIntLineIndex();
	virtual void computeIntLineIndex(int segmentIndex);
};

class RobustDeterminant {
public:
	static int signOfDet2x2(double x1,double y1,double x2,double y2);
};

class RobustLineIntersector: public LineIntersector {
public:
	RobustLineIntersector();
	virtual ~RobustLineIntersector();
	void computeIntersection(const Coordinate& p,const Coordinate& p1,const Coordinate& p2);
	int computeIntersect(const Coordinate& p1,const Coordinate& p2,const Coordinate& q1,const Coordinate& q2);
private:
//	bool between(Coordinate& p1,Coordinate& p2,Coordinate& q);
	int computeCollinearIntersection(const Coordinate& p1,const Coordinate& p2,const Coordinate& q1,const Coordinate& q2);
	Coordinate* intersection(const Coordinate& p1,const Coordinate& p2,const Coordinate& q1,const Coordinate& q2) const;
	void normalize(Coordinate *n1,Coordinate *n2,Coordinate *n3,Coordinate *n4,Coordinate *normPt) const;
	double smallestInAbsValue(double x1,double x2,double x3,double x4) const;
	/**
	* Test whether a point lies in the envelopes of both input segments.
	* A correctly computed intersection point should return <code>true</code>
	* for this test.
	* Since this test is for debugging purposes only, no attempt is
	* made to optimize the envelope test.
	*
	* @return <code>true</code> if the input point lies within both input segment envelopes
	*/
	bool isInSegmentEnvelopes(const Coordinate& intPt);
};

class NonRobustLineIntersector: public LineIntersector {
public:
	NonRobustLineIntersector();
	void computeIntersection(const Coordinate& p,const Coordinate& p1,const Coordinate& p2);
protected:
	int computeIntersect(const Coordinate& p1,const Coordinate& p2,const Coordinate& p3,const Coordinate& p4);
private:
	int computeCollinearIntersection(const Coordinate& p1,const Coordinate& p2,const Coordinate& p3,const Coordinate& p4);
	double rParameter(const Coordinate& p1,const Coordinate& p2,const Coordinate& p) const;
};

/*
 * Stub version of RobustCGAlgorithms for backwards compatibility.
 * Will be deprecated in next release - use CGAlgorithms instead.
 */
class RobustCGAlgorithms: public CGAlgorithms {
};

class NonRobustCGAlgorithms: public CGAlgorithms {
public:
	NonRobustCGAlgorithms();
	~NonRobustCGAlgorithms();
	/**
	* Computes whether a ring defined by an array of {@link Coordinate} is
	* oriented counter-clockwise.
	* <p>
	* This will handle coordinate lists which contain repeated points.
	*
	* @param ring an array of coordinates forming a ring
	* @return <code>true</code> if the ring is oriented counter-clockwise.
	*/
	static bool isPointInRing(const Coordinate& p, const CoordinateSequence* ring);
//	static bool isOnLine(const Coordinate& p, const CoordinateSequence* pt) const;
	/**
	* Computes whether a ring defined by an array of {@link Coordinate} is
	* oriented counter-clockwise.
	* <p>
	* This will handle coordinate lists which contain repeated points.
	*
	* @param ring an array of coordinates forming a ring
	* @return <code>true</code> if the ring is oriented counter-clockwise.
	* @throws IllegalArgumentException if the ring is degenerate (does not contain 3 different points)
	*/
	static bool isCCW(const CoordinateSequence* ring);
	static int computeOrientation(const Coordinate& p1,const Coordinate& p2,const Coordinate& q);
};

class SimplePointInAreaLocator {
public:
	static int locate(const Coordinate& p, const Geometry *geom);
	static bool containsPointInPolygon(const Coordinate& p,const Polygon *poly);
private:
	static bool containsPoint(const Coordinate& p,const Geometry *geom);
};

/*
 * \class PointLocator geosAlgorithm.h geos/geosAlgorithm.h
 *
 * \brief
 * Computes the topological relationship (Location)
 * of a single point to a Geometry.
 *
 * The algorithm obeys the SFS boundaryDetermination rule to correctly determine
 * whether the point lies on the boundary or not.
 * Note that instances of this class are not reentrant.
 * @version 1.3
 */
class PointLocator {
public:
	PointLocator();
	~PointLocator();
	int locate(const Coordinate& p,const Geometry *geom);
	bool intersects(const Coordinate& p,const Geometry *geom);
	int locate(const Coordinate& p,const LineString *l);
	int locate(const Coordinate& p,const LinearRing *ring);
	int locate(const Coordinate& p,const Polygon *poly);
private:
	bool isIn;         // true if the point lies in or on any Geometry element
	int numBoundaries;    // the number of sub-elements whose boundaries the point lies in
	void computeLocation(const Coordinate& p,const Geometry *geom);
	void updateLocationInfo(int loc);
};


class MCPointInRing: public PointInRing {
public:
	MCPointInRing(LinearRing *newRing);
	virtual ~MCPointInRing();
	bool isInside(const Coordinate& pt);
	void testLineSegment(Coordinate& p,LineSegment *seg);
	class MCSelecter: public MonotoneChainSelectAction {
	private:
		Coordinate p;
		MCPointInRing *parent;
	public:
		MCSelecter(const Coordinate& newP,MCPointInRing *prt);
	    void select(LineSegment *ls);
	};
private:
	LinearRing *ring;
	BinTreeInterval *interval;
	CoordinateSequence *pts;
	Bintree *tree;
	int crossings;  // number of segment/ray crossings
	void buildIndex();
	void testMonotoneChain(Envelope *rayEnv,MCSelecter *mcSelecter,indexMonotoneChain *mc);
};

class SIRtreePointInRing: public PointInRing {
private:
	LinearRing *ring;
	SIRtree *sirTree;
	int crossings;  // number of segment/ray crossings
	void buildIndex();
	void testLineSegment(const Coordinate& p,LineSegment *seg);
public:
	SIRtreePointInRing(LinearRing *newRing);
	bool isInside(const Coordinate& pt);
};

class CentroidPoint {
private:
	int ptCount;
	Coordinate* centSum;
public:
	CentroidPoint();
	virtual ~CentroidPoint();
	void add(const Geometry *geom);
	void add(const Coordinate *pt);
	Coordinate* getCentroid() const;
};

class CentroidLine {
private:
	Coordinate* centSum;
	double totalLength;
public:
	CentroidLine();
	virtual ~CentroidLine();
	void add(const Geometry *geom);
	void add(const CoordinateSequence *pts);
	Coordinate* getCentroid() const;
};

/*
 * \class CentroidArea geosAlgorithm.h geos/geosAlgorithm.h
 *
 * \brief Computes the centroid of an area geometry.
 *
 * Algorithm:
 *
 * Based on the usual algorithm for calculating
 * the centroid as a weighted sum of the centroids
 * of a decomposition of the area into (possibly overlapping) triangles.
 * The algorithm has been extended to handle holes and multi-polygons.
 * See <code>http://www.faqs.org/faqs/graphics/algorithms-faq/</code>
 * for further details of the basic approach.
 */
class CentroidArea {
public:
	CentroidArea();
	virtual ~CentroidArea();
	void add(const Geometry *geom);
	void add(const CoordinateSequence *ring);
	Coordinate* getCentroid() const;
private:
	CGAlgorithms *cga;
	Coordinate* basePt;// the point all triangles are based at
	Coordinate* triangleCent3;// temporary variable to hold centroid of triangle
	double areasum2;        /* Partial area sum */
	Coordinate* cg3; // partial centroid sum
	void setBasePoint(const Coordinate *newbasePt);
	void add(const Polygon *poly);
	void addShell(const CoordinateSequence *pts);
	void addHole(const CoordinateSequence *pts);
	inline void addTriangle(const Coordinate &p0, const Coordinate &p1, const Coordinate &p2,bool isPositiveArea);
	static inline  void centroid3(const Coordinate &p1,const Coordinate &p2,const Coordinate &p3,Coordinate *c);
	static inline double area2(const Coordinate &p1,const Coordinate &p2,const Coordinate &p3);
};

/*
 * \class InteriorPointPoint geosAlgorithm.h geos/geosAlgorithm.h
 * \brief
 * Computes a point in the interior of an point geometry.
 *
 * Algorithm:
 *
 * Find a point which is closest to the centroid of the geometry.
 */
class InteriorPointPoint {
private:
	const Coordinate* centroid;
	double minDistance;
	Coordinate *interiorPoint;
	void add(const Geometry *geom);
	void add(const Coordinate *point);
public:
	InteriorPointPoint(const Geometry *g);
	virtual	~InteriorPointPoint();
	Coordinate* getInteriorPoint() const;
};

/*
 * Computes a point in the interior of an linear geometry.
 * <h2>Algorithm</h2>
 * <ul>
 * <li>Find an interior vertex which is closest to
 * the centroid of the linestring.
 * <li>If there is no interior vertex, find the endpoint which is
 * closest to the centroid.
 * </ul>
 */
class InteriorPointLine {
public:
	InteriorPointLine(Geometry *g);
	virtual ~InteriorPointLine();
	Coordinate* getInteriorPoint() const;
private:
	const Coordinate *centroid;
	double minDistance;
	Coordinate *interiorPoint;
	void addInterior(const Geometry *geom);
	void addInterior(const CoordinateSequence *pts);
	void addEndpoints(const Geometry *geom);
	void addEndpoints(const CoordinateSequence *pts);
	void add(const Coordinate *point);
};

/*
 * Computes a point in the interior of an area geometry.
 *
 * <h2>Algorithm</h2>
 * <ul>
 *   <li>Find the intersections between the geometry
 *       and the horizontal bisector of the area's envelope
 *   <li>Pick the midpoint of the largest intersection (the intersections
 *       will be lines and points)
 * </ul>
 *
 * <b>
 * Note: If a fixed precision model is used,
 * in some cases this method may return a point
 * which does not lie in the interior.
 * </b>
 */
class InteriorPointArea {
private:
	static double avg(double a, double b);
	const GeometryFactory *factory;
	Coordinate *interiorPoint;
	double maxWidth;
	void add(const Geometry *geom);
public:
	InteriorPointArea(const Geometry *g);
	virtual ~InteriorPointArea();
	Coordinate* getInteriorPoint() const;
	void addPolygon(const Geometry *geometry);
	Coordinate* centre(const Envelope *envelope) const;
protected:
	const Geometry *widestGeometry(const Geometry *geometry);
	const Geometry *widestGeometry(const GeometryCollection *gc);
	LineString *horizontalBisector(const Geometry *geometry);

};

class BigQuad {
public:
	Coordinate northmost;
	Coordinate southmost;
	Coordinate westmost;
	Coordinate eastmost;
};

class ConvexHull {
private:
	PointLocator *pointLocator;
	//CGAlgorithms *cgAlgorithms;
	const Geometry *geometry;
	const GeometryFactory *factory;
	CoordinateSequence* reduce(const CoordinateSequence *pts);
	CoordinateSequence* preSort(CoordinateSequence *pts);
	CoordinateSequence* grahamScan(const CoordinateSequence *c);
	void radialSort(CoordinateSequence *p);
	int polarCompare(Coordinate o, Coordinate p, Coordinate q);
	bool isBetween(Coordinate c1, Coordinate c2, Coordinate c3);
    BigQuad* makeBigQuad(const CoordinateSequence *pts);
	Geometry* lineOrPolygon(CoordinateSequence *newCoordinates);
	CoordinateSequence* cleanRing(CoordinateSequence *original);
public:
	ConvexHull(const Geometry *newGeometry);
	~ConvexHull();
	Geometry* getConvexHull();
};


/*
 * Computes the minimum diameter of a {@link Geometry}.
 * The minimum diameter is defined to be the
 * width of the smallest band that
 * contains the geometry,
 * where a band is a strip of the plane defined
 * by two parallel lines.
 * This can be thought of as the smallest hole that the geometry can be
 * moved through, with a single rotation.
 * <p>
 * The first step in the algorithm is computing the convex hull of the Geometry.
 * If the input Geometry is known to be convex, a hint can be supplied to
 * avoid this computation.
 *
 * @see ConvexHull
 *
 */
class MinimumDiameter {
private:
	const Geometry* inputGeom;
	bool isConvex;
	LineSegment* minBaseSeg;
	Coordinate* minWidthPt;
	int minPtIndex;
	double minWidth;
	void computeMinimumDiameter();
	void computeWidthConvex(const Geometry* geom);
	/**
	* Compute the width information for a ring of {@link Coordinate}s.
	* Leaves the width information in the instance variables.
	*
	* @param pts
	* @return
	*/
	void computeConvexRingMinDiameter(const CoordinateSequence *pts);
	int findMaxPerpDistance(const CoordinateSequence* pts, LineSegment* seg, int startIndex);
	static int getNextIndex(const CoordinateSequence* pts, int index);
public:
	~MinimumDiameter();

	/**
	* Compute a minimum diameter for a giver {@link Geometry}.
	*
	* @param geom a Geometry
	*/
	MinimumDiameter(const Geometry* newInputGeom);

	/**
	* Compute a minimum diameter for a giver {@link Geometry},
	* with a hint if
	* the Geometry is convex
	* (e.g. a convex Polygon or LinearRing,
	* or a two-point LineString, or a Point).
	*
	* @param geom a Geometry which is convex
	* @param isConvex <code>true</code> if the input geometry is convex
	*/
	MinimumDiameter(const Geometry* newInputGeom,const bool newIsConvex);

	/**
	* Gets the length of the minimum diameter of the input Geometry
	*
	* @return the length of the minimum diameter
	*/
	double getLength();

	/**
	* Gets the {@link Coordinate} forming one end of the minimum diameter
	*
	* @return a coordinate forming one end of the minimum diameter
	*/
	Coordinate* getWidthCoordinate();

	/**
	* Gets the segment forming the base of the minimum diameter
	*
	* @return the segment forming the base of the minimum diameter
	*/
	LineString* getSupportingSegment();

	/**
	* Gets a {@link LineString} which is a minimum diameter
	*
	* @return a {@link LineString} which is a minimum diameter
	*/
	LineString* getDiameter();
};

} // namespace geos

#endif

/**********************************************************************
 * $Log: geosAlgorithm.h,v $
 * Revision 1.9.2.1  2005/05/23 16:39:05  strk
 * log lines moved at bottom file, added Refractions copyright
 *
 * Revision 1.9  2004/11/23 19:53:07  strk
 * Had LineIntersector compute Z by interpolation.
 *
 * Revision 1.8  2004/11/06 08:16:46  strk
 * Fixed CGAlgorithms::isCCW from JTS port.
 * Code cleanup in IsValidOp.
 *
 * Revision 1.7  2004/10/21 22:29:54  strk
 * Indentation changes and some more COMPUTE_Z rules
 *
 * Revision 1.6  2004/09/13 10:12:49  strk
 * Added invalid coordinates checks in IsValidOp.
 * Cleanups.
 *
 **********************************************************************/

