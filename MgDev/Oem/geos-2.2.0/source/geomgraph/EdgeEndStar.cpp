/**********************************************************************
 * $Id: EdgeEndStar.cpp,v 1.9 2004/12/08 13:54:43 strk Exp $
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

#include <geos/geomgraph.h>
#include <geos/util.h>

#define DEBUG 0

namespace geos {

EdgeEndStar::EdgeEndStar(){
	ptInAreaLocation[0]=Location::UNDEF;
	ptInAreaLocation[1]=Location::UNDEF;
	edgeMap=new map<EdgeEnd*,void*,EdgeEndLT>();
	edgeList=NULL;
}

EdgeEndStar::~EdgeEndStar(){
//	map<EdgeEnd*,void*,EdgeEndLT>::iterator	it=edgeMap->begin();
//	for (;it!=edgeMap->end();it++) {
//		EdgeEnd *ee=(EdgeEnd*) it->second;
////		void *ee= it->second;
//		delete ee;
//	}
	delete edgeMap;
	delete edgeList;
}

void EdgeEndStar::insert(EdgeEnd *e){
	delete e;
}

/**
 * Insert an EdgeEnd into the map, and clear the edgeList cache,
 * since the list of edges has now changed
 */
void EdgeEndStar::insertEdgeEnd(EdgeEnd *e,void* obj){
	edgeMap->insert(pair<EdgeEnd*,void*>(e,obj));
//	(*edgeMap)[e]=obj;
	delete edgeList;
	edgeList=NULL;  // edge list has changed - clear the cache
}

/**
 * @return the coordinate for the node this star is based at
 */
Coordinate&
EdgeEndStar::getCoordinate()
{
	if (getEdges()->size()==0)
		return *(new Coordinate(DoubleNotANumber,DoubleNotANumber,DoubleNotANumber));
	vector<EdgeEnd*>::iterator it=getIterator();
	EdgeEnd *e=*it;
	return e->getCoordinate();
}

int EdgeEndStar::getDegree(){
	return (int)edgeMap->size();
}

vector<EdgeEnd*>*
EdgeEndStar::getEdges()
{
	map<EdgeEnd*,void*,EdgeEndLT>::iterator mapIter;
	if (edgeList==NULL) {
		edgeList=new vector<EdgeEnd*>();
		for(mapIter=edgeMap->begin();mapIter!=edgeMap->end();mapIter++) {
//			edgeList->push_back(mapIter->first);
			EdgeEnd *e=(EdgeEnd*) mapIter->second;
			edgeList->push_back(e);
		}
	}
//cout << endl << "edgeList:" << endl;
//for(int i=0;i<(int)edgeList->size();i++) {
//	cout << endl << (*edgeList)[i]->print() << endl;
//}
	return edgeList;
}

vector<EdgeEnd*>::iterator
EdgeEndStar::getIterator()
{
	return getEdges()->begin();
}

EdgeEnd*
EdgeEndStar::getNextCW(EdgeEnd *ee)
{
	getEdges();
	int i=0;
	for(unsigned int j=0;j<edgeList->size();j++)
	{
		//if (ee->compareTo( *(edgeList->at(j)))==0) 
		if (ee->compareTo((*edgeList)[j])==0)
		{
			i=j;
			break;
		}
	}
	int iNextCW=i-1;
	if (i==0) iNextCW=(int)edgeList->size()-1;
	return (*edgeList)[iNextCW];
}

