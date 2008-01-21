/*-
 * Copyright (c) 2004,2006 Oracle.  All rights reserved.
 *
 * http://www.apache.org/licenses/LICENSE-2.0.txt
 * 
 * authors: Wez Furlong <wez@omniti.com>  
 *          George Schlossnagle <george@omniti.com>
 */

#include "php_dbxml_int.hpp"

ZEND_RSRC_DTOR_FUNC(php_dbxml_XmlModify_dtor)
{
    php_dbxml_delete_XmlModify(rsrc->ptr);
}

/* {{{ proto XmlModify::XmlModify()
   Create a new container using an optional dbenv (can be null) and name */
PHP_DBXML_METHOD_BEGIN(XmlModify, XmlModify)
{
  zend_error(E_ERROR, "XmlModify can not be instantiated from PHP");
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlModify::addInsertBeforeStep(XmlQueryExpression exp, int XmlObjectType, string name, string content)
   */
PHP_DBXML_METHOD_BEGIN(XmlModify, addInsertBeforeStep)
{
  zval *zexp, *zres;
  long type;
  char *name, *content;
  long name_len, content_len;
  PHP_DBXML_STUFFED_THIS(XmlModify);
  
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "Olss",
					  &zexp, php_dbxml_XmlQueryExpression_ce,
					  &type, &name, &name_len, &content,
					  &content_len)) {

    XmlQueryExpression expr =
	    php_dbxml_get_XmlQueryExpression_object_pointer(zexp TSRMLS_CC);
    This.addInsertBeforeStep(expr, (XmlModify::XmlObject)type,
			     std::string(name, name_len),
			     std::string(content, content_len));
  } else if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OlsO",
					   &zexp, php_dbxml_XmlQueryExpression_ce,
					   &type, &name, &name_len,
					   &zres, php_dbxml_XmlResults_ce)) {
    XmlQueryExpression expr =
	    php_dbxml_get_XmlQueryExpression_object_pointer(zexp TSRMLS_CC);
    XmlResults res =
	    php_dbxml_get_XmlResults_object_pointer(zres TSRMLS_CC);
    This.addInsertBeforeStep(expr, (XmlModify::XmlObject)type,
			     std::string(name, name_len), res);
  } else
    RETURN_FALSE;
  
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlModify::addInsertAfterStep(XmlQueryExpression exp, int XmlObjectType, string name, string content)
   */
PHP_DBXML_METHOD_BEGIN(XmlModify, addInsertAfterStep)
{
  zval *zexp, *zres;
  long type;
  char *name, *content;
  long name_len, content_len;
  PHP_DBXML_STUFFED_THIS(XmlModify);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "Olss",
					  &zexp, php_dbxml_XmlQueryExpression_ce,
					  &type, &name, &name_len, &content,
					  &content_len)) {

    XmlQueryExpression expr =
	    php_dbxml_get_XmlQueryExpression_object_pointer(zexp TSRMLS_CC);
    This.addInsertAfterStep(expr, (XmlModify::XmlObject)type,
			    std::string(name, name_len),
			    std::string(content, content_len));
  } else if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OlsO",
					      &zexp,
					      php_dbxml_XmlQueryExpression_ce,
					      &type, &name, &name_len,
					      &zres, php_dbxml_XmlResults_ce)) {
    XmlQueryExpression expr =
	    php_dbxml_get_XmlQueryExpression_object_pointer(zexp TSRMLS_CC);
    XmlResults res =
	    php_dbxml_get_XmlResults_object_pointer(zres TSRMLS_CC);
    This.addInsertAfterStep(expr, (XmlModify::XmlObject)type,
			    std::string(name, name_len), res);
  } else
    RETURN_FALSE;
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlModify::addAppendStep(XmlQueryExpression exp, int XmlObjectType, string name, string content [, int location])
   */
