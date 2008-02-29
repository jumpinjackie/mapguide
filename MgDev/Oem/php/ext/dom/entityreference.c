/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2007 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id: entityreference.c,v 1.12.2.1.2.2 2007/01/01 09:36:00 sebastian Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"


/*
* class DOMEntityReference extends DOMNode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-11C98490
* Since: 
*/

zend_function_entry php_dom_entityreference_class_functions[] = {
	PHP_ME(domentityreference, __construct, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto void DOMEntityReference::__construct(string name); */
PHP_METHOD(domentityreference, __construct)
{
	zval *id;
	xmlNode *node;
	xmlNodePtr oldnode = NULL;
	dom_object *intern;
	char *name;
	int name_len, name_valid;

	php_set_error_handling(EH_THROW, dom_domexception_class_entry TSRMLS_CC);
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &id, dom_entityreference_class_entry, &name, &name_len) == FAILURE) {
		php_std_error_handling();
		return;
	}

	php_std_error_handling();

	name_valid = xmlValidateName((xmlChar *) name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, 1 TSRMLS_CC);
		RETURN_FALSE;
	}

	node = xmlNewReference(NULL, name);

	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, 1 TSRMLS_CC);
		RETURN_FALSE;
	}

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL) {
		oldnode = dom_object_get_node(intern);
		if (oldnode != NULL) {
			php_libxml_node_free_resource(oldnode  TSRMLS_CC);
		}
		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, node, (void *)intern TSRMLS_CC);
	}
}

/* }}} end DOMEntityReference::__construct */
#endif
