dnl
dnl $Id: config.m4,v 1.12.4.5 2007/03/01 11:23:54 tony2001 Exp $
dnl

PHP_ARG_ENABLE(posix,whether to enable POSIX-like functions,
[  --disable-posix         Disable POSIX-like functions], yes)

if test "$PHP_POSIX" = "yes"; then
  AC_DEFINE(HAVE_POSIX, 1, [whether to include POSIX-like functions])
  PHP_NEW_EXTENSION(posix, posix.c, $ext_shared)

  AC_CHECK_HEADERS(sys/mkdev.h)

  AC_CHECK_FUNCS(seteuid setegid setsid getsid setpgid getpgid ctermid mkfifo mknod getrlimit getlogin getgroups makedev initgroups getpwuid_r getgrgid_r)

  AC_MSG_CHECKING([for working ttyname_r() implementation])
  AC_TRY_RUN([
#include <unistd.h>

int main(int argc, char *argv[])
{
	char buf[64];

	return ttyname_r(0, buf, 64) ? 1 : 0;
}
  ],[
    AC_MSG_RESULT([yes])
    AC_DEFINE(HAVE_TTYNAME_R, 1, [Whether you have a working ttyname_r])
  ],[
    AC_MSG_RESULT([no, posix_ttyname() will be thread-unsafe])
  ], [
    AC_MSG_RESULT([no, cannot detect working ttyname_r() when cross compiling. posix_ttyname() will be thread-unsafe])
  ])
fi