PHP_DBXML_METHOD_BEGIN(XmlModify, addAppendStep)
{
  zval *zexp, *zres;
  long type, location = -1;
  char *name, *content;
  long name_len, content_len;
  PHP_DBXML_STUFFED_THIS(XmlModify);

  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET,
					  ZEND_NUM_ARGS() TSRMLS_CC, "Olss|l",
					  &zexp, php_dbxml_XmlQueryExpression_ce,
					  &type, &name, &name_len, &content,
					  &content_len, &location)) {

    XmlQueryExpression expr =
	    php_dbxml_get_XmlQueryExpression_object_pointer(zexp TSRMLS_CC);
    This.addAppendStep(expr, (XmlModify::XmlObject)type,
		       std::string(name, name_len),
		       std::string(content, content_len), location);
  } else if (SUCCESS == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "OlsO|l",
					      &zexp,
					      php_dbxml_XmlQueryExpression_ce,
					      &type, &name, &name_len,
					      &zres, php_dbxml_XmlResults_ce,
					      &location)) {
    XmlQueryExpression expr =
	    php_dbxml_get_XmlQueryExpression_object_pointer(zexp TSRMLS_CC);
    XmlResults res =
	    php_dbxml_get_XmlResults_object_pointer(zres TSRMLS_CC);
    This.addAppendStep(expr, (XmlModify::XmlObject)type,
			    std::string(name, name_len), res, location);
  } else
    RETURN_FALSE;
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlModify::addUpdateStep(XmlQueryExpression exp, string content)
   */
PHP_DBXML_METHOD_BEGIN(XmlModify, addUpdateStep)
{
  zval *zexp;
  char *content;
  long content_len;
  PHP_DBXML_STUFFED_THIS(XmlModify);
  
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &zexp, php_dbxml_XmlQueryExpression_ce,
      &content, &content_len)) {
    RETURN_FALSE;
  }
  
  XmlQueryExpression expr = php_dbxml_get_XmlQueryExpression_object_pointer(zexp TSRMLS_CC);
  This.addUpdateStep(expr, std::string(content, content_len));
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlModify::addRenameStep(XmlQueryExpression exp, string newname)
   */
PHP_DBXML_METHOD_BEGIN(XmlModify, addRenameStep)
{
  zval *zexp;
  char *content;
  long content_len;
  PHP_DBXML_STUFFED_THIS(XmlModify);
  
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Os", &zexp, php_dbxml_XmlQueryExpression_ce, 
      &content, &content_len)) {
    RETURN_FALSE;
  }
  
  XmlQueryExpression expr = php_dbxml_get_XmlQueryExpression_object_pointer(zexp TSRMLS_CC);
  This.addRenameStep(expr, std::string(content, content_len));
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlModify::addRemoveStep(XmlQueryExpression exp)
   */
PHP_DBXML_METHOD_BEGIN(XmlModify, addRemoveStep)
{
  zval *zexp;
  PHP_DBXML_STUFFED_THIS(XmlModify);
  
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O", &zexp, php_dbxml_XmlQueryExpression_ce)) {
    RETURN_FALSE;
  }
  
  XmlQueryExpression expr = php_dbxml_get_XmlQueryExpression_object_pointer(zexp TSRMLS_CC);
  This.addRemoveStep(expr);
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlModify::setNewEncoding(string encoding)
   */
PHP_DBXML_METHOD_BEGIN(XmlModify, setNewEncoding)
{
  char *encoding;
  int encoding_len;
  PHP_DBXML_STUFFED_THIS(XmlModify);
  
  if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &encoding, &encoding_len)) {
    RETURN_FALSE;
  }
  
  This.setNewEncoding(std::string(encoding, encoding_len));
} PHP_DBXML_METHOD_END()
/* }}} */

/* {{{ proto void XmlModify::execute(...)
   */
