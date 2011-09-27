/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2011 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   |          Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
 */

/* $Id: session.c 314376 2011-08-06 14:47:44Z felipe $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#ifdef PHP_WIN32
# include "win32/winutil.h"
# include "win32/time.h"
#else
#include <sys/time.h>
#endif

#include <sys/stat.h>
#include <fcntl.h>

#include "php_ini.h"
#include "SAPI.h"
#include "php_session.h"
#include "ext/standard/md5.h"
#include "ext/standard/sha1.h"
#include "ext/standard/php_var.h"
#include "ext/date/php_date.h"
#include "ext/standard/php_lcg.h"
#include "ext/standard/url_scanner_ex.h"
#include "ext/standard/php_rand.h" /* for RAND_MAX */
#include "ext/standard/info.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/url.h"

#include "mod_files.h"
#include "mod_user.h"

#ifdef HAVE_LIBMM
#include "mod_mm.h"
#endif

PHPAPI ZEND_DECLARE_MODULE_GLOBALS(ps);

/* ***********
   * Helpers *
   *********** */

#define IF_SESSION_VARS() \
	if (PS(http_session_vars) && PS(http_session_vars)->type == IS_ARRAY)

#define SESSION_CHECK_ACTIVE_STATE	\
	if (PS(session_status) == php_session_active) {	\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "A session is active. You cannot change the session module's ini settings at this time");	\
		return FAILURE;	\
	}

/* Dispatched by RINIT and by php_session_destroy */
static inline void php_rinit_session_globals(TSRMLS_D) /* {{{ */
{
	PS(id) = NULL;
	PS(session_status) = php_session_none;
	PS(mod_data) = NULL;
	/* Do NOT init PS(mod_user_names) here! */
	PS(http_session_vars) = NULL;
}
/* }}} */

/* Dispatched by RSHUTDOWN and by php_session_destroy */
static inline void php_rshutdown_session_globals(TSRMLS_D) /* {{{ */
{
	if (PS(http_session_vars)) {
		zval_ptr_dtor(&PS(http_session_vars));
		PS(http_session_vars) = NULL;
	}
	/* Do NOT destroy PS(mod_user_names) here! */
	if (PS(mod_data)) {
		zend_try {
			PS(mod)->s_close(&PS(mod_data) TSRMLS_CC);
		} zend_end_try();
	}
	if (PS(id)) {
		efree(PS(id));
	}
}
/* }}} */

static int php_session_destroy(TSRMLS_D) /* {{{ */
{
	int retval = SUCCESS;

	if (PS(session_status) != php_session_active) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Trying to destroy uninitialized session");
		return FAILURE;
	}

	if (PS(mod)->s_destroy(&PS(mod_data), PS(id) TSRMLS_CC) == FAILURE) {
		retval = FAILURE;
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Session object destruction failed");
	}

	php_rshutdown_session_globals(TSRMLS_C);
	php_rinit_session_globals(TSRMLS_C);

	return retval;
}
/* }}} */

PHPAPI void php_add_session_var(char *name, size_t namelen TSRMLS_DC) /* {{{ */
{
	zval **sym_track = NULL;

	IF_SESSION_VARS() {
		zend_hash_find(Z_ARRVAL_P(PS(http_session_vars)), name, namelen + 1, (void *) &sym_track);
	} else {
		return;
	}

	/* Set up a proper reference between $_SESSION["x"] and $x. */

	if (PG(register_globals)) {
		zval **sym_global = NULL;

		if (zend_hash_find(&EG(symbol_table), name, namelen + 1, (void *) &sym_global) == SUCCESS) {
			if ((Z_TYPE_PP(sym_global) == IS_ARRAY && Z_ARRVAL_PP(sym_global) == &EG(symbol_table)) || *sym_global == PS(http_session_vars)) {
				return;
			}
		}

		if (sym_global == NULL && sym_track == NULL) {
			zval *empty_var;

			ALLOC_INIT_ZVAL(empty_var); /* this sets refcount to 1 */
			Z_SET_REFCOUNT_P(empty_var, 0); /* our module does not maintain a ref */
			/* The next call will increase refcount by NR_OF_SYM_TABLES==2 */
			zend_set_hash_symbol(empty_var, name, namelen, 1, 2, Z_ARRVAL_P(PS(http_session_vars)), &EG(symbol_table));
		} else if (sym_global == NULL) {
			SEPARATE_ZVAL_IF_NOT_REF(sym_track);
			zend_set_hash_symbol(*sym_track, name, namelen, 1, 1, &EG(symbol_table));
		} else if (sym_track == NULL) {
			SEPARATE_ZVAL_IF_NOT_REF(sym_global);
			zend_set_hash_symbol(*sym_global, name, namelen, 1, 1, Z_ARRVAL_P(PS(http_session_vars)));
		}
	} else {
		if (sym_track == NULL) {
			zval *empty_var;

			ALLOC_INIT_ZVAL(empty_var);
			ZEND_SET_SYMBOL_WITH_LENGTH(Z_ARRVAL_P(PS(http_session_vars)), name, namelen+1, empty_var, 1, 0);
		}
	}
}
/* }}} */

PHPAPI void php_set_session_var(char *name, size_t namelen, zval *state_val, php_unserialize_data_t *var_hash TSRMLS_DC) /* {{{ */
{
	if (PG(register_globals)) {
		zval **old_symbol;
		if (zend_hash_find(&EG(symbol_table),name,namelen+1,(void *)&old_symbol) == SUCCESS) {
			if ((Z_TYPE_PP(old_symbol) == IS_ARRAY && Z_ARRVAL_PP(old_symbol) == &EG(symbol_table)) || *old_symbol == PS(http_session_vars)) {
				return;
			}

			/* A global symbol with the same name exists already. That
			 * symbol might have been created by other means (e.g. $_GET).
			 *
			 * hash_update in zend_set_hash_symbol is not good, because
			 * it will leave referenced variables (such as local instances
			 * of a global variable) dangling.
			 *
			 * BTW: if you use register_globals references between
			 * session-vars won't work because of this very reason! */

			REPLACE_ZVAL_VALUE(old_symbol,state_val,1);

			/* The following line will update the reference table used for
			 * unserialization.  It is optional, because some storage
			 * formats may not be able to represent references. */

			if (var_hash) {
				PHP_VAR_UNSERIALIZE_ZVAL_CHANGED(var_hash,state_val,*old_symbol);
			}

			zend_set_hash_symbol(*old_symbol, name, namelen, 1, 1, Z_ARRVAL_P(PS(http_session_vars)));
		} else {
			zend_set_hash_symbol(state_val, name, namelen, 1, 2, Z_ARRVAL_P(PS(http_session_vars)), &EG(symbol_table));
		}
	} else IF_SESSION_VARS() {
		zend_set_hash_symbol(state_val, name, namelen, PZVAL_IS_REF(state_val), 1, Z_ARRVAL_P(PS(http_session_vars)));
	}
}
/* }}} */

PHPAPI int php_get_session_var(char *name, size_t namelen, zval ***state_var TSRMLS_DC) /* {{{ */
{
	int ret = FAILURE;

	IF_SESSION_VARS() {
		ret = zend_hash_find(Z_ARRVAL_P(PS(http_session_vars)), name, namelen + 1, (void **) state_var);

		/* If register_globals is enabled, and
		 * if there is an entry for the slot in $_SESSION, and
		 * if that entry is still set to NULL, and
		 * if the global var exists, then
		 * we prefer the same key in the global sym table. */

		if (PG(register_globals) && ret == SUCCESS && Z_TYPE_PP(*state_var) == IS_NULL) {
			zval **tmp;

			if (zend_hash_find(&EG(symbol_table), name, namelen + 1, (void **) &tmp) == SUCCESS) {
				*state_var = tmp;
			}
		}
	}
	return ret;
}
/* }}} */

static void php_session_track_init(TSRMLS_D) /* {{{ */
{
	zval *session_vars = NULL;

	/* Unconditionally destroy existing arrays -- possible dirty data */
	zend_delete_global_variable("HTTP_SESSION_VARS", sizeof("HTTP_SESSION_VARS")-1 TSRMLS_CC);
	zend_delete_global_variable("_SESSION", sizeof("_SESSION")-1 TSRMLS_CC);

	if (PS(http_session_vars)) {
		zval_ptr_dtor(&PS(http_session_vars));
	}

	MAKE_STD_ZVAL(session_vars);
	array_init(session_vars);
	PS(http_session_vars) = session_vars;

	if (PG(register_long_arrays)) {
		ZEND_SET_GLOBAL_VAR_WITH_LENGTH("HTTP_SESSION_VARS", sizeof("HTTP_SESSION_VARS"), PS(http_session_vars), 3, 1);
		ZEND_SET_GLOBAL_VAR_WITH_LENGTH("_SESSION", sizeof("_SESSION"), PS(http_session_vars), 3, 1);
	}
	else {
		ZEND_SET_GLOBAL_VAR_WITH_LENGTH("_SESSION", sizeof("_SESSION"), PS(http_session_vars), 2, 1);
	}
}
/* }}} */

