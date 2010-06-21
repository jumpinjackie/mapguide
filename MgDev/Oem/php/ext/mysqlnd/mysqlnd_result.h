/*
  +----------------------------------------------------------------------+
  | PHP Version 6                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2009 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Georg Richter <georg@mysql.com>                             |
  |          Andrey Hristov <andrey@mysql.com>                           |
  |          Ulf Wendel <uwendel@mysql.com>                              |
  +----------------------------------------------------------------------+
*/

/* $Id: mysqlnd_result.h 272370 2008-12-31 11:15:49Z sebastian $ */

#ifndef MYSQLND_RESULT_H
#define MYSQLND_RESULT_H

MYSQLND_RES *mysqlnd_result_init(unsigned int field_count, MYSQLND_THD_ZVAL_PCACHE *cache TSRMLS_DC);

void mysqlnd_unbuffered_free_last_data(MYSQLND_RES *result TSRMLS_DC);

enum_func_status
mysqlnd_store_result_fetch_data(MYSQLND * const conn, MYSQLND_RES *result,
								MYSQLND_RES_METADATA *meta,
								zend_bool binary_protocol,
								zend_bool to_cache TSRMLS_DC);

enum_func_status mysqlnd_query_read_result_set_header(MYSQLND *conn, MYSQLND_STMT *stmt TSRMLS_DC);

void mysqlnd_res_initialize_result_set_rest(MYSQLND_RES * const result TSRMLS_DC);


#ifdef MYSQLND_THREADED
void * mysqlnd_fetch_thread(void *arg);
#endif

enum_func_status mysqlnd_background_store_result_fetch_data(MYSQLND_RES *result TSRMLS_DC);
#endif /* MYSQLND_RESULT_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
