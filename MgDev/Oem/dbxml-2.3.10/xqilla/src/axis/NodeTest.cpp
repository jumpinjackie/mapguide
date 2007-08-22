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
 * $Id: NodeTest.cpp,v 1.13 2006/11/01 16:37:18 jpcs Exp $
 */

#include "../config/xqilla_config.h"
#include <assert.h>
#include <sstream>

#include <xqilla/axis/NodeTest.hpp>

#include <xqilla/utils/XPath2Utils.hpp>
#include <xqilla/items/Node.hpp>
#include <xqilla/context/DynamicContext.hpp>
#include <xqilla/utils/XStr.hpp>

#if defined(XERCES_HAS_CPP_NAMESPACE)
XERCES_CPP_NAMESPACE_USE
#endif

NodeTest::NodeTest()
  : _name(0),
    _uri(0),
    _prefix(0),
    _type(0),
    _wildcardName(false),
    _wildcardNamespace(false),
    _wildcardType(false),
    _usePrefix(false),
    _hasChildren(false),
    _itemType(0)
{
}

NodeTest::~NodeTest() {
  delete _itemType;
}

void *NodeTest::getInterface(const XMLCh *name) const
{
  return 0;
}

void NodeTest::getStaticType(StaticType &st, const StaticContext *context,
	bool &isExact, const LocationInfo *location) const
{
  if(_itemType) {
	  _itemType->getStaticType(st, context, isExact, location);
  }
  else {
    if(_wildcardType) {
      if(_hasChildren) {
        st.flags = StaticType::ELEMENT_TYPE | StaticType::DOCUMENT_TYPE;
      }
      else {
        st.flags = StaticType::NODE_TYPE;
      }
    }
    else if(_type == Node::document_string) {
      st.flags = StaticType::DOCUMENT_TYPE;
    }
    else if(_type == Node::element_string) {
      st.flags = StaticType::ELEMENT_TYPE;
    }
    else if(_type == Node::attribute_string) {
      st.flags = StaticType::ATTRIBUTE_TYPE;
    }
    else if(_type == Node::processing_instruction_string) {
      st.flags = StaticType::PI_TYPE;
    }
    else if(_type == Node::comment_string) {
      st.flags = StaticType::COMMENT_TYPE;
    }
    else if(_type == Node::text_string ||
            _type == Node::cdata_string) {
      st.flags = StaticType::TEXT_TYPE;
    }

    if(_wildcardName && _wildcardNamespace)
      isExact = true;
    else isExact = false;
  }
}

Result NodeTest::filterResult(const Result &toFilter, const LocationInfo *info) const
{
  return new FilterResult(info, toFilter, this);
}

bool NodeTest::filterNode(Node::Ptr node, DynamicContext* context, const LocationInfo *info) const
{
  if(_itemType) return _itemType->matches(node, context, info);
  else return checkNodeType(node) && checkNodeName(node, context);
}

bool NodeTest::checkNodeType(Node::Ptr node) const
{
	assert(node.notNull());

	if(_wildcardType) {
    if(_hasChildren) {
      return node->dmNodeKind() == Node::element_string || node->dmNodeKind() == Node::document_string;
    }
		else return true;
	}

  return node->dmNodeKind() == _type;
}

bool NodeTest::checkNodeName(Node::Ptr node, const DynamicContext *context) const
{
  ATQNameOrDerived::Ptr name = node->dmNodeName(context);

  const XMLCh *uri = _uri;
  if(uri == 0 && node->dmNodeKind() == Node::element_string) {
    uri = context->getDefaultElementAndTypeNS();
  }

  return (_wildcardName || (name.notNull() && XPath2Utils::equals(((ATQNameOrDerived*)name.get())->getName(), _name)))
	  && (_wildcardNamespace || (name.notNull() && XPath2Utils::equals(((ATQNameOrDerived*)name.get())->getURI(), uri)));
}


void NodeTest::setNodeName(const XMLCh* name)
{
  _name = name;
}

void NodeTest::setNodePrefix(const XMLCh* prefix)
{
  _prefix=prefix;
  _usePrefix=true;
}

void NodeTest::setNodeUri(const XMLCh* uri)
{
  _uri = uri;
  _usePrefix=false;
}

void NodeTest::setNodeType(const XMLCh *type)
{
  _type = type;
}

void NodeTest::setNameWildcard(bool value)
{
  _wildcardName = value;
}

void NodeTest::setNamespaceWildcard(bool value)
{
  _wildcardNamespace = value;
}

void NodeTest::setTypeWildcard(bool value)
{
  _wildcardType = value;
}

bool NodeTest::getHasChildren() const
{
  return _hasChildren;
}

void NodeTest::setHasChildren(bool value)
{
  _hasChildren = value;
}


const XMLCh *NodeTest::getNodeType() const
{
  return _type;
}

const XMLCh* NodeTest::getNodeUri() const
{
  return _uri;
}

const XMLCh* NodeTest::getNodePrefix() const
{
  return _prefix;
}

const XMLCh* NodeTest::getNodeName() const
{
  return _name;
}

bool NodeTest::getNameWildcard() const
{
  return _wildcardName;
}

bool NodeTest::getNamespaceWildcard() const
{
  return _wildcardNamespace;
}

bool NodeTest::getTypeWildcard() const
{
  return _wildcardType;
}


bool NodeTest::isNodeTypeSet() const
{
  return _type != 0;
}

void NodeTest::staticResolution(StaticContext *context, const LocationInfo *location)
{
  if(isNodePrefixSet()) {
    setNodeUri(context->getUriBoundToPrefix(getNodePrefix(), location));
  }
  if(!_wildcardNamespace && _uri == 0 &&
     !_wildcardType && _type == Node::element_string) {
    _uri = context->getDefaultElementAndTypeNS();
  }
}

bool NodeTest::isNodePrefixSet() const {
  return _usePrefix;
}

SequenceType::ItemType* NodeTest::getItemType() const {
  return _itemType;
}

void NodeTest::setItemType(SequenceType::ItemType* type) {
    _itemType=type;
}

/////////////////////////////////////
// FilterResult
/////////////////////////////////////

NodeTest::FilterResult::FilterResult(const LocationInfo *info, const Result &toFilter, const NodeTest *nodeTest)
  : ResultImpl(info),
    toFilter_(toFilter),
    nodeTest_(nodeTest)
{
}

Item::Ptr NodeTest::FilterResult::next(DynamicContext *context)
{
  Node::Ptr result = 0;
  while((result = toFilter_->next(context)).notNull() && !nodeTest_->filterNode(result, context, this)) {}

  return result;
}

std::string NodeTest::FilterResult::asString(DynamicContext *context, int indent) const
{
  std::ostringstream oss;
  std::string in(getIndent(indent));

  oss << in << "<nodetest_filter/>" << std::endl;

  return oss.str();
}