static char *php_session_encode(int *newlen TSRMLS_DC) /* {{{ */
{
	char *ret = NULL;

	IF_SESSION_VARS() {
		if (!PS(serializer)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown session.serialize_handler. Failed to encode session object");
			ret = NULL;
		} else if (PS(serializer)->encode(&ret, newlen TSRMLS_CC) == FAILURE) {
			ret = NULL;
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot encode non-existent session");
	}
	return ret;
}
/* }}} */

static void php_session_decode(const char *val, int vallen TSRMLS_DC) /* {{{ */
{
	if (!PS(serializer)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unknown session.serialize_handler. Failed to decode session object");
		return;
	}
	if (PS(serializer)->decode(val, vallen TSRMLS_CC) == FAILURE) {
		php_session_destroy(TSRMLS_C);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to decode session object. Session has been destroyed");
	}
}
/* }}} */

/*
 * Note that we cannot use the BASE64 alphabet here, because
 * it contains "/" and "+": both are unacceptable for simple inclusion
 * into URLs.
 */

static char hexconvtab[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ,-";

enum {
	PS_HASH_FUNC_MD5,
	PS_HASH_FUNC_SHA1,
	PS_HASH_FUNC_OTHER
};

/* returns a pointer to the byte after the last valid character in out */
static char *bin_to_readable(char *in, size_t inlen, char *out, char nbits) /* {{{ */
{
	unsigned char *p, *q;
	unsigned short w;
	int mask;
	int have;

	p = (unsigned char *) in;
	q = (unsigned char *)in + inlen;

	w = 0;
	have = 0;
	mask = (1 << nbits) - 1;

	while (1) {
		if (have < nbits) {
			if (p < q) {
				w |= *p++ << have;
				have += 8;
			} else {
				/* consumed everything? */
				if (have == 0) break;
				/* No? We need a final round */
				have = nbits;
			}
		}

		/* consume nbits */
		*out++ = hexconvtab[w & mask];
		w >>= nbits;
		have -= nbits;
	}

	*out = '\0';
	return out;
}
/* }}} */

PHPAPI char *php_session_create_id(PS_CREATE_SID_ARGS) /* {{{ */
{
	PHP_MD5_CTX md5_context;
	PHP_SHA1_CTX sha1_context;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
	void *hash_context;
#endif
	unsigned char *digest;
	int digest_len;
	int j;
	char *buf, *outid;
	struct timeval tv;
	zval **array;
	zval **token;
	char *remote_addr = NULL;

	gettimeofday(&tv, NULL);

	if (zend_hash_find(&EG(symbol_table), "_SERVER", sizeof("_SERVER"), (void **) &array) == SUCCESS &&
		Z_TYPE_PP(array) == IS_ARRAY &&
		zend_hash_find(Z_ARRVAL_PP(array), "REMOTE_ADDR", sizeof("REMOTE_ADDR"), (void **) &token) == SUCCESS
	) {
		remote_addr = Z_STRVAL_PP(token);
	}

	/* maximum 15+19+19+10 bytes */
	spprintf(&buf, 0, "%.15s%ld%ld%0.8F", remote_addr ? remote_addr : "", tv.tv_sec, (long int)tv.tv_usec, php_combined_lcg(TSRMLS_C) * 10);

	switch (PS(hash_func)) {
		case PS_HASH_FUNC_MD5:
			PHP_MD5Init(&md5_context);
			PHP_MD5Update(&md5_context, (unsigned char *) buf, strlen(buf));
			digest_len = 16;
			break;
		case PS_HASH_FUNC_SHA1:
			PHP_SHA1Init(&sha1_context);
			PHP_SHA1Update(&sha1_context, (unsigned char *) buf, strlen(buf));
			digest_len = 20;
			break;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
		case PS_HASH_FUNC_OTHER:
			if (!PS(hash_ops)) {
				php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid session hash function");
				efree(buf);
				return NULL;
			}

			hash_context = emalloc(PS(hash_ops)->context_size);
			PS(hash_ops)->hash_init(hash_context);
			PS(hash_ops)->hash_update(hash_context, (unsigned char *) buf, strlen(buf));
			digest_len = PS(hash_ops)->digest_size;
			break;
#endif /* HAVE_HASH_EXT */
		default:
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Invalid session hash function");
			efree(buf);
			return NULL;
	}
	efree(buf);

	if (PS(entropy_length) > 0) {
#ifdef PHP_WIN32
		unsigned char rbuf[2048];
		size_t toread = PS(entropy_length);

		if (php_win32_get_random_bytes(rbuf, (size_t) toread) == SUCCESS){

			switch (PS(hash_func)) {
				case PS_HASH_FUNC_MD5:
					PHP_MD5Update(&md5_context, rbuf, toread);
					break;
				case PS_HASH_FUNC_SHA1:
					PHP_SHA1Update(&sha1_context, rbuf, toread);
					break;
# if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
				case PS_HASH_FUNC_OTHER:
					PS(hash_ops)->hash_update(hash_context, rbuf, toread);
					break;
# endif /* HAVE_HASH_EXT */
			}
		}
#else
		int fd;

		fd = VCWD_OPEN(PS(entropy_file), O_RDONLY);
		if (fd >= 0) {
			unsigned char rbuf[2048];
			int n;
			int to_read = PS(entropy_length);

			while (to_read > 0) {
				n = read(fd, rbuf, MIN(to_read, sizeof(rbuf)));
				if (n <= 0) break;

				switch (PS(hash_func)) {
					case PS_HASH_FUNC_MD5:
						PHP_MD5Update(&md5_context, rbuf, n);
						break;
					case PS_HASH_FUNC_SHA1:
						PHP_SHA1Update(&sha1_context, rbuf, n);
						break;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
					case PS_HASH_FUNC_OTHER:
						PS(hash_ops)->hash_update(hash_context, rbuf, n);
						break;
#endif /* HAVE_HASH_EXT */
				}
				to_read -= n;
			}
			close(fd);
		}
#endif
	}

	digest = emalloc(digest_len + 1);
	switch (PS(hash_func)) {
		case PS_HASH_FUNC_MD5:
			PHP_MD5Final(digest, &md5_context);
			break;
		case PS_HASH_FUNC_SHA1:
			PHP_SHA1Final(digest, &sha1_context);
			break;
#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
		case PS_HASH_FUNC_OTHER:
			PS(hash_ops)->hash_final(digest, hash_context);
			efree(hash_context);
			break;
#endif /* HAVE_HASH_EXT */
	}

	if (PS(hash_bits_per_character) < 4
			|| PS(hash_bits_per_character) > 6) {
		PS(hash_bits_per_character) = 4;

		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The ini setting hash_bits_per_character is out of range (should be 4, 5, or 6) - using 4 for now");
	}
	
	outid = emalloc((size_t)((digest_len + 2) * ((8.0f / PS(hash_bits_per_character)) + 0.5)));
	j = (int) (bin_to_readable((char *)digest, digest_len, outid, (char)PS(hash_bits_per_character)) - outid);
	efree(digest);

	if (newlen) {
		*newlen = j;
	}

	return outid;
}
/* }}} */

static void php_session_initialize(TSRMLS_D) /* {{{ */
{
	char *val;
	int vallen;

	/* check session name for invalid characters */
	if (PS(id) && strpbrk(PS(id), "\r\n\t <>'\"\\")) {
		efree(PS(id));
		PS(id) = NULL;
	}

	if (!PS(mod)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "No storage module chosen - failed to initialize session");
		return;
	}

	/* Open session handler first */
	if (PS(mod)->s_open(&PS(mod_data), PS(save_path), PS(session_name) TSRMLS_CC) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Failed to initialize storage module: %s (path: %s)", PS(mod)->s_name, PS(save_path));
		return;
	}

	/* If there is no ID, use session module to create one */
	if (!PS(id)) {
new_session:
		PS(id) = PS(mod)->s_create_sid(&PS(mod_data), NULL TSRMLS_CC);
		if (PS(use_cookies)) {
			PS(send_cookie) = 1;
		}
	}

	/* Read data */
	/* Question: if you create a SID here, should you also try to read data?
	 * I'm not sure, but while not doing so will remove one session operation
	 * it could prove usefull for those sites which wish to have "default"
	 * session information. */
	php_session_track_init(TSRMLS_C);
	PS(invalid_session_id) = 0;
	if (PS(mod)->s_read(&PS(mod_data), PS(id), &val, &vallen TSRMLS_CC) == SUCCESS) {
		php_session_decode(val, vallen TSRMLS_CC);
		efree(val);
	} else if (PS(invalid_session_id)) { /* address instances where the session read fails due to an invalid id */
		PS(invalid_session_id) = 0;
		efree(PS(id));
		PS(id) = NULL;
		goto new_session;
	}
}
/* }}} */

static int migrate_global(HashTable *ht, HashPosition *pos TSRMLS_DC) /* {{{ */
{
	char *str;
	uint str_len;
	ulong num_key;
	int n;
	zval **val;
	int ret = 0;

	n = zend_hash_get_current_key_ex(ht, &str, &str_len, &num_key, 0, pos);

	switch (n) {
		case HASH_KEY_IS_STRING:
			if (zend_hash_find(&EG(symbol_table), str, str_len, (void **) &val) == SUCCESS &&
				val && Z_TYPE_PP(val) != IS_NULL
			) {
				ZEND_SET_SYMBOL_WITH_LENGTH(ht, str, str_len, *val, Z_REFCOUNT_PP(val) + 1, 1);
				ret = 1;
			}
			break;
		case HASH_KEY_IS_LONG:
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "The session bug compatibility code will not "
					"try to locate the global variable $%lu due to its "
					"numeric nature", num_key);
			break;
	}
	return ret;
}
/* }}} */

