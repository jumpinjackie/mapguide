dnl
dnl $Id: config.m4,v 1.3.2.4 2007/07/03 17:25:33 sniper Exp $
dnl

PHP_ARG_ENABLE(json, whether to enable JavaScript Object Serialization support,
[  --disable-json          Disable JavaScript Object Serialization support], yes)

if test "$PHP_JSON" != "no"; then
  AC_DEFINE([HAVE_JSON],1 ,[whether to enable JavaScript Object Serialization support])
  AC_HEADER_STDC

  PHP_NEW_EXTENSION(json, json.c utf8_to_utf16.c utf8_decode.c JSON_parser.c, $ext_shared)
  PHP_SUBST(JSON_SHARED_LIBADD)
fi
