/*-
 * Copyright (c) 2004,2006 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlResults_dtor)
{
    php_dbxml_delete_XmlResults(rsrc->ptr);
}

/* {{{ proto XmlResults::XmlResults(object dbenv [, string name])
   Create a new container using an optional dbenv (can be null) and name */
PHP_DBXML_METHOD_BEGIN(XmlResults, XmlResults)
{
  zend_error(E_ERROR, "XmlResults can not be instantiated from PHP");
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlValue XmlResults::next()
   Gets next value */
PHP_DBXML_METHOD_BEGIN(XmlResults, next)
{
  PHP_DBXML_STUFFED_THIS(XmlResults);

  XmlValue v;
  if (This.next(v)) {
    php_dbxml_set_XmlValue_object_factory(return_value, v TSRMLS_CC);
  } else {
    RETURN_FALSE;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlValue XmlResults::previous()
   Gets previous value */
PHP_DBXML_METHOD_BEGIN(XmlResults, previous)
{
  PHP_DBXML_STUFFED_THIS(XmlResults);

  XmlValue v;
  if (This.previous(v)) {
    php_dbxml_set_XmlValue_object_factory(return_value, v TSRMLS_CC);
  } else {
    RETURN_FALSE;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto XmlValue XmlResults::peek()
   peeks next value */
PHP_DBXML_METHOD_BEGIN(XmlResults, peek)
{
  PHP_DBXML_STUFFED_THIS(XmlResults);

  XmlValue v;
  if (This.peek(v)) {
    php_dbxml_set_XmlValue_object_factory(return_value, v TSRMLS_CC);
  } else {
    RETURN_FALSE;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

 /* {{{ proto void XmlResults::reset()
   rewind to start of results */
PHP_DBXML_METHOD_BEGIN(XmlResults, reset)
{
  PHP_DBXML_STUFFED_THIS(XmlResults);
  This.reset();
} PHP_DBXML_METHOD_END()
/* }}} */

PHP_DBXML_METHOD_BEGIN(XmlResults, getEvaluationType)
{
  PHP_DBXML_GETTHIS(XmlResults);
  if (ZEND_NUM_ARGS()) {
    WRONG_PARAM_COUNT;
  }
  RETURN_LONG(This->getEvaluationType());
} PHP_DBXML_METHOD_END()

/* {{{ proto XmlValue XmlResults::add()
   Adds a new value */
PHP_DBXML_METHOD_BEGIN(XmlResults, add)
{
  PHP_DBXML_STUFFED_THIS(XmlResults);
  zval *zv;
  if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O",
				       &zv, php_dbxml_XmlValue_ce)) {
    XmlValue v = php_dbxml_get_XmlValue_object_pointer(zv TSRMLS_CC);
    This.add(v);
  } else {
    WRONG_PARAM_COUNT;
  }
} PHP_DBXML_METHOD_END()
/* }}} */

#define BOOL_GETTER(method) \
PHP_DBXML_METHOD_BEGIN(XmlResults, method) \
{ \
  PHP_DBXML_STUFFED_THIS(XmlResults); \
  if (ZEND_NUM_ARGS()) { \
    WRONG_PARAM_COUNT; \
  } \
  RETURN_BOOL(This.method()); \
} PHP_DBXML_METHOD_END()

BOOL_GETTER(hasNext);
BOOL_GETTER(hasPrevious);

function_entry php_dbxml_XmlResults_methods[] = {
  PHP_ME(XmlResults, XmlResults, NULL, 0)
  PHP_ME(XmlResults, next, NULL, 0)
  PHP_ME(XmlResults, previous, NULL, 0)
  PHP_ME(XmlResults, peek, NULL, 0)
  PHP_ME(XmlResults, hasNext, NULL, 0)
  PHP_ME(XmlResults, hasPrevious, NULL, 0)
  PHP_ME(XmlResults, reset, NULL, 0)
  PHP_ME(XmlResults, getEvaluationType,       NULL, 0)
  PHP_ME(XmlResults, add, NULL, 0)
  {NULL,NULL,NULL}
};
