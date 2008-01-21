/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: FTDistance.hpp,v 1.4 2006/11/01 16:37:12 jpcs Exp $
 */

#ifndef _FTDISTANCE_HPP
#define _FTDISTANCE_HPP

#include <xqilla/fulltext/FTOption.hpp>
#include <xqilla/fulltext/FTRange.hpp>

class XQILLA_API FTDistance : public FTOption
{
public:
  FTDistance(const FTRange &range, FTUnit unit, XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::DISTANCE, memMgr), arg_(0), range_(range), unit_(unit) {}
  FTDistance(const FTRange &range, FTUnit unit, FTSelection *arg, XPath2MemoryManager *memMgr)
    : FTOption(FTSelection::DISTANCE, memMgr), arg_(arg), range_(range), unit_(unit) {}

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  virtual void setArgument(FTSelection *arg) { arg_ = arg; }
  const FTSelection *getArgument() const { return arg_; }
  const FTRange &getRange() const { return range_; }
  FTUnit getUnit() const { return unit_; }

private:
  FTSelection *arg_;
  FTRange range_;
  FTUnit unit_;
};

class XQILLA_API FTDistanceLiteral : public FTOption
{
public:
  FTDistanceLiteral(FTRange::Type type, unsigned int distance, unsigned int distance2,
                    FTUnit unit, XPath2MemoryManager *memMgr)
    : FTOption(DISTANCE_LITERAL, memMgr), arg_(0), type_(type), distance_(distance),
      distance2_(distance2), unit_(unit) {}
  FTDistanceLiteral(FTSelection *arg, FTRange::Type type, unsigned int distance,
                    unsigned int distance2, FTUnit unit, XPath2MemoryManager *memMgr)
    : FTOption(DISTANCE_LITERAL, memMgr), arg_(arg), type_(type), distance_(distance),
      distance2_(distance2), unit_(unit) {}

  virtual FTSelection *staticResolution(StaticContext *context);
  virtual FTSelection *staticTyping(StaticContext *context);
  virtual FTSelection *optimize(FTContext *context, bool execute) const;
  virtual AllMatches::Ptr execute(FTContext *ftcontext) const;

  virtual void setArgument(FTSelection *arg) { arg_ = arg; }
  const FTSelection *getArgument() const { return arg_; }
  const FTRange::Type getType() const { return type_; }
  unsigned int getDistance() const { return distance_; }
  unsigned int getDistance2() const { return distance2_; }
  FTUnit getUnit() const { return unit_; }

private:
  FTSelection *arg_;
  FTRange::Type type_;
  unsigned int distance_, distance2_;
  FTUnit unit_;
};

class FTDistanceMatches : public AllMatches
{
public:
  FTDistanceMatches(const LocationInfo *info, unsigned int distance, FTOption::FTUnit unit, const AllMatches::Ptr &arg)
    : AllMatches(info), distance_(distance), unit_(unit), arg_(arg) {}
  Match::Ptr next(DynamicContext *context);

  virtual bool distanceMatches(unsigned int actual) const = 0;

protected:
  unsigned int distance_;
  FTOption::FTUnit unit_;
  AllMatches::Ptr arg_;
};

class FTDistanceExactlyMatches : public FTDistanceMatches
{
public:
  FTDistanceExactlyMatches(const LocationInfo *info, unsigned int distance, FTOption::FTUnit unit, const AllMatches::Ptr &arg)
    : FTDistanceMatches(info, distance, unit, arg) {}
  bool distanceMatches(unsigned int actual) const;
};

class FTDistanceAtLeastMatches : public FTDistanceMatches
{
public:
  FTDistanceAtLeastMatches(const LocationInfo *info, unsigned int distance, FTOption::FTUnit unit, const AllMatches::Ptr &arg)
    : FTDistanceMatches(info, distance, unit, arg) {}
  bool distanceMatches(unsigned int actual) const;
};

class FTDistanceAtMostMatches : public FTDistanceMatches
{
public:
  FTDistanceAtMostMatches(const LocationInfo *info, unsigned int distance, FTOption::FTUnit unit, const AllMatches::Ptr &arg)
    : FTDistanceMatches(info, distance, unit, arg) {}
  bool distanceMatches(unsigned int actual) const;
};

class FTDistanceFromToMatches : public FTDistanceMatches
{
public:
  FTDistanceFromToMatches(const LocationInfo *info, unsigned int from, unsigned int to, FTOption::FTUnit unit, const AllMatches::Ptr &arg)
    : FTDistanceMatches(info, from, unit, arg), distance2_(to) {}
  bool distanceMatches(unsigned int actual) const;

private:
  unsigned int distance2_;
};

#endif
