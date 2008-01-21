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

/* $Id: string_extend.c,v 1.5.2.1.2.1 2007/01/01 09:36:00 sebastian Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"


/*
* class domstringextend 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#i18n-methods-StringExtend
* Since: 
*/

zend_function_entry php_dom_string_extend_class_functions[] = {
	PHP_FALIAS(findOffset16, dom_string_extend_find_offset16, NULL)
	PHP_FALIAS(findOffset32, dom_string_extend_find_offset32, NULL)
	{NULL, NULL, NULL}
};

/* {{{ attribute protos, not implemented yet */


/* {{{ proto int dom_string_extend_find_offset16(int offset32);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#i18n-methods-StringExtend-findOffset16
Since: 
*/
PHP_FUNCTION(dom_string_extend_find_offset16)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_string_extend_find_offset16 */


/* {{{ proto int dom_string_extend_find_offset32(int offset16);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#i18n-methods-StringExtend-findOffset32
Since: 
*/
PHP_FUNCTION(dom_string_extend_find_offset32)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_string_extend_find_offset32 */
#endif