static void php_session_save_current_state(TSRMLS_D) /* {{{ */
{
	int ret = FAILURE;

	IF_SESSION_VARS() {
		if (PS(bug_compat) && !PG(register_globals)) {
			HashTable *ht = Z_ARRVAL_P(PS(http_session_vars));
			HashPosition pos;
			zval **val;
			int do_warn = 0;

			zend_hash_internal_pointer_reset_ex(ht, &pos);

			while (zend_hash_get_current_data_ex(ht, (void **) &val, &pos) != FAILURE) {
				if (Z_TYPE_PP(val) == IS_NULL) {
					if (migrate_global(ht, &pos TSRMLS_CC)) {
						do_warn = 1;
					}
				}
				zend_hash_move_forward_ex(ht, &pos);
			}

			if (do_warn && PS(bug_compat_warn)) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Your script possibly relies on a session side-effect which existed until PHP 4.2.3. Please be advised that the session extension does not consider global variables as a source of data, unless register_globals is enabled. You can disable this functionality and this warning by setting session.bug_compat_42 or session.bug_compat_warn to off, respectively");
			}
		}

		if (PS(mod_data)) {
			char *val;
			int vallen;

			val = php_session_encode(&vallen TSRMLS_CC);
			if (val) {
				ret = PS(mod)->s_write(&PS(mod_data), PS(id), val, vallen TSRMLS_CC);
				efree(val);
			} else {
				ret = PS(mod)->s_write(&PS(mod_data), PS(id), "", 0 TSRMLS_CC);
			}
		}

		if (ret == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed to write session data (%s). Please "
					"verify that the current setting of session.save_path "
					"is correct (%s)",
					PS(mod)->s_name,
					PS(save_path));
		}
	}

	if (PS(mod_data)) {
		PS(mod)->s_close(&PS(mod_data) TSRMLS_CC);
	}
}
/* }}} */

/* *************************
   * INI Settings/Handlers *
   ************************* */

