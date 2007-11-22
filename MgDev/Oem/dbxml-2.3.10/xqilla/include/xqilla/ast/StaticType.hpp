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
 * $Id: StaticType.hpp,v 1.6 2006/11/02 15:07:56 jpcs Exp $
 */

#ifndef _STATICTYPE_HPP
#define _STATICTYPE_HPP

#include <xqilla/framework/XQillaExport.hpp>
#include <xqilla/items/AnyAtomicType.hpp>

/**
 * Class that represents the static type of a sub-expression
 */
class XQILLA_API StaticType {
public:
  /**
   * Flags that determine what item types are returned from a sub-expression
   */
  enum StaticTypeFlags {
    DOCUMENT_TYPE            = 0x00000001,
    ELEMENT_TYPE             = 0x00000002,
    ATTRIBUTE_TYPE           = 0x00000004,
    TEXT_TYPE                = 0x00000008,
    PI_TYPE                  = 0x00000010,
    COMMENT_TYPE             = 0x00000020,
    NAMESPACE_TYPE           = 0x00000040,

    ANY_SIMPLE_TYPE          = 0x00000080,
    ANY_URI_TYPE             = 0x00000100,
    BASE_64_BINARY_TYPE      = 0x00000200,
    BOOLEAN_TYPE             = 0x00000400,
    DATE_TYPE                = 0x00000800,
    DATE_TIME_TYPE           = 0x00001000,
    DAY_TIME_DURATION_TYPE   = 0x00002000,
    DECIMAL_TYPE             = 0x00004000,
    DOUBLE_TYPE              = 0x00008000,
    DURATION_TYPE            = 0x00010000,
    FLOAT_TYPE               = 0x00020000,
    G_DAY_TYPE               = 0x00040000,
    G_MONTH_TYPE             = 0x00080000,
    G_MONTH_DAY_TYPE         = 0x00100000,
    G_YEAR_TYPE              = 0x00200000,
    G_YEAR_MONTH_TYPE        = 0x00400000,
    HEX_BINARY_TYPE          = 0x00800000,
    NOTATION_TYPE            = 0x01000000,
    QNAME_TYPE               = 0x02000000,
    STRING_TYPE              = 0x04000000,
    TIME_TYPE                = 0x08000000,
    UNTYPED_ATOMIC_TYPE      = 0x10000000,
    YEAR_MONTH_DURATION_TYPE = 0x20000000,

    NODE_TYPE                = (DOCUMENT_TYPE | ELEMENT_TYPE | ATTRIBUTE_TYPE | TEXT_TYPE | PI_TYPE | COMMENT_TYPE |
                                NAMESPACE_TYPE),

    NUMERIC_TYPE             = (DECIMAL_TYPE | FLOAT_TYPE | DOUBLE_TYPE),

    TIMEZONE_TYPE            = (DATE_TYPE | DATE_TIME_TYPE | G_DAY_TYPE | G_MONTH_TYPE | G_MONTH_DAY_TYPE |
                                G_YEAR_TYPE | G_YEAR_MONTH_TYPE | TIME_TYPE),

    TYPED_ATOMIC_TYPE        = (NUMERIC_TYPE | TIMEZONE_TYPE | ANY_SIMPLE_TYPE | ANY_URI_TYPE | BASE_64_BINARY_TYPE |
                                BOOLEAN_TYPE | DAY_TIME_DURATION_TYPE | DURATION_TYPE | HEX_BINARY_TYPE |
                                NOTATION_TYPE | QNAME_TYPE | STRING_TYPE | YEAR_MONTH_DURATION_TYPE),

    ANY_ATOMIC_TYPE          = (TYPED_ATOMIC_TYPE | UNTYPED_ATOMIC_TYPE),

    ITEM_TYPE                = (NODE_TYPE | ANY_ATOMIC_TYPE),

    EMPTY_TYPE               = 0
  };

  StaticType() : flags(0) {}
  StaticType(unsigned int f) : flags(f) {}

  static unsigned int getFlagsFor(const XMLCh *uri, const XMLCh *name, const StaticContext *context,
                                  bool &isExact);
  static unsigned int getFlagsFor(AnyAtomicType::AtomicObjectType primitiveType);

  void typeUnion(const StaticType &st);
  void typeIntersect(const StaticType &st);

  bool containsType(unsigned int type) const;
  bool isType(unsigned int type) const;

  unsigned int flags;
};

#endif
