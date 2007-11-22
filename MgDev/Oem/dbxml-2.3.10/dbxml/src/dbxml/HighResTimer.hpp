//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: HighResTimer.hpp,v 1.22 2006/10/30 17:45:52 bostic Exp $
//

#ifndef __TIMER_H
#define	__TIMER_H

#include <string>
#include <db_cxx.h>

namespace DbXml
{

class DBXML_EXPORT HighResTimer
{
public:
	HighResTimer(const char *name = 0, const char *what = 0, const char *whats = 0);
	virtual ~HighResTimer();
	void start();
	void stop();
	void duration(long *seconds, long *microseconds) const;
	double durationInSeconds() const;
	void reset();
	std::string asString() const;
	long count() const
	{
		return count_;
	}
	double rate() const;

private:
	const char *name_;
	const char *what_;
	const char *whats_;
	long count_;

#ifdef _MSC_VER

	void *timer_;
#else

	u_int32_t start_secs, start_usecs;
	long duration_secs, duration_usecs;
#endif
};

}

#endif

