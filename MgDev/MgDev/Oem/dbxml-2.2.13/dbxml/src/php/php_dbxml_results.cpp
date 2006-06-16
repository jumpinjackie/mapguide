/*-
 *  Copyright (c) 2004
 *  Sleepycat Software.  All rights reserved.
 *
 *  http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 *  authors: Wez Furlong <wez@omniti.com>  
 *           George Schlossnagle <george@omniti.com>
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

/* {{{ proto bool XmlResults::hasNext()
   determine if there is a next value */
PHP_DBXML_METHOD_BEGIN(XmlResults, hasNext)
{
  PHP_DBXML_STUFFED_THIS(XmlResults);
  RETURN_BOOL(This.hasNext());
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto bool XmlResults::hasPrevious()
   determine if there is a next value */
PHP_DBXML_METHOD_BEGIN(XmlResults, hasPrevious)
{
  PHP_DBXML_STUFFED_THIS(XmlResults);
  RETURN_BOOL(This.hasPrevious());
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


function_entry php_dbxml_XmlResults_methods[] = {
  PHP_ME(XmlResults, XmlResults, NULL, 0)
  PHP_ME(XmlResults, next, NULL, 0)
  PHP_ME(XmlResults, previous, NULL, 0)
  PHP_ME(XmlResults, peek, NULL, 0)
  PHP_ME(XmlResults, hasNext, NULL, 0)
  PHP_ME(XmlResults, hasPrevious, NULL, 0)
  PHP_ME(XmlResults, reset, NULL, 0)
  {NULL,NULL,NULL}
};
