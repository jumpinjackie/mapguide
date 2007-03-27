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

/* $Id: domconfiguration.c,v 1.5.2.1.2.1 2007/01/01 09:36:00 sebastian Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"


/*
* class domdomconfiguration 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMConfiguration
* Since: DOM Level 3
*/

zend_function_entry php_dom_domconfiguration_class_functions[] = {
	PHP_FALIAS(setParameter, dom_domconfiguration_set_parameter, NULL)
	PHP_FALIAS(getParameter, dom_domconfiguration_get_parameter, NULL)
	PHP_FALIAS(canSetParameter, dom_domconfiguration_can_set_parameter, NULL)
	{NULL, NULL, NULL}
};

/* {{{ attribute protos, not implemented yet */


/* {{{ proto dom_void dom_domconfiguration_set_parameter(string name, domuserdata value);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMConfiguration-property
Since: 
*/
PHP_FUNCTION(dom_domconfiguration_set_parameter)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domconfiguration_set_parameter */


/* {{{ proto domdomuserdata dom_domconfiguration_get_parameter(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMConfiguration-getParameter
Since: 
*/
PHP_FUNCTION(dom_domconfiguration_get_parameter)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domconfiguration_get_parameter */


/* {{{ proto boolean dom_domconfiguration_can_set_parameter(string name, domuserdata value);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMConfiguration-canSetParameter
Since: 
*/
PHP_FUNCTION(dom_domconfiguration_can_set_parameter)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domconfiguration_can_set_parameter */
#endif
