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
 * $Id: AnyAtomicType.hpp,v 1.7 2006/11/01 16:37:14 jpcs Exp $
 */

#ifndef _ANYATOMICTYPE_HPP
#define _ANYATOMICTYPE_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/Item.hpp>
#include <xercesc/util/XercesDefs.hpp>

class DynamicContext;
class StaticContext;

class XQILLA_API AnyAtomicType: public Item
{

public:
  typedef RefCountPointer<const AnyAtomicType> Ptr;

  /* isAtomicValue from Item */
  virtual bool isAtomicValue() const;

  /* isNode from Item */
  virtual bool isNode() const;

  /* is this type numeric?  Return false by default */
  virtual bool isNumericValue() const;

  /* is this type date or time based?  Return false by default */
  virtual bool isDateOrTimeTypeValue() const;

  /* Get the namespace uri of the primitive type (basic type) of this type */
  virtual const XMLCh* getPrimitiveTypeURI() const;

  /* Get the name of the primitive type (basic type) of this type (ie "decimal" for xs:decimal) */
  virtual const XMLCh* getPrimitiveTypeName() const = 0;

  /* Get the namespace URI for this type */
  virtual const XMLCh* getTypeURI() const = 0;

  /* Get the name of this type  (ie "integer" for xs:integer) */
  virtual const XMLCh* getTypeName() const = 0;

  /* If possible, cast this type to the target type  -- not virtual, this is the single entry point for casting */
  AnyAtomicType::Ptr castAs(const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const;

  /* Test if this type can be cast to the target type */
  virtual bool castable(const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const;

  /* returns the XMLCh* (canonical) representation of this type */
  virtual const XMLCh* asString(const DynamicContext* context) const = 0;

  /* returns true if the two objects are equal (whatever that means 
   * in the context of the datatype), false otherwise */
  virtual bool equals(const AnyAtomicType::Ptr &target, const DynamicContext* context) const = 0;

  /* Returns true if this typeName and uri match the given typeName and uri */
  virtual bool isOfType(const XMLCh* targetURI, const XMLCh* targetType, const DynamicContext* context) const;

  /* Returns true if this typeName and uri match the given typeName and uri,
   * or if any of this type's parents match the given typeName and uri */
  virtual bool isInstanceOfType(const XMLCh* targetURI, const XMLCh* targetType, const StaticContext* context) const;
  
  enum AtomicObjectType {
    ANY_SIMPLE_TYPE    = 0,
    ANY_URI            = 1,
    BASE_64_BINARY     = 2,
    BOOLEAN            = 3,
    DATE               = 4,
    DATE_TIME          = 5,
    DAY_TIME_DURATION  = 6,
    DECIMAL            = 7,
    DOUBLE             = 8,
    DURATION           = 9,
    FLOAT              = 10,
    G_DAY              = 11,
    G_MONTH            = 12,
    G_MONTH_DAY        = 13,
    G_YEAR             = 14,
    G_YEAR_MONTH       = 15,
    HEX_BINARY         = 16,
    NOTATION           = 17,
    QNAME              = 18,
    STRING             = 19,
    TIME               = 20,
    UNTYPED_ATOMIC     = 21,
    YEAR_MONTH_DURATION= 22,
    NumAtomicObjectTypes= 23
  };

  /* same ordering as in cast table in F&O, for debug
   * enum AtomicObjectType {
    UNTYPED_ATOMIC     = 0,
    ANY_SIMPLE_TYPE    = 1,
    STRING             = 2,
    FLOAT              = 3,
    DOUBLE             = 4,
    DECIMAL            = 5,
    DURATION           = 6,
    YEAR_MONTH_DURATION= 7,
    DAY_TIME_DURATION  = 8,
    DATE_TIME          = 9,
    TIME               = 10,
    DATE               = 11,
    G_YEAR_MONTH       = 12,
    G_YEAR             = 13,
    G_MONTH_DAY        = 14,
    G_DAY              = 15,
    G_MONTH            = 16,
    BOOLEAN            = 17,
    BASE_64_BINARY     = 18,
    HEX_BINARY         = 19,
    ANY_URI            = 20,
    QNAME              = 21,
    NOTATION           = 22,
    NumAtomicObjectTypes= 23
  };*/

  /**
   * Returns true if 
   * (a) both the input type and the target type are built-in schema types and 
   *     cast is supported for the combination.
   * (b) the input type is a derived atomic type and the 
   *     target type is a supertype of the input type
   * (c) the target type is a derived atomic type and the 
   *     input type is xs:string, xs:anySimpleType, or a supertype of the 
   *     target type.
   * (d) If a primitive type P1 can be cast into a primitive type P2, then any 
   *     subtype of P1 can be cast into any subtype of P2
  **/
  bool castIsSupported(AtomicObjectType targetIndex, const DynamicContext* context) const;

  virtual AtomicObjectType getPrimitiveTypeIndex() const = 0;
 
  static const XMLCh fgDT_ANYATOMICTYPE[];
  static const XMLCh fgDT_ANYATOMICTYPE_XERCESHASH[];

protected:
  /* internal castAs method.  This one is virtual and does the real work */
  virtual AnyAtomicType::Ptr castAsInternal(AtomicObjectType targetIndex, const XMLCh* targetURI,
                                            const XMLCh* targetType, const DynamicContext* context) const;

private:

  //friend class CastTable;
  //template <class TYPE> friend class DatatypeFactoryTemplate;

  // class that implements the Casting Table
  class CastTable {
    public: 
        CastTable();
        bool getCell(AtomicObjectType source, 
                     AtomicObjectType target) const;
    private:
        bool staticCastTable[NumAtomicObjectTypes][NumAtomicObjectTypes];
  };


  // reproduction of the XPath2 casting table: http://www.w3.org/TR/xquery-operators/#casting
  static const CastTable staticCastTable;  
};

#endif //  _ANYATOMICTYPE_HPP
