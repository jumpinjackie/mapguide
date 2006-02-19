/**********************************************************************
 * $Id: EdgeRing.cpp,v 1.6.2.1 2005/11/17 10:49:54 strk Exp $
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

#include <geos/geomgraph.h>
#include <geos/util.h>

namespace geos {

EdgeRing::EdgeRing(DirectedEdge *newStart,
		const GeometryFactory *newGeometryFactory,
		CGAlgorithms *newCga):
        startDe(NULL),
        geometryFactory(newGeometryFactory),
	cga(newCga),
        holes(new vector<EdgeRing *>()),
        maxNodeDegree(-1),
        edges(new vector<DirectedEdge*>()),
        pts(newGeometryFactory->getCoordinateSequenceFactory()->create(NULL)),
        label(new Label(Location::UNDEF)),
        ring(NULL),
        isHoleVar(false),
        shell(NULL)
{
}

EdgeRing::~EdgeRing(){
	delete edges;

	/*
	 * If we constructed a ring, we did by transferring
	 * ownership of the CoordinateSequence, so it will be
	 * destroyed by `ring' dtor and we must not destroy
	 * it twice.
	 */
	if ( ring == NULL ) delete pts;
	else delete ring;

	delete label;
//	delete shell;
	for(int i=0;i<(int)holes->size();i++) {
		delete (*holes)[i];
	}
	delete holes;
}

bool EdgeRing::isIsolated(){
	return (label->getGeometryCount()==1);
}

bool EdgeRing::isHole(){
	return isHoleVar;
}

const Coordinate& EdgeRing::getCoordinate(int i) {
	return pts->getAt(i);
}

LinearRing* EdgeRing::getLinearRing() {
	return new LinearRing(*ring);
//	return ring;
}

Label* EdgeRing::getLabel() {
	return label;
}

bool EdgeRing::isShell(){
	return shell==NULL;
}

EdgeRing* EdgeRing::getShell() {
	return shell;
}

void EdgeRing::setShell(EdgeRing *newShell) {
	shell=newShell;
	if (shell!=NULL) shell->addHole(this);
}

void EdgeRing::addHole(EdgeRing *edgeRing) {
	holes->push_back(edgeRing);
}

Polygon* EdgeRing::toPolygon(const GeometryFactory* geometryFactory){
	vector<Geometry *> *holeLR=new vector<Geometry *>();
	for (unsigned int i=0;i<holes->size();i++) {
        holeLR->push_back((*holes)[i]->getLinearRing());
//        holeLR->push_back((*holes)[i]->ring);
	}
	return geometryFactory->createPolygon(getLinearRing(),holeLR);
}

void EdgeRing::computeRing() {
	if (ring!=NULL) return;   // don't compute more than once
	ring=geometryFactory->createLinearRing(pts);
	isHoleVar=cga->isCCW(pts);
}

  /**
 * Returns the list of DirectedEdges that make up this EdgeRing
 */
vector<DirectedEdge*>* EdgeRing::getEdges() {
	return edges;
}

void EdgeRing::computePoints(DirectedEdge *newStart){
	startDe=newStart;
	DirectedEdge *de=newStart;
	bool isFirstEdge=true;
	do {
		Assert::isTrue(de!=NULL,"EdgeRing::computePoints: found null Directed Edge");
		if (de->getEdgeRing()==this)
			throw new TopologyException("Directed Edge visited twice during ring-building at ",&(de->getCoordinate()));
		edges->push_back(de);
		Label *deLabel=de->getLabel();
		Assert::isTrue(deLabel->isArea());
		mergeLabel(deLabel);
		addPoints(de->getEdge(),de->isForward(),isFirstEdge);
		isFirstEdge=false;
		setEdgeRing(de,this);
		de=getNext(de);
	} while (de!=startDe);
}

int EdgeRing::getMaxNodeDegree(){
	if (maxNodeDegree<0) computeMaxNodeDegree();
	return maxNodeDegree;
}

void EdgeRing::computeMaxNodeDegree() {
	maxNodeDegree=0;
	DirectedEdge *de=startDe;
	do {
		Node *node=de->getNode();
		int degree=((DirectedEdgeStar*) node->getEdges())->getOutgoingDegree(this);
		if (degree>maxNodeDegree) maxNodeDegree=degree;
		de=getNext(de);
	} while (de!=startDe);
	maxNodeDegree *= 2;
}

void EdgeRing::setInResult(){
	DirectedEdge *de=startDe;
	do {
		(de->getEdge())->setInResult(true);
		de=de->getNext();
	} while (de!=startDe);
}

void EdgeRing::mergeLabel(Label *deLabel){
	mergeLabel(deLabel, 0);
	mergeLabel(deLabel, 1);
}