PHP_DBXML_METHOD_BEGIN(XmlModify, execute)
{
  char *encoding;
  int encoding_len;
  zval *zv, *zqc, *zuc, *zt;
  PHP_DBXML_STUFFED_THIS(XmlModify);
  
  if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
      "OOO", &zv, php_dbxml_XmlValue_ce, &zqc, php_dbxml_XmlQueryContext_ce, &zuc, php_dbxml_XmlUpdateContext_ce)) {

    XmlValue v = php_dbxml_get_XmlValue_object_pointer(zv TSRMLS_CC);
    XmlQueryContext c = php_dbxml_get_XmlQueryContext_object_pointer(zqc TSRMLS_CC);
    XmlUpdateContext u = php_dbxml_get_XmlUpdateContext_object_pointer(zuc TSRMLS_CC);

    RETURN_LONG(This.execute(v, c, u));

  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
      "OOO", &zv, php_dbxml_XmlResults_ce, &zqc, php_dbxml_XmlQueryContext_ce, &zuc, php_dbxml_XmlUpdateContext_ce)) {

    XmlResults r = php_dbxml_get_XmlResults_object_pointer(zv TSRMLS_CC);
    XmlQueryContext c = php_dbxml_get_XmlQueryContext_object_pointer(zqc TSRMLS_CC);
    XmlUpdateContext u = php_dbxml_get_XmlUpdateContext_object_pointer(zuc TSRMLS_CC);

    RETURN_LONG(This.execute(r, c, u));

  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
      "OOOO", &zt, php_dbxml_XmlTransaction_ce, &zv, php_dbxml_XmlValue_ce, &zqc, php_dbxml_XmlQueryContext_ce, &zuc, php_dbxml_XmlUpdateContext_ce)) {

    XmlValue v = php_dbxml_get_XmlValue_object_pointer(zv TSRMLS_CC);
    XmlQueryContext c = php_dbxml_get_XmlQueryContext_object_pointer(zqc TSRMLS_CC);
    XmlUpdateContext u = php_dbxml_get_XmlUpdateContext_object_pointer(zuc TSRMLS_CC);
    XmlTransaction t = php_dbxml_get_XmlTransaction_object_pointer(zt TSRMLS_CC);

    RETURN_LONG(This.execute(t, v, c, u));


  } else if (SUCCESS == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,
      "OOOO", &zt, php_dbxml_XmlTransaction_ce, &zv, php_dbxml_XmlResults_ce, &zqc, php_dbxml_XmlQueryContext_ce, &zuc, php_dbxml_XmlUpdateContext_ce)) {

    XmlResults r = php_dbxml_get_XmlResults_object_pointer(zv TSRMLS_CC);
    XmlQueryContext c = php_dbxml_get_XmlQueryContext_object_pointer(zqc TSRMLS_CC);
    XmlUpdateContext u = php_dbxml_get_XmlUpdateContext_object_pointer(zuc TSRMLS_CC);
    XmlTransaction t = php_dbxml_get_XmlTransaction_object_pointer(zt TSRMLS_CC);

    RETURN_LONG(This.execute(t, r, c, u));


  } else {
    WRONG_PARAM_COUNT;
  }
} PHP_DBXML_METHOD_END()
/* }}} */



function_entry php_dbxml_XmlModify_methods[] = {
  PHP_ME(XmlModify, XmlModify, NULL, 0)
  PHP_ME(XmlModify, addInsertBeforeStep, NULL, 0)
  PHP_ME(XmlModify, addInsertAfterStep, NULL, 0)
  PHP_ME(XmlModify, addAppendStep, NULL, 0)
  PHP_ME(XmlModify, addUpdateStep, NULL, 0)
  PHP_ME(XmlModify, addRenameStep, NULL, 0)
  PHP_ME(XmlModify, addRemoveStep, NULL, 0)
  PHP_ME(XmlModify, setNewEncoding, NULL, 0)
  PHP_ME(XmlModify, execute, NULL, 0)
  {NULL,NULL,NULL}
};
