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
  | Author: George Schlossnagle <george@omniti.com>                      |
  +----------------------------------------------------------------------+
*/

/* $Id: php_pdo_mysql.h,v 1.3.2.1.2.1 2007/01/01 09:36:05 sebastian Exp $ */

#ifndef PHP_PDO_MYSQL_H
#define PHP_PDO_MYSQL_H

extern zend_module_entry pdo_mysql_module_entry;
#define phpext_pdo_mysql_ptr &pdo_mysql_module_entry

#ifdef PHP_WIN32
#define PHP_PDO_MYSQL_API __declspec(dllexport)
#else
#define PHP_PDO_MYSQL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(pdo_mysql);
PHP_MSHUTDOWN_FUNCTION(pdo_mysql);
PHP_RINIT_FUNCTION(pdo_mysql);
PHP_RSHUTDOWN_FUNCTION(pdo_mysql);
PHP_MINFO_FUNCTION(pdo_mysql);

#endif	/* PHP_PDO_MYSQL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
