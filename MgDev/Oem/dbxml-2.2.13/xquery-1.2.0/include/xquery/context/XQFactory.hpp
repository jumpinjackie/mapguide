//
// See the file LICENSE for redistribution information.
//
// Copyright (c) 2004-2005
//	Sleepycat Software.  All rights reserved.
// Copyright (c) 2004-2005
//	Progress Software Corporation.  All rights reserved.
//
// $Id: XQFactory.hpp,v 1.3 2005/07/07 10:12:23 jsnelson Exp $
//

#if !defined(AFX_XQFACTORY_H__D608B994_F090_4206_9473_81F3D7350510__INCLUDED_)
#define AFX_XQFACTORY_H__D608B994_F090_4206_9473_81F3D7350510__INCLUDED_

#include "../XQEngine.hpp"
#include <pathan/PathanFactory.hpp>

class XQENGINE_API XQFactory : public PathanFactory
{
public:
  virtual ~XQFactory() {};

  virtual void release() = 0;

  /* @name Node factory methods */

  class XQENGINE_API ElementChild {
  public:
    ElementChild(const Node::Ptr &n) : node(n), clone(true) {}
    ElementChild(const Node::Ptr &n, bool c) : node(n), clone(c) {}
    operator const Node::Ptr &() { return node; }
    const Node *operator ->() const { return node.get(); }

    Node::Ptr node;
    bool clone;
  };

  virtual Node::Ptr createTextNode(const XMLCh *value, const DynamicContext *context) const = 0;
  virtual Node::Ptr createCommentNode(const XMLCh *value, const DynamicContext *context) const = 0;
  virtual Node::Ptr createPINode(const XMLCh *name, const XMLCh *value, const DynamicContext *context) const = 0;
  virtual Node::Ptr createAttributeNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name,
                                        const XMLCh *value, const DynamicContext *context) const = 0;
  virtual Node::Ptr createElementNode(const XMLCh *uri, const XMLCh *prefix, const XMLCh *name,
                                      const std::vector<Node::Ptr> &attrList, const std::vector<ElementChild> &childList,
                                      const DynamicContext *context) const = 0;
  virtual Node::Ptr createDocumentNode(const std::vector<Node::Ptr> &childList, const DynamicContext *context) const = 0;

};

#endif // !defined(AFX_XQFACTORY_H__D608B994_F090_4206_9473_81F3D7350510__INCLUDED_)
