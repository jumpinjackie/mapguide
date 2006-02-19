/**********************************************************************
 * $Id: EnhancedPrecisionOp.cpp,v 1.2 2004/07/02 13:28:29 strk Exp $
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
 * $Log: EnhancedPrecisionOp.cpp,v $
 * Revision 1.2  2004/07/02 13:28:29  strk
 * Fixed all #include lines to reflect headers layout change.
 * Added client application build tips in README.
 *
 * Revision 1.1  2004/04/10 22:41:25  ybychkov
 * "precision" upgraded to JTS 1.4
 *
 *
 **********************************************************************/


#include <geos/precision.h>
#include <geos/util.h>

namespace geos {
/**
* Computes the set-theoretic intersection of two {@link Geometry}s, using enhanced precision->
* @param geom0 the first Geometry
* @param geom1 the second Geometry
* @return the Geometry representing the set-theoretic intersection of the input Geometries->
*/
Geometry* EnhancedPrecisionOp::intersection(Geometry *geom0, Geometry *geom1) {
	GEOSException *originalEx;
	try {
		Geometry *result = geom0->intersection(geom1);
		return result;
	} catch (GEOSException *ex) {
		originalEx = ex;
	}
	/*
	* If we are here, the original op encountered a precision problem
	* (or some other problem)->  Retry the operation with
	* enhanced precision to see if it succeeds
	*/
	try {
		CommonBitsOp *cbo = new CommonBitsOp(true);
		Geometry *resultEP = cbo->intersection(geom0, geom1);
		delete cbo; // check that result is a valid geometry after the reshift to orginal precision
		if (! resultEP->isValid())
			throw originalEx;
		return resultEP;
	} catch (GEOSException *ex2) {
		throw originalEx;
	}
}
/**
* Computes the set-theoretic union of two {@link Geometry}s, using enhanced precision->
* @param geom0 the first Geometry
* @param geom1 the second Geometry
* @return the Geometry representing the set-theoretic union of the input Geometries->
*/
Geometry* EnhancedPrecisionOp::Union(Geometry *geom0, Geometry *geom1){
	GEOSException *originalEx;
	try {
		Geometry* result = geom0->Union(geom1);
		return result;
	}catch (GEOSException *ex) {
		originalEx = ex;
	}
	/*
	* If we are here, the original op encountered a precision problem
	* (or some other problem)->  Retry the operation with
	* enhanced precision to see if it succeeds
	*/
	try {
		CommonBitsOp *cbo = new CommonBitsOp(true);
		Geometry *resultEP = cbo->Union(geom0, geom1);
		delete cbo; // check that result is a valid geometry after the reshift to orginal precision
		if (! resultEP->isValid())
			throw originalEx;
		return resultEP;
	} catch (GEOSException *ex2) {
		throw originalEx;
	}
}
/**
* Computes the set-theoretic difference of two {@link Geometry}s, using enhanced precision->
* @param geom0 the first Geometry
* @param geom1 the second Geometry
* @return the Geometry representing the set-theoretic difference of the input Geometries->
*/
Geometry* EnhancedPrecisionOp::difference(Geometry *geom0, Geometry *geom1){
	GEOSException *originalEx;
	try {
		Geometry *result = geom0->difference(geom1);
		return result;
	} catch (GEOSException *ex) {
		originalEx = ex;
	}
	/*
	* If we are here, the original op encountered a precision problem
	* (or some other problem)->  Retry the operation with
	* enhanced precision to see if it succeeds
	*/
	try {
		CommonBitsOp *cbo = new CommonBitsOp(true);
		Geometry *resultEP = cbo->difference(geom0, geom1);
		delete cbo; // check that result is a valid geometry after the reshift to orginal precision
		if (! resultEP->isValid())
			throw originalEx;
		return resultEP;
	} catch (GEOSException *ex2) {
		throw originalEx;
	}
}
/**
* Computes the set-theoretic symmetric difference of two {@link Geometry}s, using enhanced precision->
* @param geom0 the first Geometry
* @param geom1 the second Geometry
* @return the Geometry representing the set-theoretic symmetric difference of the input Geometries->
*/
Geometry* EnhancedPrecisionOp::symDifference(Geometry *geom0, Geometry *geom1){
	GEOSException *originalEx;
	try {
		Geometry *result = geom0->symDifference(geom1);
		return result;
	} catch (GEOSException *ex) {
		originalEx = ex;
	}
	/*
	* If we are here, the original op encountered a precision problem
	* (or some other problem)->  Retry the operation with
	* enhanced precision to see if it succeeds
	*/
	try {
		CommonBitsOp *cbo = new CommonBitsOp(true);
		Geometry* resultEP = cbo->symDifference(geom0, geom1);
		delete cbo; // check that result is a valid geometry after the reshift to orginal precision
		if (! resultEP->isValid())
			throw originalEx;
		return resultEP;
	} catch (GEOSException *ex2) {
		throw originalEx;
	}
}
/**
* Computes the buffer of a {@link Geometry}, using enhanced precision->
* This method should no longer be necessary, since the buffer algorithm
* now is highly robust->
*
* @param geom0 the first Geometry
* @param distance the buffer distance
* @return the Geometry representing the buffer of the input Geometry->
*/
Geometry* EnhancedPrecisionOp::buffer(Geometry *geom, double distance){
	GEOSException *originalEx;
	try {
		Geometry *result = geom->buffer(distance);
		return result;
	} catch (GEOSException *ex)	{
		originalEx = ex;
	}
	/*
	* If we are here, the original op encountered a precision problem
	* (or some other problem)->  Retry the operation with
	* enhanced precision to see if it succeeds
	*/
	try {
		CommonBitsOp *cbo = new CommonBitsOp(true);
		Geometry *resultEP = cbo->buffer(geom, distance);
		delete cbo; // check that result is a valid geometry after the reshift to orginal precision
		if (! resultEP->isValid())
			throw originalEx;
		return resultEP;
	} catch (GEOSException *ex2) {
		throw originalEx;
	}
}

}
