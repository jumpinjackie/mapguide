dnl
dnl $Id: config.m4,v 1.58.2.4.2.11 2007/09/18 21:35:39 hirokawa Exp $
dnl

AC_DEFUN([PHP_MBSTRING_ADD_SOURCES], [
  PHP_MBSTRING_SOURCES="$PHP_MBSTRING_SOURCES $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_BASE_SOURCES], [
  PHP_MBSTRING_BASE_SOURCES="$PHP_MBSTRING_BASE_SOURCES $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_BUILD_DIR], [
  PHP_MBSTRING_EXTRA_BUILD_DIRS="$PHP_MBSTRING_EXTRA_BUILD_DIRS $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_INCLUDE], [
  PHP_MBSTRING_EXTRA_INCLUDES="$PHP_MBSTRING_EXTRA_INCLUDES $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_CONFIG_HEADER], [
  PHP_MBSTRING_EXTRA_CONFIG_HEADERS="$PHP_MBSTRING_EXTRA_CONFIG_HEADERS $1"
])

AC_DEFUN([PHP_MBSTRING_ADD_CFLAG], [
  PHP_MBSTRING_CFLAGS="$PHP_MBSTRING_CFLAGS $1"
])

AC_DEFUN([PHP_MBSTRING_EXTENSION], [
  PHP_NEW_EXTENSION(mbstring, $PHP_MBSTRING_SOURCES, $ext_shared,, $PHP_MBSTRING_CFLAGS)
  PHP_SUBST(MBSTRING_SHARED_LIBADD)

  for dir in $PHP_MBSTRING_EXTRA_BUILD_DIRS; do
    PHP_ADD_BUILD_DIR([$ext_builddir/$dir], 1)
  done
  
  for dir in $PHP_MBSTRING_EXTRA_INCLUDES; do
    PHP_ADD_INCLUDE([$ext_srcdir/$dir])
    PHP_ADD_INCLUDE([$ext_builddir/$dir])
  done

  if test "$ext_shared" = "no"; then
    PHP_ADD_SOURCES(PHP_EXT_DIR(mbstring), $PHP_MBSTRING_BASE_SOURCES)
    out="php_config.h"
  else
    PHP_ADD_SOURCES_X(PHP_EXT_DIR(mbstring),$PHP_MBSTRING_BASE_SOURCES,,shared_objects_mbstring,yes)
    if test -f "$ext_builddir/config.h.in"; then
      out="$abs_builddir/config.h"
    else
      out="php_config.h"
    fi
  fi
  
  for cfg in $PHP_MBSTRING_EXTRA_CONFIG_HEADERS; do
    cat > $ext_builddir/$cfg <<EOF
#include "$out"
EOF
  done
])

AC_DEFUN([PHP_MBSTRING_SETUP_MBREGEX], [
  if test "$PHP_MBREGEX" = "yes"; then
    AC_CACHE_CHECK(for variable length prototypes and stdarg.h, cv_php_mbstring_stdarg, [
      AC_TRY_RUN([
#include <stdarg.h>
int foo(int x, ...) {
	va_list va;
	va_start(va, x);
	va_arg(va, int);
	va_arg(va, char *);
	va_arg(va, double);
	return 0;
}
int main() { return foo(10, "", 3.14); }
      ], [cv_php_mbstring_stdarg=yes], [cv_php_mbstring_stdarg=no], 
 [
  dnl cross-compile needs something here
case $host_alias in
*netware*)
cv_php_mbstring_stdarg=yes
;;
*)
cv_php_mbstring_stdarg=no
;;
esac
]
)
    ])

    AC_CHECK_HEADERS([stdlib.h string.h strings.h unistd.h sys/time.h sys/times.h stdarg.h])
    AC_CHECK_SIZEOF(int, 4)
    AC_CHECK_SIZEOF(short, 2)
    AC_CHECK_SIZEOF(long, 4)
    AC_C_CONST
    AC_HEADER_TIME 
    AC_FUNC_ALLOCA
    AC_FUNC_MEMCMP

    AC_DEFINE([HAVE_MBREGEX], 1, [whether to have multibyte regex support])


    if test "$PHP_MBREGEX_BACKTRACK" != "no"; then
      AC_DEFINE([USE_COMBINATION_EXPLOSION_CHECK],1,[whether to check multibyte regex backtrack])
    fi

    PHP_MBSTRING_ADD_CFLAG([-DNOT_RUBY])
    PHP_MBSTRING_ADD_BUILD_DIR([oniguruma])
    PHP_MBSTRING_ADD_BUILD_DIR([oniguruma/enc])
    PHP_MBSTRING_ADD_INCLUDE([oniguruma])
    PHP_MBSTRING_ADD_CONFIG_HEADER([oniguruma/config.h])

    PHP_MBSTRING_ADD_BASE_SOURCES([php_mbregex.c])
    PHP_MBSTRING_ADD_SOURCES([
      oniguruma/regcomp.c
      oniguruma/regerror.c
      oniguruma/regexec.c
      oniguruma/reggnu.c
      oniguruma/regparse.c
      oniguruma/regenc.c
      oniguruma/regext.c
      oniguruma/regsyntax.c
      oniguruma/regtrav.c
      oniguruma/regversion.c
      oniguruma/st.c
      oniguruma/enc/unicode.c
      oniguruma/enc/ascii.c
      oniguruma/enc/utf8.c
      oniguruma/enc/euc_jp.c
      oniguruma/enc/euc_tw.c
      oniguruma/enc/euc_kr.c
      oniguruma/enc/sjis.c
      oniguruma/enc/iso8859_1.c
      oniguruma/enc/iso8859_2.c
      oniguruma/enc/iso8859_3.c
      oniguruma/enc/iso8859_4.c
      oniguruma/enc/iso8859_5.c
      oniguruma/enc/iso8859_6.c
      oniguruma/enc/iso8859_7.c
      oniguruma/enc/iso8859_8.c
      oniguruma/enc/iso8859_9.c
      oniguruma/enc/iso8859_10.c
      oniguruma/enc/iso8859_11.c
      oniguruma/enc/iso8859_13.c
      oniguruma/enc/iso8859_14.c
      oniguruma/enc/iso8859_15.c
      oniguruma/enc/iso8859_16.c
      oniguruma/enc/koi8.c
      oniguruma/enc/koi8_r.c
      oniguruma/enc/big5.c
      oniguruma/enc/utf16_be.c
      oniguruma/enc/utf16_le.c
      oniguruma/enc/utf32_be.c
      oniguruma/enc/utf32_le.c
      oniguruma/enc/gb18030.c
    ])
  fi
])

