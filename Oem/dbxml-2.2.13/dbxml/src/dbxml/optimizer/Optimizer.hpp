//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002-2005
//	Sleepycat Software.  All rights reserved.
//
// $Id: Optimizer.hpp,v 1.4 2005/04/05 16:44:14 bostic Exp $
//

#ifndef __OPTIMIZER_HPP
#define	__OPTIMIZER_HPP

class DataItem;
class DynamicContext;

class XQQuery;

namespace DbXml
{

class Optimizer
{
public:
  Optimizer(Optimizer *parent = 0);
  virtual ~Optimizer();

  void startOptimize(XQQuery *query);
  DataItem *startOptimize(DataItem *item);
  void reset();

protected:
  virtual void optimize(XQQuery *query) = 0;
  virtual DataItem *optimize(DataItem *item) = 0;
  virtual void resetInternal() = 0;

private:
  Optimizer(const Optimizer &o);
  Optimizer &operator=(const Optimizer &o);

  Optimizer *parent_;
};

class StaticResolver : public Optimizer
{
public:
  StaticResolver(DynamicContext *xpc, Optimizer *parent = 0)
    : Optimizer(parent), xpc_(xpc) {}
  virtual ~StaticResolver() {}

protected:
  virtual void optimize(XQQuery *query);
  virtual DataItem *optimize(DataItem *item);
  virtual void resetInternal() {}

private:
  DynamicContext *xpc_;
};

}

#endif