void
EdgeEndStar::computeLabelling(vector<GeometryGraph*> *geom)
	//throw(TopologyException *)
{
	computeEdgeEndLabels();

	// Propagate side labels  around the edges in the star
	// for each parent Geometry
	//
	// these calls can throw a TopologyException
	propagateSideLabels(0);
	propagateSideLabels(1);

	/**
	 * If there are edges that still have null labels for a geometry
	 * this must be because there are no area edges for that geometry
	 * incident on this node.
	 * In this case, to label the edge for that geometry we must test
	 * whether the edge is in the interior of the geometry.
	 * To do this it suffices to determine whether the node for the
	 * edge is in the interior of an area.
	 * If so, the edge has location INTERIOR for the geometry.
	 * In all other cases (e.g. the node is on a line, on a point, or
	 * not on the geometry at all) the edge
	 * has the location EXTERIOR for the geometry.
	 * 
	 * Note that the edge cannot be on the BOUNDARY of the geometry,
	 * since then there would have been a parallel edge from the
	 * Geometry at this node also labelled BOUNDARY
	 * and this edge would have been labelled in the previous step.
	 *
	 * This code causes a problem when dimensional collapses are present,
	 * since it may try and determine the location of a node where a
	 * dimensional collapse has occurred.
	 * The point should be considered to be on the EXTERIOR
	 * of the polygon, but locate() will return INTERIOR, since it is
	 * passed the original Geometry, not the collapsed version.
	 *
	 * If there are incident edges which are Line edges labelled BOUNDARY,
	 * then they must be edges resulting from dimensional collapses.
	 * In this case the other edges can be labelled EXTERIOR for this
	 * Geometry.
	 *
	 * MD 8/11/01 - NOT TRUE!  The collapsed edges may in fact be in the
	 * interior of the Geometry, which means the other edges should be
	 * labelled INTERIOR for this Geometry.
	 * Not sure how solve this...  Possibly labelling needs to be split
	 * into several phases:
	 * area label propagation, symLabel merging, then finally null label
	 * resolution.
	 */
	bool hasDimensionalCollapseEdge[2]={false,false};
	vector<EdgeEnd*>::iterator it;
	for (it=getIterator();it<edgeList->end();it++) {
		EdgeEnd *e=*it;
		Label *label=e->getLabel();
		for(int geomi=0; geomi<2; geomi++)
		{
			if (label->isLine(geomi) && label->getLocation(geomi)==Location::BOUNDARY)
				hasDimensionalCollapseEdge[geomi]=true;
		}
	}
	for (it=getIterator();it<edgeList->end();it++) {
		EdgeEnd *e=*it;
		Label *label=e->getLabel();
		for(int geomi=0;geomi<2;geomi++){
			if (label->isAnyNull(geomi)) {
				int loc=Location::UNDEF;
				if (hasDimensionalCollapseEdge[geomi]){
					loc=Location::EXTERIOR;
				}else {
					Coordinate& p=e->getCoordinate();
					loc=getLocation(geomi,p,geom);
				}
				label->setAllLocationsIfNull(geomi,loc);
			}
		}
	}
}

void
EdgeEndStar::computeEdgeEndLabels()
{
	// Compute edge label for each EdgeEnd
	vector<EdgeEnd*>::iterator it;
	for (it=getIterator(); it<edgeList->end(); it++)
	{
		EdgeEnd *e=*it;
		e->computeLabel();
	}
}

int
EdgeEndStar::getLocation(int geomIndex,Coordinate& p,vector<GeometryGraph*> *geom)
{
	// compute location only on demand
	if (ptInAreaLocation[geomIndex]==Location::UNDEF)
	{
        	ptInAreaLocation[geomIndex]=SimplePointInAreaLocator::locate(p,(*geom)[geomIndex]->getGeometry());
	}
	return ptInAreaLocation[geomIndex];
}

bool EdgeEndStar::isAreaLabelsConsistent(){
	computeEdgeEndLabels();
	return checkAreaLabelsConsistent(0);
}

bool EdgeEndStar::checkAreaLabelsConsistent(int geomIndex){
	// Since edges are stored in CCW order around the node,
	// As we move around the ring we move from the right to the left side of the edge
	vector<EdgeEnd*> *edges=getEdges();
	// if no edges, trivially consistent
	if (edges->size()<=0)
		return true;
	// initialize startLoc to location of last L side (if any)
	int lastEdgeIndex=(int)edges->size()-1;
	Label *startLabel=((*edgeList)[lastEdgeIndex])->getLabel();
	int startLoc=startLabel->getLocation(geomIndex,Position::LEFT);
	Assert::isTrue(startLoc!=Location::UNDEF, "Found unlabelled area edge");
	int currLoc=startLoc;
	for (vector<EdgeEnd*>::iterator it=getIterator();it<edgeList->end();it++) {
		EdgeEnd *e=*it;
		Label *eLabel=e->getLabel();
		// we assume that we are only checking a area
		Assert::isTrue(eLabel->isArea(geomIndex), "Found non-area edge");
		int leftLoc=eLabel->getLocation(geomIndex,Position::LEFT);
		int rightLoc=eLabel->getLocation(geomIndex,Position::RIGHT);
		// check that edge is really a boundary between inside and outside!
		if (leftLoc==rightLoc) {
			return false;
		}
		// check side location conflict
		//Assert.isTrue(rightLoc == currLoc, "side location conflict " + locStr);
		if (rightLoc!=currLoc) {
			return false;
		}
		currLoc=leftLoc;
	}
	return true;
}

