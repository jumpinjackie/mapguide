/*
 * Copyright (c) 2003, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __XPATH2NODESERIALIZER_HPP
#define __XPATH2NODESERIALIZER_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/dom/impl/DOMWriterImpl.hpp>

XERCES_CPP_NAMESPACE_BEGIN

class DOMNode;
class XMLFormatter;

XERCES_CPP_NAMESPACE_END

class PATHAN_EXPORT XPath2NodeSerializer: public XERCES_CPP_NAMESPACE_QUALIFIER DOMWriterImpl {
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

