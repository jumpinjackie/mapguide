/**********************************************************************
 * $Id: Profiler.cpp,v 1.4 2004/12/03 22:52:56 strk Exp $
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

#include <geos/profiler.h>

namespace geos {

static Profiler *internal_profiler = new Profiler();

Profile::Profile(string newname)
{
	name = newname;
	totaltime = 0;
	min = max = avg = 0;
}

Profile::~Profile()
{
}

void
Profile::start()
{
	gettimeofday(&starttime, NULL);
}

void
Profile::stop()
{
	gettimeofday(&stoptime, NULL);
	double elapsed = 1000000*(stoptime.tv_sec-starttime.tv_sec)+
		(stoptime.tv_usec-starttime.tv_usec);

	timings.push_back(elapsed);
	totaltime += elapsed;
	if ( timings.size() == 1 ) max = min = elapsed;
	else
	{
		if ( elapsed > max ) max = elapsed;
		if ( elapsed < min ) min = elapsed;
	}
	avg = totaltime / timings.size();
}

double
Profile::getMax() const
{
	return max;
}

double
Profile::getMin() const
{
	return min;
}

double
Profile::getAvg() const
{
	return avg;
}

double
Profile::getTot() const
{
	return totaltime;
}

unsigned int
Profile::getNumTimings() const
{
	return timings.size();
}

Profiler::Profiler()
{
}

Profiler::~Profiler()
{
	map<string, Profile *>::const_iterator it;
	for ( it=profs.begin(); it != profs.end(); ++it )
	{
		delete it->second;
	}
}

void
Profiler::start(string name)
{
	Profile *prof;
	map<string, Profile *>::iterator iter = profs.find(name);
	if ( iter == profs.end() ) {
		prof = new Profile(name);
		profs.insert(pair<string, Profile *>(name, prof));
	} else {
		prof = iter->second;
	}
	prof->start();
}

void
Profiler::stop(string name)
{
	map<string, Profile *>::iterator iter = profs.find(name);
	if ( iter == profs.end() )
		cerr<<name<<": no such Profile started";
	iter->second->stop();
}

Profile *
Profiler::get(string name)
{
	Profile *prof;
	map<string, Profile *>::iterator iter = profs.find(name);
	if ( iter == profs.end() ) {
		prof = new Profile(name);
		profs.insert(pair<string, Profile *>(name, prof));
	} else {
		prof = iter->second;
	}
	return prof;
}

Profiler *
Profiler::instance()
{
	return internal_profiler;
}


ostream&
operator<< (ostream &os, const Profile &prof)
{
	os << " num:"<<prof.getNumTimings()<<" min:"<<
		prof.getMin()<<" max:"<<prof.getMax()<<
		" avg:"<<prof.getAvg()<<" tot:"<<prof.getTot()<<
		" ["<<prof.name<<"]";
	return os;
}

ostream&
operator<< (ostream &os, const Profiler &prof)
{
	map<string, Profile *>::const_iterator it;
	for ( it=prof.profs.begin(); it != prof.profs.end(); ++it )
	{
		os<<*(it->second)<<endl;
	}
	return os;
}


} // namespace geos

/**********************************************************************
 * $Log: Profiler.cpp,v $
 * Revision 1.4  2004/12/03 22:52:56  strk
 * enforced const return of CoordinateSequence::toVector() method to derivate classes.
 *
 * Revision 1.3  2004/11/08 12:15:35  strk
 * Added number of gathered timings in output.
 *
 * Revision 1.2  2004/11/08 11:19:39  strk
 * Profiler::get() always return a Profile (new if not existant).
 *
 * Revision 1.1  2004/11/01 16:43:04  strk
 * Added Profiler code.
 * Temporarly patched a bug in DoubleBits (must check drawbacks).
 * Various cleanups and speedups.
 *
 **********************************************************************/