AC_DEFUN([PHP_MBSTRING_SETUP_LIBMBFL], [
  dnl libmbfl is required and can not be disabled
  if test "$PHP_LIBMBFL" = "yes" || test "$PHP_LIBMBFL" = "no"; then
    dnl
    dnl Bundled libmbfl
    dnl
    PHP_MBSTRING_ADD_BUILD_DIR([libmbfl])
    PHP_MBSTRING_ADD_BUILD_DIR([libmbfl/mbfl])
    PHP_MBSTRING_ADD_BUILD_DIR([libmbfl/filters])
    PHP_MBSTRING_ADD_BUILD_DIR([libmbfl/nls])
    PHP_MBSTRING_ADD_INCLUDE([libmbfl])
    PHP_MBSTRING_ADD_INCLUDE([libmbfl/mbfl])
    PHP_MBSTRING_ADD_CONFIG_HEADER([libmbfl/config.h])

    PHP_MBSTRING_ADD_SOURCES([
     libmbfl/filters/html_entities.c
     libmbfl/filters/mbfilter_7bit.c
     libmbfl/filters/mbfilter_ascii.c
     libmbfl/filters/mbfilter_base64.c
     libmbfl/filters/mbfilter_big5.c
     libmbfl/filters/mbfilter_byte2.c
     libmbfl/filters/mbfilter_byte4.c
     libmbfl/filters/mbfilter_cp1251.c
     libmbfl/filters/mbfilter_cp1252.c
     libmbfl/filters/mbfilter_cp866.c
     libmbfl/filters/mbfilter_cp932.c
     libmbfl/filters/mbfilter_cp936.c
     libmbfl/filters/mbfilter_euc_cn.c
     libmbfl/filters/mbfilter_euc_jp.c
     libmbfl/filters/mbfilter_euc_jp_win.c
     libmbfl/filters/mbfilter_cp51932.c
     libmbfl/filters/mbfilter_euc_kr.c
     libmbfl/filters/mbfilter_euc_tw.c
     libmbfl/filters/mbfilter_htmlent.c
     libmbfl/filters/mbfilter_hz.c
     libmbfl/filters/mbfilter_iso2022_kr.c
     libmbfl/filters/mbfilter_iso8859_1.c
     libmbfl/filters/mbfilter_iso8859_10.c
     libmbfl/filters/mbfilter_iso8859_13.c
     libmbfl/filters/mbfilter_iso8859_14.c
     libmbfl/filters/mbfilter_iso8859_15.c
     libmbfl/filters/mbfilter_iso8859_16.c
     libmbfl/filters/mbfilter_iso8859_2.c
     libmbfl/filters/mbfilter_iso8859_3.c
     libmbfl/filters/mbfilter_iso8859_4.c
     libmbfl/filters/mbfilter_iso8859_5.c
     libmbfl/filters/mbfilter_iso8859_6.c
     libmbfl/filters/mbfilter_iso8859_7.c
     libmbfl/filters/mbfilter_iso8859_8.c
     libmbfl/filters/mbfilter_iso8859_9.c
     libmbfl/filters/mbfilter_jis.c
     libmbfl/filters/mbfilter_iso2022_jp_ms.c
     libmbfl/filters/mbfilter_koi8r.c
     libmbfl/filters/mbfilter_armscii8.c
     libmbfl/filters/mbfilter_qprint.c
     libmbfl/filters/mbfilter_sjis.c
     libmbfl/filters/mbfilter_ucs2.c
     libmbfl/filters/mbfilter_ucs4.c
     libmbfl/filters/mbfilter_uhc.c
     libmbfl/filters/mbfilter_utf16.c
     libmbfl/filters/mbfilter_utf32.c
     libmbfl/filters/mbfilter_utf7.c
     libmbfl/filters/mbfilter_utf7imap.c
     libmbfl/filters/mbfilter_utf8.c
     libmbfl/filters/mbfilter_uuencode.c
     libmbfl/mbfl/mbfilter.c
     libmbfl/mbfl/mbfilter_8bit.c
     libmbfl/mbfl/mbfilter_pass.c
     libmbfl/mbfl/mbfilter_wchar.c
     libmbfl/mbfl/mbfl_convert.c
     libmbfl/mbfl/mbfl_encoding.c
     libmbfl/mbfl/mbfl_filter_output.c
     libmbfl/mbfl/mbfl_ident.c
     libmbfl/mbfl/mbfl_language.c
     libmbfl/mbfl/mbfl_memory_device.c
     libmbfl/mbfl/mbfl_string.c
     libmbfl/mbfl/mbfl_allocators.c
     libmbfl/nls/nls_de.c
     libmbfl/nls/nls_en.c
     libmbfl/nls/nls_ja.c
     libmbfl/nls/nls_kr.c
     libmbfl/nls/nls_neutral.c
     libmbfl/nls/nls_ru.c
     libmbfl/nls/nls_uni.c
     libmbfl/nls/nls_zh.c
     libmbfl/nls/nls_hy.c
     libmbfl/nls/nls_tr.c
    ])
    PHP_MBSTRING_ADD_CFLAG([-DHAVE_CONFIG_H])
  else
    dnl
    dnl External libmfl
    dnl  
    for inc in include include/mbfl-1.0 include/mbfl; do
      if test -f "$PHP_LIBMBFL/$inc/mbfilter.h"; then
        PHP_LIBMBFL_INCLUDE="$inc"
        break
      fi
    done

    if test -z "$PHP_LIBMBFL_INCLUDE"; then
      AC_MSG_ERROR([mbfilter.h not found. Please reinstall libmbfl library.])
    else 
      PHP_ADD_INCLUDE([$PHP_LIBMBFL_INCLUDE])
    fi

    PHP_CHECK_LIBRARY(mbfl, mbfl_buffer_converter_new, [
      PHP_ADD_LIBRARY_WITH_PATH(mbfl, $PHP_LIBMBFL/$PHP_LIBDIR, MBSTRING_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libmbfl. Please check config.log for more information.])
    ], [
      -L$PHP_LIBMBFL/$PHP_LIBDIR
    ])
  fi
])