void
EdgeEndStar::propagateSideLabels(int geomIndex)
	//throw(TopologyException *)
{
	// Since edges are stored in CCW order around the node,
	// As we move around the ring we move from the right to the
	// left side of the edge
	int startLoc=Location::UNDEF ;

	// initialize loc to location of last L side (if any)
	vector<EdgeEnd*>::iterator it;
	for (it=getIterator();it<edgeList->end();it++)
	{
		EdgeEnd *e=*it;
		Label *label=e->getLabel();
		if (label->isArea(geomIndex) &&
			label->getLocation(geomIndex,Position::LEFT)!=Location::UNDEF)
			startLoc=label->getLocation(geomIndex,Position::LEFT);
	}
	// no labelled sides found, so no labels to propagate
	if (startLoc==Location::UNDEF) return;
	int currLoc=startLoc;
	for (it=getIterator();it<edgeList->end();it++) {
		EdgeEnd *e=*it;
		Label *label=e->getLabel();
		// set null ON values to be in current location
		if (label->getLocation(geomIndex,Position::ON)==Location::UNDEF)
			label->setLocation(geomIndex,Position::ON,currLoc);
		// set side labels (if any)
		// if (label.isArea())  //ORIGINAL
		if (label->isArea(geomIndex))
		{
			int leftLoc=label->getLocation(geomIndex,Position::LEFT);
			int rightLoc=label->getLocation(geomIndex,Position::RIGHT);
			// if there is a right location, that is the next location to propagate
			if (rightLoc!=Location::UNDEF) {
				if (rightLoc!=currLoc)
					throw new TopologyException("side location conflict",&(e->getCoordinate()));
				if (leftLoc==Location::UNDEF) {
					Assert::shouldNeverReachHere("found single null side (at " + (e->getCoordinate()).toString() + ")");
				}
				currLoc=leftLoc;
			} else {
				/**
				 * RHS is null - LHS must be null too.
				 * This must be an edge from the other
				 * geometry, which has no location
				 * labelling for this geometry.
				 * This edge must lie wholly inside or
				 * outside the other geometry (which is
				 * determined by the current location).
				 * Assign both sides to be the current
				 * location.
				 */
				Assert::isTrue(label->getLocation(geomIndex,
					Position::LEFT)==Location::UNDEF,
					"found single null side");
				label->setLocation(geomIndex,Position::RIGHT,
					currLoc);
				label->setLocation(geomIndex,Position::LEFT,
					currLoc);
			}
		}
	}
}

int EdgeEndStar::findIndex(EdgeEnd *eSearch){
	getIterator();   // force edgelist to be computed
	for (unsigned int i=0; i<edgeList->size(); i++ ) {
		EdgeEnd *e=(*edgeList)[i];
//		if (e->compareTo(eSearch)) return i;
		if (e==eSearch) return i;
	}
	return -1;
}

string
EdgeEndStar::print()
{
	string out="EdgeEndStar:   " + getCoordinate().toString()+"\n";
	for (vector<EdgeEnd*>::iterator it=getIterator();it<edgeList->end();it++)
	{
		EdgeEnd *e=*it;
		out+=e->print();
	}
	return out;
}

}

/**********************************************************************
 * $Log: EdgeEndStar.cpp,v $
 * Revision 1.9  2004/12/08 13:54:43  strk
 * gcc warnings checked and fixed, general cleanups.
 *
 * Revision 1.8  2004/11/23 19:53:06  strk
 * Had LineIntersector compute Z by interpolation.
 *
 * Revision 1.7  2004/11/22 11:34:49  strk
 * More debugging lines and comments/indentation cleanups
 *
 * Revision 1.6  2004/11/17 08:13:16  strk
 * Indentation changes.
 * Some Z_COMPUTATION activated by default.
 *
 * Revision 1.5  2004/11/01 16:43:04  strk
 * Added Profiler code.
 * Temporarly patched a bug in DoubleBits (must check drawbacks).
 * Various cleanups and speedups.
 *
 * Revision 1.4  2004/10/21 22:29:54  strk
 * Indentation changes and some more COMPUTE_Z rules
 *
 * Revision 1.3  2004/07/02 13:28:26  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.2  2004/05/03 10:43:42  strk
 * Exception specification considered harmful - left as comment.
 *
 * Revision 1.1  2004/03/19 09:48:45  ybychkov
 * "geomgraph" and "geomgraph/indexl" upgraded to JTS 1.4
 *
 * Revision 1.18  2003/11/12 15:43:38  strk
 * Added some more throw specifications
 *
 * Revision 1.17  2003/11/07 01:23:42  pramsey
 * Add standard CVS headers licence notices and copyrights to all cpp and h
 * files.
 *
 *
 **********************************************************************/

