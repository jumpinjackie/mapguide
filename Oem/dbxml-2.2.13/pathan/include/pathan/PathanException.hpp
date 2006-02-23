/*
 * Copyright (c) 2002, DecisionSoft Limited All rights reserved.
 * Please see LICENSE.TXT for more information.
 */

#ifndef __PATHANEXCEPTION_HPP
#define __PATHANEXCEPTION_HPP

#include <pathan/Pathan.hpp>
#include <xercesc/dom/DOMXPathException.hpp>
#include <xercesc/util/XMLString.hpp>

/**
 * The <code>PathanException</code> interface extends the <code>DOMXPathException</code>
 * allowing an additional exception of type <code>RUNTIME_ERR</code> to be thrown.
 * @since DOM Level 3
 */
class PATHAN_EXPORT PathanException: public XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  /** @name Constructors */
  //@{
  /**
   * Constructor which takes an error code and a message.
   *
   * @param code The error code which indicates the exception
   * @param message The string containing the error message
   */
  PathanException(short code, const XMLCh* message): 
      XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException(code, XERCES_CPP_NAMESPACE_QUALIFIER XMLString::replicate(message)) {};

  /**
   * Copy constructor.
   *
   * @param other The object to be copied.
   */
  PathanException(const PathanException &other);
  //@}

  // -----------------------------------------------------------------------
  //  Destructors
  // -----------------------------------------------------------------------
  /** @name Destructor */
  //@{
  /**
   * Destructor for PathanException.
   *
   */
  virtual ~PathanException();
  //@}
  
  //@{
  /**
   * ExceptionCode
   * RUNTIME_ERR If Pathan encounters a runtime exception
   */
  enum valExtension {
     RUNTIME_ERR = 53
   };
  //@}

  // -----------------------------------------------------------------------
  //  Getter methods
  // -----------------------------------------------------------------------
  
  /** @name Getter methods */
  //@{  

  /**
   * Returns the code associated with this exception.
   *
   * @return the exception code
   */
  virtual XERCES_CPP_NAMESPACE_QUALIFIER DOMXPathException::ExceptionCode getCode(void) const;
  
  /**
   * Returns the string representing the exception's error message.
   *
   * @return the error message
   */
  virtual const XMLCh* getString(void) const;
  //@}
  
  /** @name Getter methods */
  //@{  
  /**
   * Statically returns whether or not PathanExceptions have debug on
   *
   * @return debug
   */
  static bool getDebug(void);
  //@}

  // -----------------------------------------------------------------------
  //  Setter methods
  // -----------------------------------------------------------------------
  
  /** @name Setter methods */
  //@{
  /** Set the 'debug' flag
   *
   * This method allows users to enable or disable debug on PathanExceptions.
   *
   * @param debug The value specifying whether or not debug is enabled
   */
  static void setDebug(bool flag);
  //@}

private:
  static bool _debug;
};//PathanException

#endif //__PATHANEXCEPTION_HPP

