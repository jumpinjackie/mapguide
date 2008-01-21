//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: HighResTimer.cpp,v 1.30 2006/10/30 17:45:52 bostic Exp $
//

#include "DbXmlInternal.hpp"
#include "HighResTimer.hpp"

#include <db_cxx.h>

#include <iostream>
#include <sstream>

extern "C" int __os_clock(DB_ENV *, u_int32_t *, u_int32_t *);

using namespace DbXml;

#define	USECS_PER_SEC 1000000

#ifdef _MSC_VER

#include <windows.h>

//
// This struct exists so that we don't have to have #include <windows.h>
// HighResTimer.hpp. For some reason that ends up including microsoft's
// XML header file that defines DOMDocument and leads to all sorts of
// fun.
//
struct Timer
{
	static LARGE_INTEGER frequency_;
	LARGE_INTEGER start_;
	LARGE_INTEGER stop_;
	__int64 duration_;
};

LARGE_INTEGER Timer::frequency_ = {0, 0};

HighResTimer::HighResTimer(const char *name, const char *what, const char *whats)
	: name_(name),
	what_(what),
	whats_(whats),
	count_(0),
	timer_(new Timer)
{
	((Timer*)timer_)->start_.QuadPart = 0;
	((Timer*)timer_)->stop_.QuadPart = 0;
	((Timer*)timer_)->duration_ = 0;
	if (*((__int64*)&((Timer*)timer_)->frequency_) == 0) {
		::QueryPerformanceFrequency(&((Timer*)timer_)->frequency_);
	}
}

HighResTimer::~HighResTimer()
{
	delete timer_;
}

void HighResTimer::start()
{
	::QueryPerformanceCounter(&((Timer*)timer_)->start_);
	((Timer*)timer_)->stop_.QuadPart = 0;
}

void HighResTimer::stop()
{
	DBXML_ASSERT(((Timer*)timer_)->start_.QuadPart > 0);
	::QueryPerformanceCounter(&((Timer*)timer_)->stop_);
	((Timer*)timer_)->duration_ += ((Timer*)timer_)->stop_.QuadPart - ((Timer*)timer_)->start_.QuadPart;
	count_++;
}

void HighResTimer::duration(long *seconds, long *microseconds) const
{
	__int64 d = ((Timer*)timer_)->duration_;
	__int64 f = *((__int64*) & ((Timer*)timer_)->frequency_);
	__int64 s = d / f;
	__int64 m = ( USECS_PER_SEC * ( d % f ) ) / f;
	// throwing away the top 32 bits...
	*seconds = (long)s; // that limits us to 70 years or something.
	*microseconds = (long)m; // Can't be >USECS_PER_SEC
}

void HighResTimer::reset()
{
	count_ = 0;
	((Timer*)timer_)->start_.QuadPart = 0;
	((Timer*)timer_)->stop_.QuadPart = 0;
	((Timer*)timer_)->duration_ = 0;
}

#else /* !_MSC_VER */

HighResTimer::HighResTimer(const char *name, const char *what, const char *whats)
	: name_(name),
	what_(what),
	whats_(whats),
	count_(0),
	start_secs(0), start_usecs(0),
	duration_secs(0), duration_usecs(0)
{}

HighResTimer::~HighResTimer()
{}

void HighResTimer::start()
{
	(void)__os_clock(NULL, &start_secs, &start_usecs);
}

void HighResTimer::stop()
{
	u_int32_t stop_secs, stop_usecs;
	(void)__os_clock(NULL, &stop_secs, &stop_usecs);
	duration_secs += (stop_secs - start_secs);
	duration_usecs += (stop_usecs - start_usecs);
	// fixup for usec under/overflow
	if (duration_usecs < 0) {
		duration_secs -= 1;
		duration_usecs += USECS_PER_SEC;
	} else if (duration_usecs > USECS_PER_SEC) {
		duration_secs += 1;
		duration_usecs -= USECS_PER_SEC;
	}
	count_++;
}

void HighResTimer::duration(long *seconds, long *microseconds) const
{
	*seconds = duration_secs;
	*microseconds = duration_usecs;
}

void HighResTimer::reset()
{
	start_secs = 0;
	start_usecs = 0;
	duration_secs = 0;
	duration_usecs = 0;
}
#endif /* _MSC_VER */

double HighResTimer::durationInSeconds() const
{
	long seconds, microseconds;
	duration(&seconds, &microseconds);
	return (double)seconds + ((double)microseconds / (double)USECS_PER_SEC);
}

double HighResTimer::rate() const
{
	double duration = durationInSeconds();
	double rate = duration == 0 ? 0 : (count_ / duration);
	return rate;
}

std::string HighResTimer::asString() const
{
	double duration = durationInSeconds();
	double average = count_ == 0 ? 0 : (duration / count_);
	double rate = average == 0 ? 0 : (1 / average);
	std::ostringstream s;
	s << (name_ == 0 ? "noname" : name_) << " Timer: ";
	s << count_ << " " << (whats_ != 0 ? whats_ : "") << " in " << duration << " seconds, ";
	s << average << " seconds per " << (what_ != 0 ? what_ : "") << ", ";
	s << rate << " " << whats_ << " per second.";
	return s.str();
}
