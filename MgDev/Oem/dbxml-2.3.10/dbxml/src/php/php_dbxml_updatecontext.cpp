/*-
 * Copyright (c) 2004,2006 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlUpdateContext_dtor)
{
    php_dbxml_delete_XmlUpdateContext(rsrc->ptr);
}

/* {{{ proto XmlUpdateContext::XmlUpdateContext()
   Create a new container using an optional dbenv (can be null) and name */
PHP_DBXML_METHOD_BEGIN(XmlUpdateContext, XmlUpdateContext)
{
  zend_error(E_ERROR, "XmlUpdateContext can not be instantiated from PHP");
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlUpdateContext::setApplyChangesToContainers(bool apply)
   */
PHP_DBXML_METHOD_BEGIN(XmlUpdateContext, setApplyChangesToContainers)
{
  zend_bool b;
  PHP_DBXML_STUFFED_THIS(XmlUpdateContext);
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "b", &b)) {
    RETURN_FALSE;
  }
  This.setApplyChangesToContainers(b);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto bool XmlUpdateContext::getApplyChangesToContainers()
   */
PHP_DBXML_METHOD_BEGIN(XmlUpdateContext, getApplyChangesToContainers)
{
  PHP_DBXML_STUFFED_THIS(XmlUpdateContext);
  if (ZEND_NUM_ARGS()) {
    RETURN_FALSE;
  }
  RETURN_BOOL(This.getApplyChangesToContainers());
} PHP_DBXML_METHOD_END()
/* }}} */

function_entry php_dbxml_XmlUpdateContext_methods[] = {
  PHP_ME(XmlUpdateContext, XmlUpdateContext, NULL, 0)
  PHP_ME(XmlUpdateContext, setApplyChangesToContainers, NULL, 0)
  PHP_ME(XmlUpdateContext, getApplyChangesToContainers, NULL, 0)
  {NULL,NULL,NULL}
};