static PHP_INI_MH(OnUpdateSaveHandler) /* {{{ */
{
	ps_module *tmp;
	SESSION_CHECK_ACTIVE_STATE;

	tmp = _php_find_ps_module(new_value TSRMLS_CC);

	if (PG(modules_activated) && !tmp) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL TSRMLS_CC, err_type, "Cannot find save handler '%s'", new_value);
		}
		return FAILURE;
	}
	PS(mod) = tmp;

	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateSerializer) /* {{{ */
{
	const ps_serializer *tmp;
	SESSION_CHECK_ACTIVE_STATE;

	tmp = _php_find_ps_serializer(new_value TSRMLS_CC);

	if (PG(modules_activated) && !tmp) {
		int err_type;

		if (stage == ZEND_INI_STAGE_RUNTIME) {
			err_type = E_WARNING;
		} else {
			err_type = E_ERROR;
		}

		/* Do not output error when restoring ini options. */
		if (stage != ZEND_INI_STAGE_DEACTIVATE) {
			php_error_docref(NULL TSRMLS_CC, err_type, "Cannot find serialization handler '%s'", new_value);
		}
		return FAILURE;
	}
	PS(serializer) = tmp;

	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateTransSid) /* {{{ */
{
	SESSION_CHECK_ACTIVE_STATE;

	if (!strncasecmp(new_value, "on", sizeof("on"))) {
		PS(use_trans_sid) = (zend_bool) 1;
	} else {
		PS(use_trans_sid) = (zend_bool) atoi(new_value);
	}

	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateSaveDir) /* {{{ */
{
	/* Only do the safemode/open_basedir check at runtime */
	if (stage == PHP_INI_STAGE_RUNTIME || stage == PHP_INI_STAGE_HTACCESS) {
		char *p;

		if (memchr(new_value, '\0', new_value_length) != NULL) {
			return FAILURE;
		}

		/* we do not use zend_memrchr() since path can contain ; itself */
		if ((p = strchr(new_value, ';'))) {
			char *p2;
			p++;
			if ((p2 = strchr(p, ';'))) {
				p = p2 + 1;
			}
		} else {
			p = new_value;
		}

		if (PG(safe_mode) && *p && (!php_checkuid(p, NULL, CHECKUID_CHECK_FILE_AND_DIR))) {
			return FAILURE;
		}

		if (PG(open_basedir) && *p && php_check_open_basedir(p TSRMLS_CC)) {
			return FAILURE;
		}
	}

	OnUpdateString(entry, new_value, new_value_length, mh_arg1, mh_arg2, mh_arg3, stage TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

static PHP_INI_MH(OnUpdateHashFunc) /* {{{ */
{
	long val;
	char *endptr = NULL;

#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH)
	PS(hash_ops) = NULL;
#endif

	val = strtol(new_value, &endptr, 10);
	if (endptr && (*endptr == '\0')) {
		/* Numeric value */
		PS(hash_func) = val ? 1 : 0;

		return SUCCESS;
	}

	if (new_value_length == (sizeof("md5") - 1) &&
		strncasecmp(new_value, "md5", sizeof("md5") - 1) == 0) {
		PS(hash_func) = PS_HASH_FUNC_MD5;

		return SUCCESS;
	}

	if (new_value_length == (sizeof("sha1") - 1) &&
		strncasecmp(new_value, "sha1", sizeof("sha1") - 1) == 0) {
		PS(hash_func) = PS_HASH_FUNC_SHA1;

		return SUCCESS;
	}

#if defined(HAVE_HASH_EXT) && !defined(COMPILE_DL_HASH) /* {{{ */
{
	php_hash_ops *ops = (php_hash_ops*)php_hash_fetch_ops(new_value, new_value_length);

	if (ops) {
		PS(hash_func) = PS_HASH_FUNC_OTHER;
		PS(hash_ops) = ops;

		return SUCCESS;
	}
}
#endif /* HAVE_HASH_EXT }}} */

	return FAILURE;
}
/* }}} */

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
	STD_PHP_INI_BOOLEAN("session.bug_compat_42",    "1",         PHP_INI_ALL, OnUpdateBool,   bug_compat,         php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.bug_compat_warn",  "1",         PHP_INI_ALL, OnUpdateBool,   bug_compat_warn,    php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.save_path",          "",          PHP_INI_ALL, OnUpdateSaveDir,save_path,          php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.name",               "PHPSESSID", PHP_INI_ALL, OnUpdateString, session_name,       php_ps_globals,    ps_globals)
	PHP_INI_ENTRY("session.save_handler",           "files",     PHP_INI_ALL, OnUpdateSaveHandler)
	STD_PHP_INI_BOOLEAN("session.auto_start",       "0",         PHP_INI_ALL, OnUpdateBool,   auto_start,         php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.gc_probability",     "1",         PHP_INI_ALL, OnUpdateLong,   gc_probability,     php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.gc_divisor",         "100",       PHP_INI_ALL, OnUpdateLong,   gc_divisor,         php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.gc_maxlifetime",     "1440",      PHP_INI_ALL, OnUpdateLong,   gc_maxlifetime,     php_ps_globals,    ps_globals)
	PHP_INI_ENTRY("session.serialize_handler",      "php",       PHP_INI_ALL, OnUpdateSerializer)
	STD_PHP_INI_ENTRY("session.cookie_lifetime",    "0",         PHP_INI_ALL, OnUpdateLong,   cookie_lifetime,    php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cookie_path",        "/",         PHP_INI_ALL, OnUpdateString, cookie_path,        php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cookie_domain",      "",          PHP_INI_ALL, OnUpdateString, cookie_domain,      php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.cookie_secure",    "",          PHP_INI_ALL, OnUpdateBool,   cookie_secure,      php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.cookie_httponly",  "",          PHP_INI_ALL, OnUpdateBool,   cookie_httponly,    php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_cookies",      "1",         PHP_INI_ALL, OnUpdateBool,   use_cookies,        php_ps_globals,    ps_globals)
	STD_PHP_INI_BOOLEAN("session.use_only_cookies", "1",         PHP_INI_ALL, OnUpdateBool,   use_only_cookies,   php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.referer_check",      "",          PHP_INI_ALL, OnUpdateString, extern_referer_chk, php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.entropy_file",       "",          PHP_INI_ALL, OnUpdateString, entropy_file,       php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.entropy_length",     "0",         PHP_INI_ALL, OnUpdateLong,   entropy_length,     php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cache_limiter",      "nocache",   PHP_INI_ALL, OnUpdateString, cache_limiter,      php_ps_globals,    ps_globals)
	STD_PHP_INI_ENTRY("session.cache_expire",       "180",       PHP_INI_ALL, OnUpdateLong,   cache_expire,       php_ps_globals,    ps_globals)
	PHP_INI_ENTRY("session.use_trans_sid",          "0",         PHP_INI_ALL, OnUpdateTransSid)
	PHP_INI_ENTRY("session.hash_function",          "0",         PHP_INI_ALL, OnUpdateHashFunc)
	STD_PHP_INI_ENTRY("session.hash_bits_per_character", "4",    PHP_INI_ALL, OnUpdateLong,   hash_bits_per_character, php_ps_globals, ps_globals)

	/* Commented out until future discussion */
	/* PHP_INI_ENTRY("session.encode_sources", "globals,track", PHP_INI_ALL, NULL) */
PHP_INI_END()
/* }}} */

/* ***************
   * Serializers *
   *************** */

#define PS_BIN_NR_OF_BITS 8
#define PS_BIN_UNDEF (1<<(PS_BIN_NR_OF_BITS-1))
#define PS_BIN_MAX (PS_BIN_UNDEF-1)

PS_SERIALIZER_ENCODE_FUNC(php_binary) /* {{{ */
{
	smart_str buf = {0};
	php_serialize_data_t var_hash;
	PS_ENCODE_VARS;

	PHP_VAR_SERIALIZE_INIT(var_hash);

	PS_ENCODE_LOOP(
			if (key_length > PS_BIN_MAX) continue;
			smart_str_appendc(&buf, (unsigned char) key_length);
			smart_str_appendl(&buf, key, key_length);
			php_var_serialize(&buf, struc, &var_hash TSRMLS_CC);
		} else {
			if (key_length > PS_BIN_MAX) continue;
			smart_str_appendc(&buf, (unsigned char) (key_length & PS_BIN_UNDEF));
			smart_str_appendl(&buf, key, key_length);
	);

	if (newlen) {
		*newlen = buf.len;
	}
	smart_str_0(&buf);
	*newstr = buf.c;
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	return SUCCESS;
}
/* }}} */

PS_SERIALIZER_DECODE_FUNC(php_binary) /* {{{ */
{
	const char *p;
	char *name;
	const char *endptr = val + vallen;
	zval *current;
	int namelen;
	int has_value;
	php_unserialize_data_t var_hash;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	for (p = val; p < endptr; ) {
		zval **tmp;
		namelen = ((unsigned char)(*p)) & (~PS_BIN_UNDEF);

		if (namelen < 0 || namelen > PS_BIN_MAX || (p + namelen) >= endptr) {
			return FAILURE;
		}

		has_value = *p & PS_BIN_UNDEF ? 0 : 1;

		name = estrndup(p + 1, namelen);

		p += namelen + 1;

		if (zend_hash_find(&EG(symbol_table), name, namelen + 1, (void **) &tmp) == SUCCESS) {
			if ((Z_TYPE_PP(tmp) == IS_ARRAY && Z_ARRVAL_PP(tmp) == &EG(symbol_table)) || *tmp == PS(http_session_vars)) {
				efree(name);
				continue;
			}
		}

		if (has_value) {
			ALLOC_INIT_ZVAL(current);
			if (php_var_unserialize(&current, (const unsigned char **) &p, (const unsigned char *) endptr, &var_hash TSRMLS_CC)) {
				php_set_session_var(name, namelen, current, &var_hash  TSRMLS_CC);
			}
			zval_ptr_dtor(&current);
		}
		PS_ADD_VARL(name, namelen);
		efree(name);
	}

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	return SUCCESS;
}
/* }}} */

#define PS_DELIMITER '|'
#define PS_UNDEF_MARKER '!'

PS_SERIALIZER_ENCODE_FUNC(php) /* {{{ */
{
	smart_str buf = {0};
	php_serialize_data_t var_hash;
	PS_ENCODE_VARS;

	PHP_VAR_SERIALIZE_INIT(var_hash);

	PS_ENCODE_LOOP(
			smart_str_appendl(&buf, key, key_length);
			if (memchr(key, PS_DELIMITER, key_length) || memchr(key, PS_UNDEF_MARKER, key_length)) {
				PHP_VAR_SERIALIZE_DESTROY(var_hash);
				smart_str_free(&buf);
				return FAILURE;
			}
			smart_str_appendc(&buf, PS_DELIMITER);

			php_var_serialize(&buf, struc, &var_hash TSRMLS_CC);
		} else {
			smart_str_appendc(&buf, PS_UNDEF_MARKER);
			smart_str_appendl(&buf, key, key_length);
			smart_str_appendc(&buf, PS_DELIMITER);
	);

	if (newlen) {
		*newlen = buf.len;
	}
	smart_str_0(&buf);
	*newstr = buf.c;

	PHP_VAR_SERIALIZE_DESTROY(var_hash);
	return SUCCESS;
}
/* }}} */

PS_SERIALIZER_DECODE_FUNC(php) /* {{{ */
{
	const char *p, *q;
	char *name;
	const char *endptr = val + vallen;
	zval *current;
	int namelen;
	int has_value;
	php_unserialize_data_t var_hash;

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	p = val;

	while (p < endptr) {
		zval **tmp;
		q = p;
		while (*q != PS_DELIMITER) {
			if (++q >= endptr) goto break_outer_loop;
		}
		if (p[0] == PS_UNDEF_MARKER) {
			p++;
			has_value = 0;
		} else {
			has_value = 1;
		}

		namelen = q - p;
		name = estrndup(p, namelen);
		q++;

		if (zend_hash_find(&EG(symbol_table), name, namelen + 1, (void **) &tmp) == SUCCESS) {
			if ((Z_TYPE_PP(tmp) == IS_ARRAY && Z_ARRVAL_PP(tmp) == &EG(symbol_table)) || *tmp == PS(http_session_vars)) {
				goto skip;
			}
		}

		if (has_value) {
			ALLOC_INIT_ZVAL(current);
			if (php_var_unserialize(&current, (const unsigned char **) &q, (const unsigned char *) endptr, &var_hash TSRMLS_CC)) {
				php_set_session_var(name, namelen, current, &var_hash  TSRMLS_CC);
			}
			zval_ptr_dtor(&current);
		}
		PS_ADD_VARL(name, namelen);
skip:
		efree(name);

		p = q;
	}
break_outer_loop:

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

	return SUCCESS;
}
/* }}} */

#define MAX_SERIALIZERS 10
#define PREDEFINED_SERIALIZERS 2

static ps_serializer ps_serializers[MAX_SERIALIZERS + 1] = {
	PS_SERIALIZER_ENTRY(php),
	PS_SERIALIZER_ENTRY(php_binary)
};

PHPAPI int php_session_register_serializer(const char *name, int (*encode)(PS_SERIALIZER_ENCODE_ARGS), int (*decode)(PS_SERIALIZER_DECODE_ARGS)) /* {{{ */
{
	int ret = -1;
	int i;

	for (i = 0; i < MAX_SERIALIZERS; i++) {
		if (ps_serializers[i].name == NULL) {
			ps_serializers[i].name = name;
			ps_serializers[i].encode = encode;
			ps_serializers[i].decode = decode;
			ps_serializers[i + 1].name = NULL;
			ret = 0;
			break;
		}
	}
	return ret;
}
/* }}} */

/* *******************
   * Storage Modules *
   ******************* */

#define MAX_MODULES 10
#define PREDEFINED_MODULES 2

static ps_module *ps_modules[MAX_MODULES + 1] = {
	ps_files_ptr,
	ps_user_ptr
};

PHPAPI int php_session_register_module(ps_module *ptr) /* {{{ */
{
	int ret = -1;
	int i;

	for (i = 0; i < MAX_MODULES; i++) {
		if (!ps_modules[i]) {
			ps_modules[i] = ptr;
			ret = 0;
			break;
		}
	}
	return ret;
}
/* }}} */

/* ******************
   * Cache Limiters *
   ****************** */

typedef struct {
	char *name;
	void (*func)(TSRMLS_D);
} php_session_cache_limiter_t;

#define CACHE_LIMITER(name) _php_cache_limiter_##name
#define CACHE_LIMITER_FUNC(name) static void CACHE_LIMITER(name)(TSRMLS_D)
#define CACHE_LIMITER_ENTRY(name) { #name, CACHE_LIMITER(name) },
#define ADD_HEADER(a) sapi_add_header(a, strlen(a), 1);
#define MAX_STR 512

static char *month_names[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char *week_days[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

static inline void strcpy_gmt(char *ubuf, time_t *when) /* {{{ */
{
	char buf[MAX_STR];
	struct tm tm, *res;
	int n;

	res = php_gmtime_r(when, &tm);

	if (!res) {
		buf[0] = '\0';
		return;
	}

	n = slprintf(buf, sizeof(buf), "%s, %02d %s %d %02d:%02d:%02d GMT", /* SAFE */
				week_days[tm.tm_wday], tm.tm_mday,
				month_names[tm.tm_mon], tm.tm_year + 1900,
				tm.tm_hour, tm.tm_min,
				tm.tm_sec);
	memcpy(ubuf, buf, n);
	ubuf[n] = '\0';
}
/* }}} */

static inline void last_modified(TSRMLS_D) /* {{{ */
{
	const char *path;
	struct stat sb;
	char buf[MAX_STR + 1];

	path = SG(request_info).path_translated;
	if (path) {
		if (VCWD_STAT(path, &sb) == -1) {
			return;
		}

#define LAST_MODIFIED "Last-Modified: "
		memcpy(buf, LAST_MODIFIED, sizeof(LAST_MODIFIED) - 1);
		strcpy_gmt(buf + sizeof(LAST_MODIFIED) - 1, &sb.st_mtime);
		ADD_HEADER(buf);
	}
}
/* }}} */

#define EXPIRES "Expires: "
CACHE_LIMITER_FUNC(public) /* {{{ */
{
	char buf[MAX_STR + 1];
	struct timeval tv;
	time_t now;

	gettimeofday(&tv, NULL);
	now = tv.tv_sec + PS(cache_expire) * 60;
	memcpy(buf, EXPIRES, sizeof(EXPIRES) - 1);
	strcpy_gmt(buf + sizeof(EXPIRES) - 1, &now);
	ADD_HEADER(buf);

	snprintf(buf, sizeof(buf) , "Cache-Control: public, max-age=%ld", PS(cache_expire) * 60); /* SAFE */
	ADD_HEADER(buf);

	last_modified(TSRMLS_C);
}
/* }}} */

CACHE_LIMITER_FUNC(private_no_expire) /* {{{ */
{
	char buf[MAX_STR + 1];

	snprintf(buf, sizeof(buf), "Cache-Control: private, max-age=%ld, pre-check=%ld", PS(cache_expire) * 60, PS(cache_expire) * 60); /* SAFE */
	ADD_HEADER(buf);

	last_modified(TSRMLS_C);
}
/* }}} */

CACHE_LIMITER_FUNC(private) /* {{{ */
{
	ADD_HEADER("Expires: Thu, 19 Nov 1981 08:52:00 GMT");
	CACHE_LIMITER(private_no_expire)(TSRMLS_C);
}
/* }}} */

CACHE_LIMITER_FUNC(nocache) /* {{{ */
{
	ADD_HEADER("Expires: Thu, 19 Nov 1981 08:52:00 GMT");

	/* For HTTP/1.1 conforming clients and the rest (MSIE 5) */
	ADD_HEADER("Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0");

	/* For HTTP/1.0 conforming clients */
	ADD_HEADER("Pragma: no-cache");
}
/* }}} */

static php_session_cache_limiter_t php_session_cache_limiters[] = {
	CACHE_LIMITER_ENTRY(public)
	CACHE_LIMITER_ENTRY(private)
	CACHE_LIMITER_ENTRY(private_no_expire)
	CACHE_LIMITER_ENTRY(nocache)
	{0}
};

static int php_session_cache_limiter(TSRMLS_D) /* {{{ */
{
	php_session_cache_limiter_t *lim;

	if (PS(cache_limiter)[0] == '\0') return 0;

	if (SG(headers_sent)) {
		char *output_start_filename = php_get_output_start_filename(TSRMLS_C);
		int output_start_lineno = php_get_output_start_lineno(TSRMLS_C);

		if (output_start_filename) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot send session cache limiter - headers already sent (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot send session cache limiter - headers already sent");
		}
		return -2;
	}

	for (lim = php_session_cache_limiters; lim->name; lim++) {
		if (!strcasecmp(lim->name, PS(cache_limiter))) {
			lim->func(TSRMLS_C);
			return 0;
		}
	}

	return -1;
}
/* }}} */

/* *********************
   * Cookie Management *
   ********************* */

#define COOKIE_SET_COOKIE "Set-Cookie: "
#define COOKIE_EXPIRES	"; expires="
#define COOKIE_PATH		"; path="
#define COOKIE_DOMAIN	"; domain="
#define COOKIE_SECURE	"; secure"
#define COOKIE_HTTPONLY	"; HttpOnly"

static void php_session_send_cookie(TSRMLS_D) /* {{{ */
{
	smart_str ncookie = {0};
	char *date_fmt = NULL;
	char *e_session_name, *e_id;

	if (SG(headers_sent)) {
		char *output_start_filename = php_get_output_start_filename(TSRMLS_C);
		int output_start_lineno = php_get_output_start_lineno(TSRMLS_C);

		if (output_start_filename) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot send session cookie - headers already sent by (output started at %s:%d)", output_start_filename, output_start_lineno);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot send session cookie - headers already sent");
		}
		return;
	}

	/* URL encode session_name and id because they might be user supplied */
	e_session_name = php_url_encode(PS(session_name), strlen(PS(session_name)), NULL);
	e_id = php_url_encode(PS(id), strlen(PS(id)), NULL);

	smart_str_appends(&ncookie, COOKIE_SET_COOKIE);
	smart_str_appends(&ncookie, e_session_name);
	smart_str_appendc(&ncookie, '=');
	smart_str_appends(&ncookie, e_id);

	efree(e_session_name);
	efree(e_id);

	if (PS(cookie_lifetime) > 0) {
		struct timeval tv;
		time_t t;

		gettimeofday(&tv, NULL);
		t = tv.tv_sec + PS(cookie_lifetime);

		if (t > 0) {
			date_fmt = php_format_date("D, d-M-Y H:i:s T", sizeof("D, d-M-Y H:i:s T")-1, t, 0 TSRMLS_CC);
			smart_str_appends(&ncookie, COOKIE_EXPIRES);
			smart_str_appends(&ncookie, date_fmt);
			efree(date_fmt);
		}
	}

	if (PS(cookie_path)[0]) {
		smart_str_appends(&ncookie, COOKIE_PATH);
		smart_str_appends(&ncookie, PS(cookie_path));
	}

	if (PS(cookie_domain)[0]) {
		smart_str_appends(&ncookie, COOKIE_DOMAIN);
		smart_str_appends(&ncookie, PS(cookie_domain));
	}

	if (PS(cookie_secure)) {
		smart_str_appends(&ncookie, COOKIE_SECURE);
	}

	if (PS(cookie_httponly)) {
		smart_str_appends(&ncookie, COOKIE_HTTPONLY);
	}

	smart_str_0(&ncookie);

	/*	'replace' must be 0 here, else a previous Set-Cookie
		header, probably sent with setcookie() will be replaced! */
	sapi_add_header_ex(ncookie.c, ncookie.len, 0, 0 TSRMLS_CC);
}
/* }}} */

PHPAPI ps_module *_php_find_ps_module(char *name TSRMLS_DC) /* {{{ */
{
	ps_module *ret = NULL;
	ps_module **mod;
	int i;

	for (i = 0, mod = ps_modules; i < MAX_MODULES; i++, mod++) {
		if (*mod && !strcasecmp(name, (*mod)->s_name)) {
			ret = *mod;
			break;
		}
	}
	return ret;
}
/* }}} */

PHPAPI const ps_serializer *_php_find_ps_serializer(char *name TSRMLS_DC) /* {{{ */
{
	const ps_serializer *ret = NULL;
	const ps_serializer *mod;

	for (mod = ps_serializers; mod->name; mod++) {
		if (!strcasecmp(name, mod->name)) {
			ret = mod;
			break;
		}
	}
	return ret;
}
/* }}} */

#define PPID2SID \
		convert_to_string((*ppid)); \
		PS(id) = estrndup(Z_STRVAL_PP(ppid), Z_STRLEN_PP(ppid))

static void php_session_reset_id(TSRMLS_D) /* {{{ */
{
	int module_number = PS(module_number);

	if (PS(use_cookies) && PS(send_cookie)) {
		php_session_send_cookie(TSRMLS_C);
		PS(send_cookie) = 0;
	}

	/* if the SID constant exists, destroy it. */
	zend_hash_del(EG(zend_constants), "sid", sizeof("sid"));

	if (PS(define_sid)) {
		smart_str var = {0};

		smart_str_appends(&var, PS(session_name));
		smart_str_appendc(&var, '=');
		smart_str_appends(&var, PS(id));
		smart_str_0(&var);
		REGISTER_STRINGL_CONSTANT("SID", var.c, var.len, 0);
	} else {
		REGISTER_STRINGL_CONSTANT("SID", STR_EMPTY_ALLOC(), 0, 0);
	}

	if (PS(apply_trans_sid)) {
		php_url_scanner_reset_vars(TSRMLS_C);
		php_url_scanner_add_var(PS(session_name), strlen(PS(session_name)), PS(id), strlen(PS(id)), 1 TSRMLS_CC);
	}
}
/* }}} */

PHPAPI void php_session_start(TSRMLS_D) /* {{{ */
{
	zval **ppid;
	zval **data;
	char *p, *value;
	int nrand;
	int lensess;

	if (PS(use_only_cookies)) {
		PS(apply_trans_sid) = 0;
	} else {
		PS(apply_trans_sid) = PS(use_trans_sid);
	}

	switch (PS(session_status)) {
		case php_session_active:
			php_error(E_NOTICE, "A session had already been started - ignoring session_start()");
			return;
			break;

		case php_session_disabled:
			value = zend_ini_string("session.save_handler", sizeof("session.save_handler"), 0);
			if (!PS(mod) && value) {
				PS(mod) = _php_find_ps_module(value TSRMLS_CC);
				if (!PS(mod)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot find save handler '%s' - session startup failed", value);
					return;
				}
			}
			value = zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler"), 0);
			if (!PS(serializer) && value) {
				PS(serializer) = _php_find_ps_serializer(value TSRMLS_CC);
				if (!PS(serializer)) {
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot find serialization handler '%s' - session startup failed", value);
					return;
				}
			}
			PS(session_status) = php_session_none;
			/* fallthrough */

		default:
		case php_session_none:
			PS(define_sid) = 1;
			PS(send_cookie) = 1;
	}

	lensess = strlen(PS(session_name));

	/* Cookies are preferred, because initially
	 * cookie and get variables will be available. */

	if (!PS(id)) {
		if (PS(use_cookies) && zend_hash_find(&EG(symbol_table), "_COOKIE", sizeof("_COOKIE"), (void **) &data) == SUCCESS &&
				Z_TYPE_PP(data) == IS_ARRAY &&
				zend_hash_find(Z_ARRVAL_PP(data), PS(session_name), lensess + 1, (void **) &ppid) == SUCCESS
		) {
			PPID2SID;
			PS(apply_trans_sid) = 0;
			PS(send_cookie) = 0;
			PS(define_sid) = 0;
		}

		if (!PS(use_only_cookies) && !PS(id) &&
				zend_hash_find(&EG(symbol_table), "_GET", sizeof("_GET"), (void **) &data) == SUCCESS &&
				Z_TYPE_PP(data) == IS_ARRAY &&
				zend_hash_find(Z_ARRVAL_PP(data), PS(session_name), lensess + 1, (void **) &ppid) == SUCCESS
		) {
			PPID2SID;
			PS(send_cookie) = 0;
		}

		if (!PS(use_only_cookies) && !PS(id) &&
				zend_hash_find(&EG(symbol_table), "_POST", sizeof("_POST"), (void **) &data) == SUCCESS &&
				Z_TYPE_PP(data) == IS_ARRAY &&
				zend_hash_find(Z_ARRVAL_PP(data), PS(session_name), lensess + 1, (void **) &ppid) == SUCCESS
		) {
			PPID2SID;
			PS(send_cookie) = 0;
		}
	}

	/* Check the REQUEST_URI symbol for a string of the form
	 * '<session-name>=<session-id>' to allow URLs of the form
	 * http://yoursite/<session-name>=<session-id>/script.php */

	if (!PS(use_only_cookies) && !PS(id) && PG(http_globals)[TRACK_VARS_SERVER] &&
			zend_hash_find(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER]), "REQUEST_URI", sizeof("REQUEST_URI"), (void **) &data) == SUCCESS &&
			Z_TYPE_PP(data) == IS_STRING &&
			(p = strstr(Z_STRVAL_PP(data), PS(session_name))) &&
			p[lensess] == '='
	) {
		char *q;

		p += lensess + 1;
		if ((q = strpbrk(p, "/?\\"))) {
			PS(id) = estrndup(p, q - p);
			PS(send_cookie) = 0;
		}
	}

	/* Check whether the current request was referred to by
	 * an external site which invalidates the previously found id. */

	if (PS(id) &&
			PS(extern_referer_chk)[0] != '\0' &&
			PG(http_globals)[TRACK_VARS_SERVER] &&
			zend_hash_find(Z_ARRVAL_P(PG(http_globals)[TRACK_VARS_SERVER]), "HTTP_REFERER", sizeof("HTTP_REFERER"), (void **) &data) == SUCCESS &&
			Z_TYPE_PP(data) == IS_STRING &&
			Z_STRLEN_PP(data) != 0 &&
			strstr(Z_STRVAL_PP(data), PS(extern_referer_chk)) == NULL
	) {
		efree(PS(id));
		PS(id) = NULL;
		PS(send_cookie) = 1;
		if (PS(use_trans_sid) && !PS(use_only_cookies)) {
			PS(apply_trans_sid) = 1;
		}
	}

	php_session_initialize(TSRMLS_C);

	if (!PS(use_cookies) && PS(send_cookie)) {
		if (PS(use_trans_sid) && !PS(use_only_cookies)) {
			PS(apply_trans_sid) = 1;
		}
		PS(send_cookie) = 0;
	}

	php_session_reset_id(TSRMLS_C);

	PS(session_status) = php_session_active;

	php_session_cache_limiter(TSRMLS_C);

	if (PS(mod_data) && PS(gc_probability) > 0) {
		int nrdels = -1;

		nrand = (int) ((float) PS(gc_divisor) * php_combined_lcg(TSRMLS_C));
		if (nrand < PS(gc_probability)) {
			PS(mod)->s_gc(&PS(mod_data), PS(gc_maxlifetime), &nrdels TSRMLS_CC);
#ifdef SESSION_DEBUG
			if (nrdels != -1) {
				php_error_docref(NULL TSRMLS_CC, E_NOTICE, "purged %d expired session objects", nrdels);
			}
#endif
		}
	}
}
/* }}} */

static void php_session_flush(TSRMLS_D) /* {{{ */
{
	if (PS(session_status) == php_session_active) {
		PS(session_status) = php_session_none;
		zend_try {
			php_session_save_current_state(TSRMLS_C);
		} zend_end_try();
	}
}
/* }}} */

PHPAPI void session_adapt_url(const char *url, size_t urllen, char **new, size_t *newlen TSRMLS_DC) /* {{{ */
{
	if (PS(apply_trans_sid) && (PS(session_status) == php_session_active)) {
		*new = php_url_scanner_adapt_single_url(url, urllen, PS(session_name), PS(id), newlen TSRMLS_CC);
	}
}
/* }}} */

/* ********************************
   * Userspace exported functions *
   ******************************** */

/* {{{ proto void session_set_cookie_params(int lifetime [, string path [, string domain [, bool secure[, bool httponly]]]])
   Set session cookie parameters */
static PHP_FUNCTION(session_set_cookie_params)
{
	zval **lifetime = NULL;
	char *path = NULL, *domain = NULL;
	int path_len, domain_len, argc = ZEND_NUM_ARGS();
	zend_bool secure = 0, httponly = 0;

	if (!PS(use_cookies) ||
		zend_parse_parameters(argc TSRMLS_CC, "Z|ssbb", &lifetime, &path, &path_len, &domain, &domain_len, &secure, &httponly) == FAILURE) {
		return;
	}

	convert_to_string_ex(lifetime);

	zend_alter_ini_entry("session.cookie_lifetime", sizeof("session.cookie_lifetime"), Z_STRVAL_PP(lifetime), Z_STRLEN_PP(lifetime), PHP_INI_USER, PHP_INI_STAGE_RUNTIME);

	if (path) {
		zend_alter_ini_entry("session.cookie_path", sizeof("session.cookie_path"), path, path_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
	if (domain) {
		zend_alter_ini_entry("session.cookie_domain", sizeof("session.cookie_domain"), domain, domain_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}

	if (argc > 3) {
		zend_alter_ini_entry("session.cookie_secure", sizeof("session.cookie_secure"), secure ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
	if (argc > 4) {
		zend_alter_ini_entry("session.cookie_httponly", sizeof("session.cookie_httponly"), httponly ? "1" : "0", 1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto array session_get_cookie_params(void)
   Return the session cookie parameters */
static PHP_FUNCTION(session_get_cookie_params)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	array_init(return_value);

	add_assoc_long(return_value, "lifetime", PS(cookie_lifetime));
	add_assoc_string(return_value, "path", PS(cookie_path), 1);
	add_assoc_string(return_value, "domain", PS(cookie_domain), 1);
	add_assoc_bool(return_value, "secure", PS(cookie_secure));
	add_assoc_bool(return_value, "httponly", PS(cookie_httponly));
}
/* }}} */

/* {{{ proto string session_name([string newname])
   Return the current session name. If newname is given, the session name is replaced with newname */
static PHP_FUNCTION(session_name)
{
	char *name = NULL;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	RETVAL_STRING(PS(session_name), 1);

	if (name) {
		zend_alter_ini_entry("session.name", sizeof("session.name"), name, name_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto string session_module_name([string newname])
   Return the current module name used for accessing session data. If newname is given, the module name is replaced with newname */
static PHP_FUNCTION(session_module_name)
{
	char *name = NULL;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	/* Set return_value to current module name */
	if (PS(mod) && PS(mod)->s_name) {
		RETVAL_STRING(safe_estrdup(PS(mod)->s_name), 0);
	} else {
		RETVAL_EMPTY_STRING();
	}

	if (name) {
		if (!_php_find_ps_module(name TSRMLS_CC)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot find named PHP session module (%s)", name);

			zval_dtor(return_value);
			RETURN_FALSE;
		}
		if (PS(mod_data)) {
			PS(mod)->s_close(&PS(mod_data) TSRMLS_CC);
		}
		PS(mod_data) = NULL;

		zend_alter_ini_entry("session.save_handler", sizeof("session.save_handler"), name, name_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto void session_set_save_handler(string open, string close, string read, string write, string destroy, string gc)
   Sets user-level functions */
static PHP_FUNCTION(session_set_save_handler)
{
	zval ***args = NULL;
	int i, num_args, argc = ZEND_NUM_ARGS();
	char *name;

	if (PS(session_status) != php_session_none) {
		RETURN_FALSE;
	}

	if (argc != 6) {
		WRONG_PARAM_COUNT;
	}

	if (zend_parse_parameters(argc TSRMLS_CC, "+", &args, &num_args) == FAILURE) {
		return;
	}

	for (i = 0; i < 6; i++) {
		if (!zend_is_callable(*args[i], 0, &name TSRMLS_CC)) {
			efree(args);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Argument %d is not a valid callback", i+1);
			efree(name);
			RETURN_FALSE;
		}
		efree(name);
	}

	zend_alter_ini_entry("session.save_handler", sizeof("session.save_handler"), "user", sizeof("user")-1, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);

	for (i = 0; i < 6; i++) {
		if (PS(mod_user_names).names[i] != NULL) {
			zval_ptr_dtor(&PS(mod_user_names).names[i]);
		}
		Z_ADDREF_PP(args[i]);
		PS(mod_user_names).names[i] = *args[i];
	}

	efree(args);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string session_save_path([string newname])
   Return the current save path passed to module_name. If newname is given, the save path is replaced with newname */
static PHP_FUNCTION(session_save_path)
{
	char *name = NULL;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	RETVAL_STRING(PS(save_path), 1);

	if (name) {
		if (memchr(name, '\0', name_len) != NULL) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The save_path cannot contain NULL characters");
			zval_dtor(return_value);
			RETURN_FALSE;
		}
		zend_alter_ini_entry("session.save_path", sizeof("session.save_path"), name, name_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto string session_id([string newid])
   Return the current session id. If newid is given, the session id is replaced with newid */
static PHP_FUNCTION(session_id)
{
	char *name = NULL;
	int name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &name, &name_len) == FAILURE) {
		return;
	}

	if (PS(id)) {
		RETVAL_STRING(PS(id), 1);
	} else {
		RETVAL_EMPTY_STRING();
	}

	if (name) {
		if (PS(id)) {
			efree(PS(id));
		}
		PS(id) = estrndup(name, name_len);
	}
}
/* }}} */

/* {{{ proto bool session_regenerate_id([bool delete_old_session])
   Update the current session id with a newly generated one. If delete_old_session is set to true, remove the old session. */
static PHP_FUNCTION(session_regenerate_id)
{
	zend_bool del_ses = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &del_ses) == FAILURE) {
		return;
	}

	if (SG(headers_sent)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot regenerate session id - headers already sent");
		RETURN_FALSE;
	}

	if (PS(session_status) == php_session_active) {
		if (PS(id)) {
			if (del_ses && PS(mod)->s_destroy(&PS(mod_data), PS(id) TSRMLS_CC) == FAILURE) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Session object destruction failed");
				RETURN_FALSE;
			}
			efree(PS(id));
			PS(id) = NULL;
		}

		PS(id) = PS(mod)->s_create_sid(&PS(mod_data), NULL TSRMLS_CC);

		PS(send_cookie) = 1;
		php_session_reset_id(TSRMLS_C);

		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string session_cache_limiter([string new_cache_limiter])
   Return the current cache limiter. If new_cache_limited is given, the current cache_limiter is replaced with new_cache_limiter */
static PHP_FUNCTION(session_cache_limiter)
{
	char *limiter = NULL;
	int limiter_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &limiter, &limiter_len) == FAILURE) {
		return;
	}

	RETVAL_STRING(PS(cache_limiter), 1);

	if (limiter) {
		zend_alter_ini_entry("session.cache_limiter", sizeof("session.cache_limiter"), limiter, limiter_len, PHP_INI_USER, PHP_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ proto int session_cache_expire([int new_cache_expire])
   Return the current cache expire. If new_cache_expire is given, the current cache_expire is replaced with new_cache_expire */
static PHP_FUNCTION(session_cache_expire)
{
	zval **expires = NULL;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "|Z", &expires) == FAILURE) {
		return;
	}

	RETVAL_LONG(PS(cache_expire));

	if (argc == 1) {
		convert_to_string_ex(expires);
		zend_alter_ini_entry("session.cache_expire", sizeof("session.cache_expire"), Z_STRVAL_PP(expires), Z_STRLEN_PP(expires), ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME);
	}
}
/* }}} */

/* {{{ static void php_register_var(zval** entry TSRMLS_DC) */
static void php_register_var(zval** entry TSRMLS_DC)
{
	zval **value;

	if (Z_TYPE_PP(entry) == IS_ARRAY) {
		if (Z_ARRVAL_PP(entry)->nApplyCount > 1) {
			return;
		}

		zend_hash_internal_pointer_reset(Z_ARRVAL_PP(entry));
		Z_ARRVAL_PP(entry)->nApplyCount++;

		while (zend_hash_get_current_data(Z_ARRVAL_PP(entry), (void**)&value) == SUCCESS) {
			php_register_var(value TSRMLS_CC);
			zend_hash_move_forward(Z_ARRVAL_PP(entry));
		}

		Z_ARRVAL_PP(entry)->nApplyCount--;
	} else {
		convert_to_string_ex(entry);

		if ((strcmp(Z_STRVAL_PP(entry), "HTTP_SESSION_VARS") != 0) &&
			(strcmp(Z_STRVAL_PP(entry), "_SESSION") != 0)
		) {
			PS_ADD_VARL(Z_STRVAL_PP(entry), Z_STRLEN_PP(entry));
		}
	}
}
/* }}} */

/* {{{ proto string session_encode(void)
   Serializes the current setup and returns the serialized representation */
static PHP_FUNCTION(session_encode)
{
	int len;
	char *enc;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	enc = php_session_encode(&len TSRMLS_CC);
	if (enc == NULL) {
		RETURN_FALSE;
	}

	RETVAL_STRINGL(enc, len, 0);
}
/* }}} */

/* {{{ proto bool session_decode(string data)
   Deserializes data and reinitializes the variables */
static PHP_FUNCTION(session_decode)
{
	char *str;
	int str_len;

	if (PS(session_status) == php_session_none) {
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &str, &str_len) == FAILURE) {
		return;
	}

	php_session_decode(str, str_len TSRMLS_CC);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool session_start(void)
   Begin session - reinitializes freezed variables, registers browsers etc */
static PHP_FUNCTION(session_start)
{
	/* skipping check for non-zero args for performance reasons here ?*/
	php_session_start(TSRMLS_C);

	if (PS(session_status) != php_session_active) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool session_destroy(void)
   Destroy the current session and all data associated with it */
static PHP_FUNCTION(session_destroy)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(php_session_destroy(TSRMLS_C) == SUCCESS);
}
/* }}} */

/* {{{ proto void session_unset(void)
   Unset all registered variables */
static PHP_FUNCTION(session_unset)
{
	if (PS(session_status) == php_session_none) {
		RETURN_FALSE;
	}

	IF_SESSION_VARS() {
		HashTable *ht_sess_var;

		SEPARATE_ZVAL_IF_NOT_REF(&PS(http_session_vars));
		ht_sess_var = Z_ARRVAL_P(PS(http_session_vars));

		if (PG(register_globals)) {
			uint str_len;
			char *str;
			ulong num_key;
			HashPosition pos;

			zend_hash_internal_pointer_reset_ex(ht_sess_var, &pos);

			while (zend_hash_get_current_key_ex(ht_sess_var, &str, &str_len, &num_key, 0, &pos) == HASH_KEY_IS_STRING) {
				zend_delete_global_variable(str, str_len - 1 TSRMLS_CC);
				zend_hash_move_forward_ex(ht_sess_var, &pos);
			}
		}

		/* Clean $_SESSION. */
		zend_hash_clean(ht_sess_var);
	}
}
/* }}} */

/* {{{ proto void session_write_close(void)
   Write session data and end session */
static PHP_FUNCTION(session_write_close)
{
	php_session_flush(TSRMLS_C);
}
/* }}} */

/* {{{ proto bool session_register(mixed var_names [, mixed ...])
   Adds varname(s) to the list of variables which are freezed at the session end */
static PHP_FUNCTION(session_register)
{
	zval ***args = NULL;
	int num_args, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "+", &args, &num_args) == FAILURE) {
		return;
	}

	if (PS(session_status) == php_session_none || PS(session_status) == php_session_disabled) {
		php_session_start(TSRMLS_C);
	}

	if (PS(session_status) == php_session_disabled) {
		if (args) {
			efree(args);
		}
		RETURN_FALSE;
	}

	for (i = 0; i < num_args; i++) {
		if (Z_TYPE_PP(args[i]) == IS_ARRAY) {
			SEPARATE_ZVAL(args[i]);
		}
		php_register_var(args[i] TSRMLS_CC);
	}

	if (args) {
		efree(args);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool session_unregister(string varname)
   Removes varname from the list of variables which are freezed at the session end */
static PHP_FUNCTION(session_unregister)
{
	char *p_name;
	int p_name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &p_name, &p_name_len) == FAILURE) {
		return;
	}

	IF_SESSION_VARS() {
		SEPARATE_ZVAL_IF_NOT_REF(&PS(http_session_vars));
		PS_DEL_VARL(p_name, p_name_len);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool session_is_registered(string varname)
   Checks if a variable is registered in session */
static PHP_FUNCTION(session_is_registered)
{
	zval *p_var;
	char *p_name;
	int p_name_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &p_name, &p_name_len) == FAILURE) {
		return;
	}

	if (PS(session_status) == php_session_none) {
		RETURN_FALSE;
	}

	IF_SESSION_VARS() {
		if (zend_hash_find(Z_ARRVAL_P(PS(http_session_vars)), p_name, p_name_len+1, (void **)&p_var) == SUCCESS) {
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_session_name, 0, 0, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_module_name, 0, 0, 0)
	ZEND_ARG_INFO(0, module)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_save_path, 0, 0, 0)
	ZEND_ARG_INFO(0, path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_id, 0, 0, 0)
	ZEND_ARG_INFO(0, id)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_regenerate_id, 0, 0, 0)
	ZEND_ARG_INFO(0, delete_old_session)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_register, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_unregister, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_is_registered, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_session_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_set_save_handler, 0, 0, 6)
	ZEND_ARG_INFO(0, open)
	ZEND_ARG_INFO(0, close)
	ZEND_ARG_INFO(0, read)
	ZEND_ARG_INFO(0, write)
	ZEND_ARG_INFO(0, destroy)
	ZEND_ARG_INFO(0, gc)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_cache_limiter, 0, 0, 0)
	ZEND_ARG_INFO(0, cache_limiter)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_cache_expire, 0, 0, 0)
	ZEND_ARG_INFO(0, new_cache_expire)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_session_set_cookie_params, 0, 0, 1)
	ZEND_ARG_INFO(0, lifetime)
	ZEND_ARG_INFO(0, path)
	ZEND_ARG_INFO(0, domain)
	ZEND_ARG_INFO(0, secure)
	ZEND_ARG_INFO(0, httponly)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ session_functions[]
 */
static const zend_function_entry session_functions[] = {
	PHP_FE(session_name,              arginfo_session_name)
	PHP_FE(session_module_name,       arginfo_session_module_name)
	PHP_FE(session_save_path,         arginfo_session_save_path)
	PHP_FE(session_id,                arginfo_session_id)
	PHP_FE(session_regenerate_id,     arginfo_session_regenerate_id)
	PHP_FE(session_decode,            arginfo_session_decode)
	PHP_DEP_FE(session_register,      arginfo_session_register)
	PHP_DEP_FE(session_unregister,    arginfo_session_unregister)
	PHP_DEP_FE(session_is_registered, arginfo_session_is_registered)
	PHP_FE(session_encode,            arginfo_session_void)
	PHP_FE(session_start,             arginfo_session_void)
	PHP_FE(session_destroy,           arginfo_session_void)
	PHP_FE(session_unset,             arginfo_session_void)
	PHP_FE(session_set_save_handler,  arginfo_session_set_save_handler)
	PHP_FE(session_cache_limiter,     arginfo_session_cache_limiter)
	PHP_FE(session_cache_expire,      arginfo_session_cache_expire)
	PHP_FE(session_set_cookie_params, arginfo_session_set_cookie_params)
	PHP_FE(session_get_cookie_params, arginfo_session_void)
	PHP_FE(session_write_close,       arginfo_session_void)
	PHP_FALIAS(session_commit, session_write_close, arginfo_session_void)
	PHP_FE_END
};
/* }}} */

/* ********************************
   * Module Setup and Destruction *
   ******************************** */

static PHP_RINIT_FUNCTION(session) /* {{{ */
{
	php_rinit_session_globals(TSRMLS_C);

	if (PS(mod) == NULL) {
		char *value;

		value = zend_ini_string("session.save_handler", sizeof("session.save_handler"), 0);
		if (value) {
			PS(mod) = _php_find_ps_module(value TSRMLS_CC);
		}
	}

	if (PS(serializer) == NULL) {
		char *value;

		value = zend_ini_string("session.serialize_handler", sizeof("session.serialize_handler"), 0);
		if (value) {
			PS(serializer) = _php_find_ps_serializer(value TSRMLS_CC);
		}
	}

	if (PS(mod) == NULL || PS(serializer) == NULL) {
		/* current status is unusable */
		PS(session_status) = php_session_disabled;
		return SUCCESS;
	}

	if (PS(auto_start)) {
		php_session_start(TSRMLS_C);
	}

	return SUCCESS;
}
/* }}} */

static PHP_RSHUTDOWN_FUNCTION(session) /* {{{ */
{
	int i;

	php_session_flush(TSRMLS_C);
	php_rshutdown_session_globals(TSRMLS_C);

	/* this should NOT be done in php_rshutdown_session_globals() */
	for (i = 0; i < 6; i++) {
		if (PS(mod_user_names).names[i] != NULL) {
			zval_ptr_dtor(&PS(mod_user_names).names[i]);
			PS(mod_user_names).names[i] = NULL;
		}
	}

	return SUCCESS;
}
/* }}} */

static PHP_GINIT_FUNCTION(ps) /* {{{ */
{
	int i;

	ps_globals->save_path = NULL;
	ps_globals->session_name = NULL;
	ps_globals->id = NULL;
	ps_globals->mod = NULL;
	ps_globals->serializer = NULL;
	ps_globals->mod_data = NULL;
	ps_globals->session_status = php_session_none;
	for (i = 0; i < 6; i++) {
		ps_globals->mod_user_names.names[i] = NULL;
	}
	ps_globals->http_session_vars = NULL;
}
/* }}} */

static PHP_MINIT_FUNCTION(session) /* {{{ */
{
	zend_register_auto_global("_SESSION", sizeof("_SESSION")-1, NULL TSRMLS_CC);

	PS(module_number) = module_number; /* if we really need this var we need to init it in zts mode as well! */

	PS(session_status) = php_session_none;
	REGISTER_INI_ENTRIES();

#ifdef HAVE_LIBMM
	PHP_MINIT(ps_mm) (INIT_FUNC_ARGS_PASSTHRU);
#endif
	return SUCCESS;
}
/* }}} */

static PHP_MSHUTDOWN_FUNCTION(session) /* {{{ */
{
	UNREGISTER_INI_ENTRIES();

#ifdef HAVE_LIBMM
	PHP_MSHUTDOWN(ps_mm) (SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif

	ps_serializers[PREDEFINED_SERIALIZERS].name = NULL;
	memset(&ps_modules[PREDEFINED_MODULES], 0, (MAX_MODULES-PREDEFINED_MODULES)*sizeof(ps_module *));

	return SUCCESS;
}
/* }}} */

static PHP_MINFO_FUNCTION(session) /* {{{ */
{
	ps_module **mod;
	ps_serializer *ser;
	smart_str save_handlers = {0};
	smart_str ser_handlers = {0};
	int i;

	/* Get save handlers */
	for (i = 0, mod = ps_modules; i < MAX_MODULES; i++, mod++) {
		if (*mod && (*mod)->s_name) {
			smart_str_appends(&save_handlers, (*mod)->s_name);
			smart_str_appendc(&save_handlers, ' ');
		}
	}

	/* Get serializer handlers */
	for (i = 0, ser = ps_serializers; i < MAX_SERIALIZERS; i++, ser++) {
		if (ser && ser->name) {
			smart_str_appends(&ser_handlers, ser->name);
			smart_str_appendc(&ser_handlers, ' ');
		}
	}

	php_info_print_table_start();
	php_info_print_table_row(2, "Session Support", "enabled" );

	if (save_handlers.c) {
		smart_str_0(&save_handlers);
		php_info_print_table_row(2, "Registered save handlers", save_handlers.c);
		smart_str_free(&save_handlers);
	} else {
		php_info_print_table_row(2, "Registered save handlers", "none");
	}

	if (ser_handlers.c) {
		smart_str_0(&ser_handlers);
		php_info_print_table_row(2, "Registered serializer handlers", ser_handlers.c);
		smart_str_free(&ser_handlers);
	} else {
		php_info_print_table_row(2, "Registered serializer handlers", "none");
	}

	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static const zend_module_dep session_deps[] = { /* {{{ */
	ZEND_MOD_OPTIONAL("hash")
	ZEND_MOD_REQUIRED("spl")
	ZEND_MOD_END
};
/* }}} */

zend_module_entry session_module_entry = {
	STANDARD_MODULE_HEADER_EX,
	NULL,
	session_deps,
	"session",
	session_functions,
	PHP_MINIT(session), PHP_MSHUTDOWN(session),
	PHP_RINIT(session), PHP_RSHUTDOWN(session),
	PHP_MINFO(session),
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(ps),
	PHP_GINIT(ps),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

#ifdef COMPILE_DL_SESSION
ZEND_GET_MODULE(session)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
