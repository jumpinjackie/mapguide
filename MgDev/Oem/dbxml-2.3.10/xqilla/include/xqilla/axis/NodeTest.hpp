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
 * $Id: NodeTest.hpp,v 1.10 2006/11/01 16:37:11 jpcs Exp $
 */

#ifndef _NODETEST_HPP
#define _NODETEST_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/runtime/Result.hpp>
#include <xqilla/runtime/ResultImpl.hpp>
#include <xqilla/schema/SequenceType.hpp>

class StaticContext;
class DynamicContext;

/**
 * This class represents a NodeTest production from the grammar, 
 * and provides methods for filtering a Result containing Node items,
 * and determining if a single Item matches.
 */
class XQILLA_API NodeTest
{
public:
  NodeTest();
  virtual ~NodeTest();

  /** Method for returning arbitrary interfaces from the implementations */
  virtual void *getInterface(const XMLCh *name) const;

  /// Filters the given Result, returning only Items that match the NodeTest
  Result filterResult(const Result &toFilter, const LocationInfo *info) const;
  ///check if a node has to be placed in the result set
  bool filterNode(Node::Ptr node, DynamicContext* context, const LocationInfo *info) const;

  void staticResolution(StaticContext *context, const LocationInfo *location);

  ///gets the sequence type used to match the nodes
  SequenceType::ItemType* getItemType() const;
  ///set function for private member
  void setItemType(SequenceType::ItemType* type);
  
  ///gets the name of the node we are selecting
  const XMLCh* getNodeName() const;
  ///set function for private member
  void setNodeName(const XMLCh* name);

  ///gets the uri of the node we are selecting
  const XMLCh* getNodeUri() const;
  ///set function for private member
  void setNodeUri(const XMLCh* uri);

  ///gets the prefix of the node we are selecting
  const XMLCh* getNodePrefix() const;
  ///set function for private member
  void setNodePrefix(const XMLCh* prefix);
  ///returns whether the prefix has been set (instead of the uri)
  bool isNodePrefixSet() const;

  ///gets the node type we are selecting
  const XMLCh *getNodeType() const;
  ///set node type looked for
  void setNodeType(const XMLCh *type);
  ///retuns whether the type has been set in this NavStep
  bool isNodeTypeSet() const;

  ///returns true if the Node Name ia a wildcard
  bool getNameWildcard() const;
  ///sets the wildcard nodeName bool
  void setNameWildcard(bool value = true);

  ///returns true if the Node prefix ia a wildcard
  bool getNamespaceWildcard() const;
  ///sets the prefix wildcard bool
  void setNamespaceWildcard(bool value = true);

  ///returns true if the type is a wildcard
  bool getTypeWildcard() const;
  ///sets the type wildcard bool
  void setTypeWildcard(bool value = true);

  /// returns true if the following step is a child axis (for optimisation reasons)
  bool getHasChildren() const;
  /// sets whether the following step is a child axis
  void setHasChildren(bool value = true);

  void getStaticType(StaticType &st, const StaticContext *context,
                     bool &isExact, const LocationInfo *location) const;

protected:
  ///check a node's type against _type, considers wildcard
  bool checkNodeType(Node::Ptr node) const;

  ///check a node's name against _name, considers wildcard
  bool checkNodeName(Node::Ptr node, const DynamicContext *context) const;

  /**
   * Performs filtering based on type or name.
   */
  class XQILLA_API FilterResult : public ResultImpl
  {
  public:
    FilterResult(const LocationInfo *info, const Result &toFilter, const NodeTest *nodeTest);
    virtual ~FilterResult() {}

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;

  protected:
    Result toFilter_;
    const NodeTest *nodeTest_;
  };

protected:
  const XMLCh* _name,*_uri,*_prefix;
  const XMLCh* _type;
  bool _wildcardName, _wildcardNamespace, _wildcardType, _usePrefix, _hasChildren;
  SequenceType::ItemType* _itemType;
};

#endif