dnl
dnl Main config
dnl

PHP_ARG_ENABLE(mbstring, whether to enable multibyte string support,
[  --enable-mbstring       Enable multibyte string support])

PHP_ARG_ENABLE([mbregex], [whether to enable multibyte regex support],
[  --disable-mbregex         MBSTRING: Disable multibyte regex support], yes, no)

PHP_ARG_ENABLE([mbregex_backtrack], [whether to check multibyte regex backtrack],
[  --disable-mbregex-backtrack
                            MBSTRING: Disable multibyte regex backtrack check], yes, no)

PHP_ARG_WITH(libmbfl, [for external libmbfl],
[  --with-libmbfl[=DIR]      MBSTRING: Use external libmbfl.  DIR is the libmbfl base
                            install directory [BUNDLED]], no, no)

if test "$PHP_MBSTRING" != "no"; then  
  AC_DEFINE([HAVE_MBSTRING],1,[whether to have multibyte string support])

  PHP_MBSTRING_ADD_BASE_SOURCES([mbstring.c php_unicode.c mb_gpc.c])

  if test "$PHP_MBREGEX" != "no"; then
    PHP_MBSTRING_SETUP_MBREGEX
  fi
  
  dnl libmbfl is required
  PHP_MBSTRING_SETUP_LIBMBFL
  PHP_MBSTRING_EXTENSION
  PHP_INSTALL_HEADERS([ext/mbstring], [mbstring.h php_mbregex.h libmbfl/config.h libmbfl/mbfl/eaw_table.h libmbfl/mbfl/mbfilter.h libmbfl/mbfl/mbfilter_8bit.h libmbfl/mbfl/mbfilter_pass.h libmbfl/mbfl/mbfilter_wchar.h libmbfl/mbfl/mbfl_allocators.h libmbfl/mbfl/mbfl_consts.h libmbfl/mbfl/mbfl_convert.h libmbfl/mbfl/mbfl_defs.h libmbfl/mbfl/mbfl_encoding.h libmbfl/mbfl/mbfl_filter_output.h libmbfl/mbfl/mbfl_ident.h libmbfl/mbfl/mbfl_language.h libmbfl/mbfl/mbfl_memory_device.h libmbfl/mbfl/mbfl_string.h oniguruma/oniguruma.h oniguruma/php_onig_compat.h])
fi

# vim600: sts=2 sw=2 et
