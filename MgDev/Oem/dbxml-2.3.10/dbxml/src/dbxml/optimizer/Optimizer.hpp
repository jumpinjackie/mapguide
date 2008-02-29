//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2002,2006 Oracle.  All rights reserved.
//
// $Id: Optimizer.hpp,v 1.8 2006/10/30 17:45:59 bostic Exp $
//

#ifndef __OPTIMIZER_HPP
#define	__OPTIMIZER_HPP

class ASTNode;
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
  ASTNode *startOptimize(ASTNode *item);
  void reset();

protected:
  virtual void optimize(XQQuery *query) = 0;
  virtual ASTNode *optimize(ASTNode *item) = 0;
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
  virtual ASTNode *optimize(ASTNode *item);
  virtual void resetInternal() {}

private:
  DynamicContext *xpc_;
};

class StaticTyper : public Optimizer
{
public:
  StaticTyper(DynamicContext *xpc, Optimizer *parent = 0)
    : Optimizer(parent), xpc_(xpc) {}
  virtual ~StaticTyper() {}

protected:
  virtual void optimize(XQQuery *query);
  virtual ASTNode *optimize(ASTNode *item);
  virtual void resetInternal() {}

private:
  DynamicContext *xpc_;
};

}

#endif
