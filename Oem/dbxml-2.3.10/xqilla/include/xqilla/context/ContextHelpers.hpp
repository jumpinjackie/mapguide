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
 * $Id: ContextHelpers.hpp,v 1.2 2006/11/01 16:37:11 jpcs Exp $
 */

#if !defined(AFXQ_CONTEXTHELPERS_H__D6A320F5_21F1_421D_9E46_E4373B375E1A__INCLUDED_)
#define AFXQ_CONTEXTHELPERS_H__D6A320F5_21F1_421D_9E46_E4373B375E1A__INCLUDED_

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/context/DynamicContext.hpp>

class XQILLA_API AutoNodeSetOrderingReset
{
public:
  AutoNodeSetOrderingReset(StaticContext* context, StaticContext::NodeSetOrdering ordering = StaticContext::ORDERING_UNORDERED)
  {
    context_ = context;
    ordering_ = context->getNodeSetOrdering();
    context->setNodeSetOrdering(ordering);
  }

  ~AutoNodeSetOrderingReset()
  {
    context_->setNodeSetOrdering(ordering_);
  }

protected:
  StaticContext* context_;
  StaticContext::NodeSetOrdering ordering_;  
};

class XQILLA_API AutoContextItemTypeReset
{
public:
  AutoContextItemTypeReset(StaticContext* context, const StaticType &sType)
  {
    context_ = context;
    sType_ = context->getContextItemType();
    context->setContextItemType(sType);
  }

  ~AutoContextItemTypeReset()
  {
    context_->setContextItemType(sType_);
  }

protected:
  StaticContext* context_;
  StaticType sType_;
};

class XQILLA_API AutoContextInfoReset
{
public:
  AutoContextInfoReset(DynamicContext* context)
    : oldContextItem(context->getContextItem()),
      oldContextPosition(context->getContextPosition()),
      oldContextSize(context->getContextSize()),
      context_(context)
  {
  }

  ~AutoContextInfoReset()
  {
    resetContextInfo();
  }

  void resetContextInfo()
  {
    context_->setContextItem(oldContextItem);
    context_->setContextPosition(oldContextPosition);
    context_->setContextSize(oldContextSize);
  }

  Item::Ptr oldContextItem;
  unsigned int oldContextPosition;
  unsigned int oldContextSize;

private:
  DynamicContext* context_;
};

class XQILLA_API AutoDocumentCacheReset
{
public:
  AutoDocumentCacheReset(DynamicContext* context)
    : oldDC(const_cast<DocumentCache*>(context->getDocumentCache())),
      context_ (context)
  {
  }

  ~AutoDocumentCacheReset()
  {
    context_->setDocumentCache(oldDC);
  }

  DocumentCache *oldDC;

protected:
  DynamicContext* context_;
};

#endif
