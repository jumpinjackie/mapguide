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
   | Authors: Sara Golemon (pollita@php.net)                              |
   +----------------------------------------------------------------------+
*/

/* $Id: bz2_filter.c,v 1.3.2.2.2.5 2007/08/09 23:27:22 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_bz2.h"

/* {{{ data structure */

typedef struct _php_bz2_filter_data {
	int persistent;
	bz_stream strm;
	char *inbuf;
	size_t inbuf_len;
	char *outbuf;
	size_t outbuf_len;
} php_bz2_filter_data;

/* }}} */

/* {{{ Memory management wrappers */

static void *php_bz2_alloc(void *opaque, int items, int size)
{
	return (void *)safe_pemalloc(items, size, 0, ((php_bz2_filter_data*)opaque)->persistent);
}

static void php_bz2_free(void *opaque, void *address)
{
	pefree((void *)address, ((php_bz2_filter_data*)opaque)->persistent);
}
/* }}} */

/* {{{ bzip2.decompress filter implementation */

static php_stream_filter_status_t php_bz2_decompress_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_bz2_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;
	bz_stream *streamp;

	if (!thisfilter || !thisfilter->abstract) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_bz2_filter_data *)(thisfilter->abstract);
	streamp = &(data->strm);

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);
		while (bin < bucket->buflen) {
			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf + bin, desired);
			data->strm.avail_in = desired;

			status = BZ2_bzDecompress(&(data->strm));
			if (status != BZ_OK && status != BZ_STREAM_END) {
				/* Something bad happened */
				php_stream_bucket_delref(bucket TSRMLS_CC);
				return PSFS_ERR_FATAL;
			}
			desired -= data->strm.avail_in; /* desired becomes what we consumed this round through */
			data->strm.next_in = data->inbuf;
			data->strm.avail_in = 0;
			consumed += desired;
			bin += desired;

			if (!desired) {
				flags |= PSFS_FLAG_FLUSH_CLOSE;
				break;
			}

			if (data->strm.avail_out < data->outbuf_len) {
				php_stream_bucket *out_bucket;
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;
				out_bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0 TSRMLS_CC);
				php_stream_bucket_append(buckets_out, out_bucket TSRMLS_CC);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}
		}
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags & PSFS_FLAG_FLUSH_CLOSE) {
		/* Spit it out! */
		status = BZ_OK;
		while (status == BZ_OK) {
			status = BZ2_bzDecompress(&(data->strm));
			if (data->strm.avail_out < data->outbuf_len) {
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0 TSRMLS_CC);
				php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}
		}
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}

	return exit_status;
}

static void php_bz2_decompress_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_bz2_filter_data *data = thisfilter->abstract;
		BZ2_bzDecompressEnd(&(data->strm));
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static php_stream_filter_ops php_bz2_decompress_ops = {
	php_bz2_decompress_filter,
	php_bz2_decompress_dtor,
	"bzip2.decompress"
};
/* }}} */

/* {{{ bzip2.compress filter implementation */

static php_stream_filter_status_t php_bz2_compress_filter(
	php_stream *stream,
	php_stream_filter *thisfilter,
	php_stream_bucket_brigade *buckets_in,
	php_stream_bucket_brigade *buckets_out,
	size_t *bytes_consumed,
	int flags
	TSRMLS_DC)
{
	php_bz2_filter_data *data;
	php_stream_bucket *bucket;
	size_t consumed = 0;
	int status;
	php_stream_filter_status_t exit_status = PSFS_FEED_ME;
	bz_stream *streamp;

	if (!thisfilter || !thisfilter->abstract) {
		/* Should never happen */
		return PSFS_ERR_FATAL;
	}

	data = (php_bz2_filter_data *)(thisfilter->abstract);
	streamp = &(data->strm);

	while (buckets_in->head) {
		size_t bin = 0, desired;

		bucket = php_stream_bucket_make_writeable(buckets_in->head TSRMLS_CC);

		while (bin < bucket->buflen) {
			desired = bucket->buflen - bin;
			if (desired > data->inbuf_len) {
				desired = data->inbuf_len;
			}
			memcpy(data->strm.next_in, bucket->buf + bin, desired);
			data->strm.avail_in = desired;

			status = BZ2_bzCompress(&(data->strm), flags & PSFS_FLAG_FLUSH_CLOSE ? BZ_FINISH : (flags & PSFS_FLAG_FLUSH_INC ? BZ_FLUSH : BZ_RUN));
			if (status != BZ_RUN_OK && status != BZ_FLUSH_OK && status != BZ_FINISH_OK) {
				/* Something bad happened */
				php_stream_bucket_delref(bucket TSRMLS_CC);
				return PSFS_ERR_FATAL;
			}
			desired -= data->strm.avail_in; /* desired becomes what we consumed this round through */
			data->strm.next_in = data->inbuf;
			data->strm.avail_in = 0;
			consumed += desired;
			bin += desired;

			if (data->strm.avail_out < data->outbuf_len) {
				php_stream_bucket *out_bucket;
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				out_bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0 TSRMLS_CC);
				php_stream_bucket_append(buckets_out, out_bucket TSRMLS_CC);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}
		}
		php_stream_bucket_delref(bucket TSRMLS_CC);
	}

	if (flags & PSFS_FLAG_FLUSH_CLOSE) {
		/* Spit it out! */
		status = BZ_FINISH_OK;
		while (status == BZ_FINISH_OK) {
			status = BZ2_bzCompress(&(data->strm), BZ_FINISH);
			if (data->strm.avail_out < data->outbuf_len) {
				size_t bucketlen = data->outbuf_len - data->strm.avail_out;

				bucket = php_stream_bucket_new(stream, estrndup(data->outbuf, bucketlen), bucketlen, 1, 0 TSRMLS_CC);
				php_stream_bucket_append(buckets_out, bucket TSRMLS_CC);
				data->strm.avail_out = data->outbuf_len;
				data->strm.next_out = data->outbuf;
				exit_status = PSFS_PASS_ON;
			}
		}
	}

	if (bytes_consumed) {
		*bytes_consumed = consumed;
	}
	return exit_status;
}

