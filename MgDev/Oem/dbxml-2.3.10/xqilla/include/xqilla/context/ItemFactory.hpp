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
 * $Id: ItemFactory.hpp,v 1.12 2006/11/01 16:37:12 jpcs Exp $
 */

/*
  Factory base class
*/

#ifndef _ITEMFACTORY_HPP
#define _ITEMFACTORY_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/mapm/m_apm.h>
#include <xqilla/items/ATAnyURIOrDerived.hpp>
#include <xqilla/items/ATBooleanOrDerived.hpp>
#include <xqilla/items/ATDateOrDerived.hpp>
#include <xqilla/items/ATDateTimeOrDerived.hpp>
#include <xqilla/items/ATDecimalOrDerived.hpp>
#include <xqilla/items/ATDoubleOrDerived.hpp>
#include <xqilla/items/ATDurationOrDerived.hpp>
#include <xqilla/items/ATFloatOrDerived.hpp>
#include <xqilla/items/ATGDayOrDerived.hpp>
#include <xqilla/items/ATGMonthOrDerived.hpp>
#include <xqilla/items/ATGMonthDayOrDerived.hpp>
#include <xqilla/items/ATGYearMonthOrDerived.hpp>
#include <xqilla/items/ATGYearOrDerived.hpp>
#include <xqilla/items/ATQNameOrDerived.hpp>
#include <xqilla/items/ATStringOrDerived.hpp>
#include <xqilla/items/ATTimeOrDerived.hpp>
#include <xqilla/items/ATUntypedAtomic.hpp>
#include <xqilla/items/Node.hpp>

#include <xercesc/util/XMemory.hpp>

class XQILLA_API ItemFactory : public XERCES_CPP_NAMESPACE_QUALIFIER XMemory
{
public:
  virtual ~ItemFactory() {};

  /* @name Atomic type factory methods */

  virtual AnyAtomicType::AtomicObjectType getPrimitiveTypeIndex(const XMLCh* typeURI, const XMLCh* typeName, bool &isPrimitive) const = 0;
  virtual AnyAtomicType::Ptr createDerivedFromAtomicType(AnyAtomicType::AtomicObjectType typeIndex, const XMLCh* value,
                                                         const DynamicContext* context) = 0;
  virtual AnyAtomicType::Ptr createDerivedFromAtomicType(AnyAtomicType::AtomicObjectType typeIndex, const XMLCh* typeURI,
                                                         const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual AnyAtomicType::Ptr createDerivedFromAtomicType(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  /* @name Node factory methods */

  class XQILLA_API ElementChild {
  public:
    ElementChild(const Node::Ptr &n) : node(n), clone(true) {}
    ElementChild(const Node::Ptr &n, bool c) : node(n), clone(c) {}
    operator const Node::Ptr &() { return node; }
    const Node *operator ->() const { return node.get(); }

    Node::Ptr node;
    bool clone;
  };

  virtual Node::Ptr cloneNode(const Node::Ptr node, const DynamicContext *context) const = 0;
  virtual Node::Ptr createTextNode(const XMLCh *value, const DynamicContext *context) const = 0;
  virtual Node::Ptr createCommentNode(const XMLCh *value, const DynamicContext *context) const = 0;
  virtual Node::Ptr createPINode(const XMLCh *name, const XMLCh *value, const DynamicContext *context) const = 0;
  virtual Node::Ptr createAttributeNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name,
                                        const XMLCh *value, const DynamicContext *context) const = 0;
  virtual Node::Ptr createElementNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name,
                                      const std::vector<Node::Ptr> &attrList, const std::vector<ElementChild> &childList,
                                      const DynamicContext *context) const = 0;
  virtual Node::Ptr createDocumentNode(const std::vector<Node::Ptr> &childList, const DynamicContext *context) const = 0;
  virtual const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* createNamespaceNode(const XMLCh* prefix, const XMLCh* uri, const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* parentNode, const DynamicContext *context) const = 0;

  /* @name Number factory methods */

  virtual ATDoubleOrDerived::Ptr createDouble(const MAPM value, const DynamicContext* context) = 0;
  virtual ATDoubleOrDerived::Ptr createDouble(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDoubleOrDerived::Ptr createDoubleOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context) = 0;
  virtual ATDoubleOrDerived::Ptr createDoubleOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATFloatOrDerived::Ptr createFloat(const MAPM value, const DynamicContext* context) = 0;
  virtual ATFloatOrDerived::Ptr createFloat(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATFloatOrDerived::Ptr createFloatOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context) = 0;
  virtual ATFloatOrDerived::Ptr createFloatOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATDecimalOrDerived::Ptr createInteger(const int value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createInteger(const MAPM value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createInteger(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createNonNegativeInteger(const MAPM value, const DynamicContext* context) = 0;

  virtual ATDecimalOrDerived::Ptr createDecimal(const MAPM value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createDecimal(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createDecimalOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const MAPM value, const DynamicContext* context) = 0;
  virtual ATDecimalOrDerived::Ptr createDecimalOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  /* @name Date / time factory methods */

  virtual ATDateOrDerived::Ptr createDate(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDateOrDerived::Ptr createDateOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATDateTimeOrDerived::Ptr createDateTime(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDateTimeOrDerived::Ptr createDateTimeOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATTimeOrDerived::Ptr createTime(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATTimeOrDerived::Ptr createTimeOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATGDayOrDerived::Ptr createGDayOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATGMonthDayOrDerived::Ptr createGMonthDayOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATGMonthOrDerived::Ptr createGMonthOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATGYearMonthOrDerived::Ptr createGYearMonthOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATGYearOrDerived::Ptr createGYearOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATDurationOrDerived::Ptr createDayTimeDuration(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDurationOrDerived::Ptr createDayTimeDuration(const MAPM &seconds, const DynamicContext* context) = 0;
  virtual ATDurationOrDerived::Ptr createYearMonthDuration(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATDurationOrDerived::Ptr createYearMonthDuration(const MAPM &months, const DynamicContext* context) = 0;
  virtual ATDurationOrDerived::Ptr createDurationOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  /* @name Other factory methods */

  virtual ATStringOrDerived::Ptr createString(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATStringOrDerived::Ptr createStringOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATUntypedAtomic::Ptr createUntypedAtomic(const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATBooleanOrDerived::Ptr createBoolean(bool value, const DynamicContext* context) = 0;
  virtual ATBooleanOrDerived::Ptr createBoolean(const XMLCh* value, const DynamicContext* context) = 0;
  virtual ATBooleanOrDerived::Ptr createBooleanOrDerived(const XMLCh* typeURI, const XMLCh* typeName, bool value, const DynamicContext* context) = 0;
  virtual ATBooleanOrDerived::Ptr createBooleanOrDerived(const XMLCh* typeURI, const XMLCh* typeName, const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATAnyURIOrDerived::Ptr createAnyURI(const XMLCh* value, const DynamicContext* context) = 0;

  virtual ATQNameOrDerived::Ptr createQName(const XMLCh* uri, const XMLCh* prefix, const XMLCh* name, const DynamicContext* context) = 0;
  virtual ATQNameOrDerived::Ptr createQNameOrDerived(const XMLCh* typeURI, const XMLCh *prefix, const XMLCh* typeName, const XMLCh* uri, const XMLCh* name, const DynamicContext* context) = 0;

};

#endif
