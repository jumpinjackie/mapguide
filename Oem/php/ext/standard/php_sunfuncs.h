/* 
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.0 of the PHP license,       |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_0.txt.                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Moshe Doron <mosdoron@netvision.net.il>                      |
   +----------------------------------------------------------------------+
*/

/* $Id: php_sunfuncs.h,v 1.3 2004/01/08 17:32:51 sniper Exp $ */

#ifndef PHP_SUNFUNCS_H
#define PHP_SUNFUNCS_H

/* default ini entries: */
/* Jerusalem one. */
#define DATE_DEFAULT_LATITUDE "31.7667"
#define DATE_DEFAULT_LONGITUDE "35.2333"

/* on 90'50; common jewish sunset declaration (start of sun body appear) */
#define DATE_SUNSET_ZENITH "90.83"

/* on 90'50; common jewish  sunrise declaration (sun body disappeared) */
#define DATE_SUNRISE_ZENITH "90.83"

#define SUNFUNCS_RET_TIMESTAMP 0
#define SUNFUNCS_RET_STRING 1
#define SUNFUNCS_RET_DOUBLE 2

PHP_FUNCTION(date_sunrise);
PHP_FUNCTION(date_sunset);

#endif /* PHP_SUNFUNCS_H */