static void php_bz2_compress_dtor(php_stream_filter *thisfilter TSRMLS_DC)
{
	if (thisfilter && thisfilter->abstract) {
		php_bz2_filter_data *data = thisfilter->abstract;
		BZ2_bzCompressEnd(&(data->strm));
		pefree(data->inbuf, data->persistent);
		pefree(data->outbuf, data->persistent);
		pefree(data, data->persistent);
	}
}

static php_stream_filter_ops php_bz2_compress_ops = {
	php_bz2_compress_filter,
	php_bz2_compress_dtor,
	"bzip2.compress"
};

/* }}} */

/* {{{ bzip2.* common factory */

static php_stream_filter *php_bz2_filter_create(const char *filtername, zval *filterparams, int persistent TSRMLS_DC)
{
	php_stream_filter_ops *fops = NULL;
	php_bz2_filter_data *data;
	int status;

	/* Create this filter */
	data = pecalloc(1, sizeof(php_bz2_filter_data), persistent);
	if (!data) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %zu bytes.", sizeof(php_bz2_filter_data));
		return NULL;
	}

	/* Circular reference */
	data->strm.opaque = (void *) data;

	data->strm.bzalloc = php_bz2_alloc;
	data->strm.bzfree = php_bz2_free;
	data->persistent = persistent;
	data->strm.avail_out = data->outbuf_len = data->inbuf_len = 2048;
	data->strm.next_in = data->inbuf = (char *) pemalloc(data->inbuf_len, persistent);
	if (!data->inbuf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %zu bytes.", data->inbuf_len);
		pefree(data, persistent);
		return NULL;
	}
	data->strm.avail_in = 0;
	data->strm.next_out = data->outbuf = (char *) pemalloc(data->outbuf_len, persistent);
	if (!data->outbuf) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Failed allocating %zu bytes.", data->outbuf_len);
		pefree(data->inbuf, persistent);
		pefree(data, persistent);
		return NULL;
	}

	if (strcasecmp(filtername, "bzip2.decompress") == 0) {
		int smallFootprint = 0;

		if (filterparams) {
			zval **tmpzval = NULL;

			if (Z_TYPE_P(filterparams) == IS_ARRAY || Z_TYPE_P(filterparams) == IS_OBJECT) {
				zend_hash_find(HASH_OF(filterparams), "small", sizeof("small"), (void **) &tmpzval);
			} else {
				tmpzval = &filterparams;
			}

			if (tmpzval) {
				SEPARATE_ZVAL(tmpzval);
				convert_to_boolean_ex(tmpzval);
				smallFootprint = Z_LVAL_PP(tmpzval);
				zval_ptr_dtor(tmpzval);
			}
		}

		status = BZ2_bzDecompressInit(&(data->strm), 0, smallFootprint);
		fops = &php_bz2_decompress_ops;
	} else if (strcasecmp(filtername, "bzip2.compress") == 0) {
		int blockSize100k = PHP_BZ2_FILTER_DEFAULT_BLOCKSIZE;
		int workFactor = PHP_BZ2_FILTER_DEFAULT_WORKFACTOR;

		if (filterparams) {
			zval **tmpzval;

			if (Z_TYPE_P(filterparams) == IS_ARRAY || Z_TYPE_P(filterparams) == IS_OBJECT) {
				if (zend_hash_find(HASH_OF(filterparams), "blocks", sizeof("blocks"), (void**) &tmpzval) == SUCCESS) {
					/* How much memory to allocate (1 - 9) x 100kb */
					SEPARATE_ZVAL(tmpzval);
					convert_to_long_ex(tmpzval);
					if (Z_LVAL_PP(tmpzval) < 1 || Z_LVAL_PP(tmpzval) > 9) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter given for number of blocks to allocate. (%ld)", Z_LVAL_PP(tmpzval));
					} else {
						blockSize100k = Z_LVAL_PP(tmpzval);
					}
					zval_ptr_dtor(tmpzval);
				}

				if (zend_hash_find(HASH_OF(filterparams), "work", sizeof("work"), (void**) &tmpzval) == SUCCESS) {
					/* Work Factor (0 - 250) */
					SEPARATE_ZVAL(tmpzval);
					convert_to_long_ex(tmpzval);
					if (Z_LVAL_PP(tmpzval) < 0 || Z_LVAL_PP(tmpzval) > 250) {
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid parameter given for work factor. (%ld)", Z_LVAL_PP(tmpzval));
					} else {
						workFactor = Z_LVAL_PP(tmpzval);
					}
					zval_ptr_dtor(tmpzval);
				}
			}
		}

		status = BZ2_bzCompressInit(&(data->strm), blockSize100k, 0, workFactor);
		fops = &php_bz2_compress_ops;
	} else {
		status = BZ_DATA_ERROR;
	}

	if (status != BZ_OK) {
		/* Unspecified (probably strm) error, let stream-filter error do its own whining */
		pefree(data->strm.next_in, persistent);
		pefree(data->strm.next_out, persistent);
		pefree(data, persistent);
		return NULL;
	}

	return php_stream_filter_alloc(fops, data, persistent);
}

php_stream_filter_factory php_bz2_filter_factory = {
	php_bz2_filter_create
};
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