/**
 * Merge the RHS label from a DirectedEdge into the label for this EdgeRing.
 * The DirectedEdge label may be null.  This is acceptable - it results
 * from a node which is NOT an intersection node between the Geometries
 * (e.g. the end node of a LinearRing).  In this case the DirectedEdge label
 * does not contribute any information to the overall labelling, and is simply skipped.
 */
void EdgeRing::mergeLabel(Label *deLabel, int geomIndex){
	int loc=deLabel->getLocation(geomIndex,Position::RIGHT);
	// no information to be had from this label
	if (loc==Location::UNDEF) return;
	// if there is no current RHS value, set it
	if (label->getLocation(geomIndex)==Location::UNDEF) {
		label->setLocation(geomIndex,loc);
		return;
	}
}

void EdgeRing::addPoints(Edge *edge, bool isForward, bool isFirstEdge){
	const CoordinateSequence* edgePts=edge->getCoordinates();
	if (isForward) {
		int startIndex=1;
		if (isFirstEdge) startIndex=0;
		for (int i=startIndex; i<edgePts->getSize(); i++) {
			pts->add(edgePts->getAt(i));
		}
	} else { // is backward
		int startIndex=edgePts->getSize()-2;
		if (isFirstEdge) startIndex=edgePts->getSize()-1;
		for (int i=startIndex;i>=0;i--) {
			pts->add(edgePts->getAt(i));
		}
	}
}

/**
 * This method will cause the ring to be computed.
 * It will also check any holes, if they have been assigned.
 */
bool EdgeRing::containsPoint(Coordinate& p){
	const Envelope* env=ring->getEnvelopeInternal();
	return env->contains(p);
	//if (!env->contains(p))
	//{
	//	delete env;
	//	return false;
	//}
	//delete env;

//External Dependency
//	if (!cga.isPointInPolygon(p, shell.getCoordinates()) ) return false;
	for (vector<EdgeRing*>::iterator i=holes->begin();i<holes->end();i++) {
		EdgeRing *hole=*i;
		if (hole->containsPoint(p))
		{
			return false;
		}
	}
	return true;
}

} // namespace geos

/**********************************************************************
 * $Log: EdgeRing.cpp,v $
 * Revision 1.6.2.1  2005/11/17 10:49:54  strk
 * Removed a useless CoordinateSequence copy in EdgeRing.
 *
 * Revision 1.6  2004/07/27 16:35:46  strk
 * Geometry::getEnvelopeInternal() changed to return a const Envelope *.
 * This should reduce object copies as once computed the envelope of a
 * geometry remains the same.
 *
 * Revision 1.5  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added DefaultCoordinateSequenceFactory::instance() function.
 *
 * Revision 1.4  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.3  2004/07/01 14:12:44  strk
 *
 * Geometry constructors come now in two flavors:
 * 	- deep-copy args (pass-by-reference)
 * 	- take-ownership of args (pass-by-pointer)
 * Same functionality is available through GeometryFactory,
 * including buildGeometry().
 *
 * Revision 1.2  2004/06/30 20:59:12  strk
 * Removed GeoemtryFactory copy from geometry constructors.
 * Enforced const-correctness on GeometryFactory arguments.
 *
 * Revision 1.1  2004/03/19 09:48:45  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 * Revision 1.22  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 * Revision 1.21  2003/11/06 17:59:03  strk
 * Memory leaks fixed in ::containsPoint()
 *
 * Revision 1.20  2003/10/20 14:02:14  strk
 * more explicit exception thrown on null Directed Edge detection
 *
 * Revision 1.19  2003/10/15 16:39:03  strk
 * Made Edge::getCoordinates() return a 'const' value. Adapted code set.
 * $Log: EdgeRing.cpp,v $
 * Revision 1.6.2.1  2005/11/17 10:49:54  strk
 * Removed a useless CoordinateSequence copy in EdgeRing.
 *
 * Revision 1.6  2004/07/27 16:35:46  strk
 * Geometry::getEnvelopeInternal() changed to return a const Envelope *.
 * This should reduce object copies as once computed the envelope of a
 * geometry remains the same.
 *
 * Revision 1.5  2004/07/08 19:34:49  strk
 * Mirrored JTS interface of CoordinateSequence, factory and
 * default implementations.
 * Added DefaultCoordinateSequenceFactory::instance() function.
 *
 * Revision 1.4  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.3  2004/07/01 14:12:44  strk
 *
 * Geometry constructors come now in two flavors:
 * 	- deep-copy args (pass-by-reference)
 * 	- take-ownership of args (pass-by-pointer)
 * Same functionality is available through GeometryFactory,
 * including buildGeometry().
 *
 * Revision 1.2  2004/06/30 20:59:12  strk
 * Removed GeoemtryFactory copy from geometry constructors.
 * Enforced const-correctness on GeometryFactory arguments.
 *
 * Revision 1.1  2004/03/19 09:48:45  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 * Revision 1.22  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/

