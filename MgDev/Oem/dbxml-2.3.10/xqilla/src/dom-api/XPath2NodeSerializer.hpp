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
 * $Id: XPath2NodeSerializer.hpp,v 1.4 2006/11/01 16:37:18 jpcs Exp $
 */

#ifndef __XPATH2NODESERIALIZER_HPP
#define __XPATH2NODESERIALIZER_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xercesc/dom/impl/DOMWriterImpl.hpp>

XERCES_CPP_NAMESPACE_BEGIN

class DOMNode;
class XMLFormatter;

XERCES_CPP_NAMESPACE_END

class XQILLA_API XPath2NodeSerializer: public XERCES_CPP_NAMESPACE_QUALIFIER DOMWriterImpl {
public:
  /** @name Constructor */
  //@{
  
  /**
   * Constructor.
   * 
   * Constructs an XPath2NodeSerializer 
   * 
   */
  XPath2NodeSerializer(XERCES_CPP_NAMESPACE_QUALIFIER MemoryManager* const manager = XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::fgMemoryManager);

  // -----------------------------------------------------------------------
  //  All constructors are hidden, just the destructor is available
  // -----------------------------------------------------------------------
  /** @name Destructor */
  //@{
  /**
   * Destructor
   *
   */
  virtual ~XPath2NodeSerializer();
  //@}

protected:

  virtual bool customNodeSerialize(const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* const nodeToWrite, int level);

};

#endif // __XPATH2NODESERIALIZER_HPP

