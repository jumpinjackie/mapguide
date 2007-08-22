//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Timer.cpp,v 1.3 2006/10/30 17:45:50 bostic Exp $
//
#include "Timer.hpp"

#include <db_cxx.h>
#include <iostream>
#include <sstream>

// use __os_clock from Berkeley DB internals

extern "C" int __os_clock(DB_ENV *, u_int32_t *, u_int32_t *);

using namespace DbXml;

#define	USECS_PER_SEC 1000000

Timer::Timer(const char *name, const char *what, const char *whats)
	: name_(name),
	what_(what),
	whats_(whats),
	count_(0),
	start_secs(0), start_usecs(0),
	duration_secs(0), duration_usecs(0)
{}

Timer::~Timer()
{}

void Timer::start()
{
	(void)__os_clock(NULL, &start_secs, &start_usecs);
}

void Timer::stop()
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

void Timer::duration(long *seconds, long *microseconds) const
{
	*seconds = duration_secs;
	*microseconds = duration_usecs;
}

void Timer::reset()
{
	start_secs = 0;
	start_usecs = 0;
	duration_secs = 0;
	duration_usecs = 0;
}

double Timer::durationInSeconds() const
{
	long seconds, microseconds;
	duration(&seconds, &microseconds);
	return (double)seconds + ((double)microseconds / (double)USECS_PER_SEC);
}

double Timer::durationInUsecs() const
{
	long seconds, microseconds;
	duration(&seconds, &microseconds);
	return ((double)seconds * (double)USECS_PER_SEC) + (double)microseconds;
}

double Timer::rate(long count) const
{
	if (count == 0)
		count = count_;
	double duration = durationInSeconds();
	double rate = duration == 0 ? 0 : (count / duration);
	return rate;
}

std::string Timer::asString() const
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
