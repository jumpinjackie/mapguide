/* main/php_config.h.  Generated automatically by configure.  */
/* main/php_config.h.in.  Generated automatically from configure.in by autoheader.  */
/* Leave this file alone */
/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2004 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/

/* $Id: acconfig.h,v 1.34.2.3 2005/07/29 09:20:02 hyanantha Exp $ */

#define ZEND_API
#define ZEND_DLEXPORT
#define ZEND_DLIMPORT


/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if your system has a working fnmatch function.  */
#define HAVE_FNMATCH 1

/* Define if your struct stat has st_blksize.  */
#define HAVE_ST_BLKSIZE 1

/* Define if your struct stat has st_blocks.  */
#define HAVE_ST_BLOCKS 1

/* Define if your struct stat has st_rdev.  */
#define HAVE_ST_RDEV 1

/* Define if your struct tm has tm_zone.  */
#define HAVE_TM_ZONE 1

/* Define if you don't have tm_zone but do have the external array
   tzname.  */
/* #undef HAVE_TZNAME */

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
#define HAVE_UTIME_NULL 1

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define if your C compiler doesn't accept -c and -o together.  */
/* #undef NO_MINUS_C_MINUS_O */

/* Define as the return type of signal handlers (int or void).  */
#define RETSIGTYPE void

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
/* #undef TIME_WITH_SYS_TIME */

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define if lex declares yytext as a char * by default, not a char[].  */
#define YYTEXT_POINTER 1

/* #undef uint */
/* #undef ulong */

/* The number of bytes in a char.  */
/* #undef SIZEOF_CHAR */

/* The number of bytes in a char *.  */
#define SIZEOF_CHAR_P 4

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a intmax_t.  */
#define SIZEOF_INTMAX_T 0

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a long long.  */
#define SIZEOF_LONG_LONG 8

/* The number of bytes in a long long int.  */
#define SIZEOF_LONG_LONG_INT 8

/* The number of bytes in a ptrdiff_t.  */
#define SIZEOF_PTRDIFF_T 0

/* The number of bytes in a short.  */
/* #undef SIZEOF_SHORT */

/* The number of bytes in a size_t.  */
#define SIZEOF_SIZE_T 4

/* The number of bytes in a ssize_t.  */
#define SIZEOF_SSIZE_T 0

/* Define if you have the CreateProcess function.  */
/* #undef HAVE_CREATEPROCESS */

/* Define if you have the acosh function.  */
#define HAVE_ACOSH 1

/* Define if you have the alphasort function.  */
#define HAVE_ALPHASORT 1

/* Define if you have the asctime_r function.  */
#define HAVE_ASCTIME_R 1

/* Define if you have the asinh function.  */
#define HAVE_ASINH 1

/* Define if you have the atanh function.  */
#define HAVE_ATANH 1

/* Define if you have the chroot function.  */
#define HAVE_CHROOT 1

/* Define if you have the crypt function.  */
#define HAVE_CRYPT 1

/* Define if you have the ctermid function.  */
#define HAVE_CTERMID 1

/* Define if you have the ctime_r function.  */
#define HAVE_CTIME_R 1

/* Define if you have the cuserid function.  */
#define HAVE_CUSERID 1

/* Define if you have the fabsf function.  */
/* #undef HAVE_FABSF */

/* Define if you have the finite function.  */
#define HAVE_FINITE 1

/* Define if you have the flock function.  */
#define HAVE_FLOCK 1

/* Define if you have the floorf function.  */
/* #undef HAVE_FLOORF */

/* Define if you have the fork function.  */
#define HAVE_FORK 1

/* Define if you have the fpclass function.  */
/* #undef HAVE_FPCLASS */

/* Define if you have the ftok function.  */
#define HAVE_FTOK 1

/* Define if you have the funopen function.  */
/* #undef HAVE_FUNOPEN */

/* Define if you have the gai_strerror function.  */
#define HAVE_GAI_STRERROR 1

/* Define if you have the gcvt function.  */
#define HAVE_GCVT 1

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getgroups function.  */
#define HAVE_GETGROUPS 1

/* Define if you have the getlogin function.  */
#define HAVE_GETLOGIN 1

/* Define if you have the getopt function.  */
#define HAVE_GETOPT 1

/* Define if you have the getpgid function.  */
#define HAVE_GETPGID 1

/* Define if you have the getpid function.  */
#define HAVE_GETPID 1

/* Define if you have the getpriority function.  */
/* #undef HAVE_GETPRIORITY */

/* Define if you have the getprotobyname function.  */
#define HAVE_GETPROTOBYNAME 1

/* Define if you have the getprotobynumber function.  */
#define HAVE_GETPROTOBYNUMBER 1

/* Define if you have the getrlimit function.  */
#define HAVE_GETRLIMIT 1

/* Define if you have the getrusage function.  */
#define HAVE_GETRUSAGE 1

/* Define if you have the getservbyname function.  */
#define HAVE_GETSERVBYNAME 1

/* Define if you have the getservbyport function.  */
#define HAVE_GETSERVBYPORT 1

/* Define if you have the getsid function.  */
#define HAVE_GETSID 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the getwd function.  */
#define HAVE_GETWD 1

/* Define if you have the glob function.  */
#define HAVE_GLOB 1

/* Define if you have the gmtime_r function.  */
#define HAVE_GMTIME_R 1

/* Define if you have the grantpt function.  */
#define HAVE_GRANTPT 1

/* Define if you have the hstrerror function.  */
/* #undef HAVE_HSTRERROR */

/* Define if you have the hypot function.  */
#define HAVE_HYPOT 1

/* Define if you have the inet_ntoa function.  */
#define HAVE_INET_NTOA 1

/* Define if you have the inet_ntop function.  */
#define HAVE_INET_NTOP 1

/* Define if you have the inet_pton function.  */
#define HAVE_INET_PTON 1

/* Define if you have the isascii function.  */
#define HAVE_ISASCII 1

/* Define if you have the isfinite function.  */
/* #undef HAVE_ISFINITE */

/* Define if you have the isinf function.  */
#define HAVE_ISINF 1

/* Define if you have the isnan function.  */
#define HAVE_ISNAN 1

/* Define if you have the kill function.  */
#define HAVE_KILL 1

/* Define if you have the ldap_parse_reference function.  */
/* #undef HAVE_LDAP_PARSE_REFERENCE */

/* Define if you have the ldap_parse_result function.  */
/* #undef HAVE_LDAP_PARSE_RESULT */

/* Define if you have the ldap_start_tls_s function.  */
/* #undef HAVE_LDAP_START_TLS_S */

/* Define if you have the link function.  */
#define HAVE_LINK 1

/* Define if you have the localeconv function.  */
#define HAVE_LOCALECONV 1

/* Define if you have the localtime_r function.  */
#define HAVE_LOCALTIME_R 1

/* Define if you have the lockf function.  */
#define HAVE_LOCKF 1

/* Define if you have the log1p function.  */
#define HAVE_LOG1P 1

/* Define if you have the lrand48 function.  */
#define HAVE_LRAND48 1

/* Define if you have the mblen function.  */
#define HAVE_MBLEN 1

/* Define if you have the mbrlen function.  */
#define HAVE_MBRLEN 1

/* Define if you have the mbsinit function.  */
#define HAVE_MBSINIT 1

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the mkfifo function.  */
#define HAVE_MKFIFO 1

/* Define if you have the mknod function.  */
#define HAVE_MKNOD 1

/* Define if you have the mkstemp function.  */
#define HAVE_MKSTEMP 1

/* Define if you have the mmap function.  */
#define HAVE_MMAP 1

/* Define if you have the nanosleep function.  */
#define HAVE_NANOSLEEP 1

/* Define if you have the nice function.  */
#define HAVE_NICE 1

/* Define if you have the nl_langinfo function.  */
#define HAVE_NL_LANGINFO 1

/* Define if you have the perror function.  */
#define HAVE_PERROR 1

/* Define if you have the poll function.  */
#define HAVE_POLL 1

/* Define if you have the ptsname function.  */
#define HAVE_PTSNAME 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the rand_r function.  */
#define HAVE_RAND_R 1

/* Define if you have the random function.  */
#define HAVE_RANDOM 1

/* Define if you have the realpath function.  */
#define HAVE_REALPATH 1

/* Define if you have the regcomp function.  */
#define HAVE_REGCOMP 1

/* Define if you have the res_search function.  */
#define HAVE_RES_SEARCH 1

/* Define if you have the scandir function.  */
#define HAVE_SCANDIR 1

/* Define if you have the setegid function.  */
#define HAVE_SETEGID 1

/* Define if you have the seteuid function.  */
#define HAVE_SETEUID 1

/* Define if you have the setitimer function.  */
#define HAVE_SETITIMER 1

/* Define if you have the setlocale function.  */
#define HAVE_SETLOCALE 1

/* Define if you have the setpgid function.  */
#define HAVE_SETPGID 1

/* Define if you have the setpriority function.  */
/* #undef HAVE_SETPRIORITY */

/* Define if you have the setsid function.  */
#define HAVE_SETSID 1

/* Define if you have the setsockopt function.  */
#define HAVE_SETSOCKOPT 1

/* Define if you have the setvbuf function.  */
#define HAVE_SETVBUF 1

/* Define if you have the shutdown function.  */
#define HAVE_SHUTDOWN 1

/* Define if you have the sigaction function.  */
/* #undef HAVE_SIGACTION */

/* Define if you have the sin function.  */
#define HAVE_SIN 1

/* Define if you have the snprintf function.  */
#define HAVE_SNPRINTF 1

/* Define if you have the srand48 function.  */
#define HAVE_SRAND48 1

/* Define if you have the srandom function.  */
#define HAVE_SRANDOM 1

/* Define if you have the statfs function.  */
#define HAVE_STATFS 1

/* Define if you have the statvfs function.  */
#define HAVE_STATVFS 1

/* Define if you have the std_syslog function.  */
/* #undef HAVE_STD_SYSLOG */

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strcoll function.  */
#define HAVE_STRCOLL 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strfmon function.  */
#define HAVE_STRFMON 1

/* Define if you have the strftime function.  */
#define HAVE_STRFTIME 1

/* Define if you have the strlcat function.  */
/* #undef HAVE_STRLCAT */

/* Define if you have the strlcpy function.  */
/* #undef HAVE_STRLCPY */

/* Define if you have the strpbrk function.  */
/* #undef HAVE_STRPBRK */

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the strtod function.  */
#define HAVE_STRTOD 1

/* Define if you have the strtok_r function.  */
#define HAVE_STRTOK_R 1

/* Define if you have the strtol function.  */
#define HAVE_STRTOL 1

/* Define if you have the strtoul function.  */
/* #undef HAVE_STRTOUL */

/* Define if you have the strtoull function.  */
/* #undef HAVE_STRTOULL */

/* Define if you have the symlink function.  */
#define HAVE_SYMLINK 1

/* Define if you have the tempnam function.  */
#define HAVE_TEMPNAM 1

/* Define if you have the tzset function.  */
#define HAVE_TZSET 1

/* Define if you have the unlockpt function.  */
#define HAVE_UNLOCKPT 1

/* Define if you have the unsetenv function.  */
#define HAVE_UNSETENV 1

/* Define if you have the usleep function.  */
#define HAVE_USLEEP 1

/* Define if you have the utime function.  */
#define HAVE_UTIME 1

/* Define if you have the vsnprintf function.  */
#define HAVE_VSNPRINTF 1

/* Define if you have the wait3 function.  */
/* #undef HAVE_WAIT3 */

/* Define if you have the waitpid function.  */
/* #undef HAVE_WAITPID */

/* Define if you have the </nsapi.h> header file.  */
/* #undef HAVE__NSAPI_H */

/* Define if you have the <ApplicationServices/ApplicationServices.h> header file.  */
/* #undef HAVE_APPLICATIONSERVICES_APPLICATIONSERVICES_H */

/* Define if you have the <alloca.h> header file.  */
#define HAVE_ALLOCA_H 1

/* Define if you have the <arpa/inet.h> header file.  */
#define HAVE_ARPA_INET_H 1

/* Define if you have the <arpa/nameser.h> header file.  */
#define HAVE_ARPA_NAMESER_H 1

/* Define if you have the <assert.h> header file.  */
#define HAVE_ASSERT_H 1

/* Define if you have the <crypt.h> header file.  */
#define HAVE_CRYPT_H 1

/* Define if you have the <default_store.h> header file.  */
/* #undef HAVE_DEFAULT_STORE_H */

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Define if you have the <errno.h> header file.  */
/* #undef HAVE_ERRNO_H */

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <grp.h> header file.  */
#define HAVE_GRP_H 1

/* Define if you have the <ieeefp.h> header file.  */
/* #undef HAVE_IEEEFP_H */

/* Define if you have the <inttypes.h> header file.  */
#define HAVE_INTTYPES_H 1

/* Define if you have the <langinfo.h> header file.  */
#define HAVE_LANGINFO_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <mach-o/dyld.h> header file.  */
/* #undef HAVE_MACH_O_DYLD_H */

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <monetary.h> header file.  */
#define HAVE_MONETARY_H 1

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <netdb.h> header file.  */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet/in.h> header file.  */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <netinet/tcp.h> header file.  */
/* #undef HAVE_NETINET_TCP_H */

/* Define if you have the <pwd.h> header file.  */
#define HAVE_PWD_H 1

/* Define if you have the <resolv.h> header file.  */
#define HAVE_RESOLV_H 1

/* Define if you have the <signal.h> header file.  */
#define HAVE_SIGNAL_H 1

/* Define if you have the <st.h> header file.  */
/* #undef HAVE_ST_H */

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <stdbool.h> header file.  */
/* #undef HAVE_STDBOOL_H */

/* Define if you have the <stdint.h> header file.  */
#define HAVE_STDINT_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
/* #undef HAVE_STRINGS_H */

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/ioctl.h> header file.  */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/ipc.h> header file.  */
#define HAVE_SYS_IPC_H 1

/* Define if you have the <sys/mkdev.h> header file.  */
/* #undef HAVE_SYS_MKDEV_H */

/* Define if you have the <sys/mman.h> header file.  */
#define HAVE_SYS_MMAN_H 1

/* Define if you have the <sys/mount.h> header file.  */
#define HAVE_SYS_MOUNT_H 1

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/poll.h> header file.  */
#define HAVE_SYS_POLL_H 1

/* Define if you have the <sys/resource.h> header file.  */
#define HAVE_SYS_RESOURCE_H 1

/* Define if you have the <sys/select.h> header file.  */
#define HAVE_SYS_SELECT_H 1

/* Define if you have the <sys/socket.h> header file.  */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/statfs.h> header file.  */
#define HAVE_SYS_STATFS_H 1

/* Define if you have the <sys/statvfs.h> header file.  */
#define HAVE_SYS_STATVFS_H 1

/* Define if you have the <sys/sysexits.h> header file.  */
/* #undef HAVE_SYS_SYSEXITS_H */

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/times.h> header file.  */
/* #undef HAVE_SYS_TIMES_H */

/* Define if you have the <sys/types.h> header file.  */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/un.h> header file.  */
/* #undef HAVE_SYS_UN_H */

/* Define if you have the <sys/utsname.h> header file.  */
#define HAVE_SYS_UTSNAME_H 1

/* Define if you have the <sys/varargs.h> header file.  */
/* #undef HAVE_SYS_VARARGS_H */

/* Define if you have the <sys/vfs.h> header file.  */
#define HAVE_SYS_VFS_H 1

/* Define if you have the <sys/wait.h> header file.  */
#define HAVE_SYS_WAIT_H 1

/* Define if you have the <sysexits.h> header file.  */
#define HAVE_SYSEXITS_H 1

/* Define if you have the <syslog.h> header file.  */
#define HAVE_SYSLOG_H 1

/* Define if you have the <termios.h> header file.  */
#define HAVE_TERMIOS_H 1

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <tuxmodule.h> header file.  */
/* #undef HAVE_TUXMODULE_H */

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the <unix.h> header file.  */
/* #undef HAVE_UNIX_H */

/* Define if you have the <utime.h> header file.  */
#define HAVE_UTIME_H 1

/* Define if you have the <wchar.h> header file.  */
#define HAVE_WCHAR_H 1

/* Define if you have the <xmlparse.h> header file.  */
/* #undef HAVE_XMLPARSE_H */

/* Define if you have the <xmltok.h> header file.  */
/* #undef HAVE_XMLTOK_H */

/* Define if you have the m library (-lm).  */
#define HAVE_LIBM 1

/* Define if you have the nsl library (-lnsl).  */
#define HAVE_LIBNSL 1

/* Define if you have the socket library (-lsocket).  */
/* #undef HAVE_LIBSOCKET */

/* Define if you have the sqlcli library (-lsqlcli).  */
/* #undef HAVE_LIBSQLCLI */

/* Define if processor uses big-endian word */
/* #undef WORDS_BIGENDIAN */

/* Enabling BIND8 compatibility for Panther */
/* #undef BIND_8_COMPAT */

/* Whether you have AOLserver */
/* #undef HAVE_AOLSERVER */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_COMPAT_H */

/*   */
/* #undef HAVE_APACHE */

/*   */
/* #undef HAVE_APACHE */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_COMPAT_H */

/*   */
/* #undef HAVE_OLD_COMPAT_H */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_COMPAT_H */

/*   */
/* #undef HAVE_OLD_COMPAT_H */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_COMPAT_H */

/*   */
/* #undef HAVE_OLD_COMPAT_H */

/*   */
/* #undef USE_TRANSFER_TABLES */

/*   */
/* #undef PHP_APACHE_HAVE_CLIENT_FD */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_COMPAT_H */

/*   */
/* #undef HAVE_APACHE_HOOKS */

/*   */
/* #undef HAVE_APACHE_HOOKS */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_COMPAT_H */

/*   */
/* #undef HAVE_OLD_COMPAT_H */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_COMPAT_H */

/*   */
/* #undef HAVE_OLD_COMPAT_H */

/*   */
/* #undef HAVE_AP_CONFIG_H */

/*   */
/* #undef HAVE_AP_COMPAT_H */

/*   */
/* #undef HAVE_OLD_COMPAT_H */

/*   */
/* #undef USE_TRANSFER_TABLES */

/*   */
/* #undef PHP_APACHE_HAVE_CLIENT_FD */

/* Whether to compile with Caudium support */
/* #undef HAVE_CAUDIUM */

/* Whether you have a Continuity Server */
/* #undef HAVE_CONTINUITY */

/*   */
/* #undef WITH_ZEUS */

/* Whether you have a Netscape/iPlanet/SunONE Server */
/* #undef HAVE_NSAPI */

/* Whether you have phttpd */
/* #undef HAVE_PHTTPD */

/* whether you want Pi3Web support */
/* #undef WITH_PI3WEB */

/* Whether you use Roxen */
/* #undef HAVE_ROXEN */

/* Whether to use Roxen in ZTS mode */
/* #undef ROXEN_USE_ZTS */

/* whether write(2) works */
#define PHP_WRITE_STDOUT 1

/*   */
#define FORCE_CGI_REDIRECT 0

/*   */
#define DISCARD_PATH 0

/*   */
#define ENABLE_PATHINFO_CHECK 1

/*   */
#define PHP_FASTCGI 0

/*   */
#define PHP_FCGI_STATIC 0

/* Define to the necessary symbol if this constant
                           uses a non-standard name on your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define if sockaddr_un in sys/un.h contains a sun_len component */
/* #undef HAVE_SOCKADDR_UN_SUN_LEN */

/* Define if the fpos_t typedef is in stdio.h */
/* #undef HAVE_FPOS */

/* Define if there's a fileno() prototype in stdio.h */
/* #undef HAVE_FILENO_PROTO */

/* Define if the socklen_t typedef is in sys/socket.h */
/* #undef HAVE_SOCKLEN */

/* Define if cross-process locking is required by accept() */
/* #undef USE_LOCKING */

/* Define if va_arg(arg, long double) crashes the compiler */
/* #undef HAVE_VA_ARG_LONG_DOUBLE_BUG */

/* Whether localtime_r is declared */
/* #undef MISSING_LOCALTIME_R_DECL */

/* Whether gmtime_r is declared */
/* #undef MISSING_GMTIME_R_DECL */

/* Whether asctime_r is declared */
/* #undef MISSING_ASCTIME_R_DECL */

/* Whether ctime_r is declared */
/* #undef MISSING_CTIME_R_DECL */

/* Whether strtok_r is declared */
/* #undef MISSING_STRTOK_R_DECL */

/* whether you have sendmail */
#define HAVE_SENDMAIL 1

/* Define if system uses EBCDIC */
/* #undef CHARSET_EBCDIC */

/*   */
#define HAVE_SOCKET 1

/*   */
#define HAVE_SOCKET 1

/*   */
/* #undef HAVE_LIBSOCKET */

/*   */
#define HAVE_HTONL 1

/*   */
#define HAVE_HTONL 1

/*   */
/* #undef HAVE_LIBSOCKET */

/*   */
#define HAVE_GETHOSTNAME 1

/*   */
#define HAVE_GETHOSTNAME 1

/*   */
#define HAVE_LIBNSL 1

/*   */
#define HAVE_GETHOSTBYADDR 1

/*   */
#define HAVE_GETHOSTBYADDR 1

/*   */
#define HAVE_LIBNSL 1

/*   */
#define HAVE_YP_GET_DEFAULT_DOMAIN 1

/*   */
#define HAVE_YP_GET_DEFAULT_DOMAIN 1

/*   */
#define HAVE_LIBNSL 1

/*   */
#define HAVE_DLOPEN 1

/*   */
#define HAVE_DLOPEN 1

/*   */
#define HAVE_LIBDL 1

/*   */
#define HAVE_LIBDL 1

/*   */
#define HAVE_RES_SEARCH 1

/*   */
#define HAVE_RES_SEARCH 1

/*   */
#define HAVE_LIBRESOLV 1

/*   */
#define HAVE_RES_SEARCH 1

/*   */
/* #undef HAVE_LIBBIND */

/*   */
#define HAVE_RES_SEARCH 1

/*   */
/* #undef HAVE_LIBSOCKET */

/*   */
#define HAVE_INET_ATON 1

/*   */
#define HAVE_INET_ATON 1

/*   */
#define HAVE_LIBRESOLV 1

/*   */
#define HAVE_INET_ATON 1

/*   */
/* #undef HAVE_LIBBIND */

/*   */
#define HAVE_DN_SKIPNAME 1

/*   */
#define HAVE_DN_SKIPNAME 1

/*   */
#define HAVE_LIBRESOLV 1

/*   */
#define HAVE_DN_SKIPNAME 1

/*   */
/* #undef HAVE_LIBBIND */

/*   */
#define MISSING_FCLOSE_DECL 0

/*   */
#define MISSING_FCLOSE_DECL 0

/*   */
#define HAVE_FOPENCOOKIE 1

/*   */
#define COOKIE_IO_FUNCTIONS_T cookie_io_functions_t

/*   */
#define COOKIE_SEEKER_USES_OFF64_T 1

/* Define if system has broken getcwd */
/* #undef HAVE_BROKEN_GETCWD */

/* Define if your glibc borks on fopen with mode a+ */
#define HAVE_BROKEN_GLIBC_FOPEN_APPEND 1

/* whether you have tm_gmtoff in struct tm */
#define HAVE_TM_GMTOFF 1

/* whether you have struct flock */
#define HAVE_STRUCT_FLOCK 1

/* Whether you have socklen_t */
#define HAVE_SOCKLEN_T 1

/* Whether you have struct sockaddr_storage */
#define HAVE_SOCKADDR_STORAGE 1

/* Whether sockaddr struct has sa_len */
/* #undef HAVE_SOCKADDR_LEN */

/* Define if you have the getaddrinfo function */
#define HAVE_GETADDRINFO 1

/* Whether system headers declare timezone */
#define HAVE_DECLARED_TIMEZONE 1

/* Whether you have HP-UX 10.x */
/* #undef PHP_HPUX_TIME_R */

/* Whether you have IRIX-style functions */
/* #undef PHP_IRIX_TIME_R */

/* whether you have POSIX readdir_r */
#define HAVE_POSIX_READDIR_R 1

/* whether you have old-style readdir_r */
/* #undef HAVE_OLD_READDIR_R */

/*   */
/* #undef in_addr_t */

/*   */
#define PHP_SAFE_MODE 0

/*   */
#define PHP_SAFE_MODE 0

/*   */
#define PHP_SAFE_MODE_EXEC_DIR "/usr/local/php/bin"

/*   */
#define PHP_SAFE_MODE_EXEC_DIR "/usr/local/php/bin"

/*   */
#define PHP_SAFE_MODE_EXEC_DIR "/usr/local/php/bin"

/*   */
#define PHP_SAFE_MODE_EXEC_DIR "/usr/local/php/bin"

/*   */
#define PHP_SIGCHILD 0

/*   */
#define PHP_SIGCHILD 0

/*   */
#define MAGIC_QUOTES 0

/*   */
#define MAGIC_QUOTES 0

/*   */
#define DEFAULT_SHORT_OPEN_TAG "1"

/*   */
#define DEFAULT_SHORT_OPEN_TAG "1"

/* Whether you have dmalloc */
/* #undef HAVE_DMALLOC */

/* Whether to enable IPv6 support */
#define HAVE_IPV6 1

/*   */
#define HAVE_LIBXML 1

/*   */
#define HAVE_LIBXML 1

/* Whether to build libxml as dynamic module */
/* #undef COMPILE_DL_LIBXML */

/* Whether to build openssl as dynamic module */
/* #undef COMPILE_DL_OPENSSL */

/*   */
/* #undef HAVE_OPENSSL_EXT */

/* Whether to build zlib as dynamic module */
/* #undef COMPILE_DL_ZLIB */

/*   */
/* #undef HAVE_ZLIB */

/* Whether to build bcmath as dynamic module */
/* #undef COMPILE_DL_BCMATH */

/* Whether you have bcmath */
/* #undef HAVE_BCMATH */

/*   */
/* #undef HAVE_BZ2 */

/* Whether to build bz2 as dynamic module */
/* #undef COMPILE_DL_BZ2 */

/*   */
/* #undef HAVE_CALENDAR */

/* Whether to build calendar as dynamic module */
/* #undef COMPILE_DL_CALENDAR */

/* Whether to build cpdf as dynamic module */
/* #undef COMPILE_DL_CPDF */

/* Whether you have cpdflib */
/* #undef HAVE_CPDFLIB */

/*   */
#define HAVE_CTYPE 1

/* Whether to build ctype as dynamic module */
/* #undef COMPILE_DL_CTYPE */

/*   */
/* #undef HAVE_CURL */

/*   */
/* #undef HAVE_CURL_VERSION_INFO */

/*   */
/* #undef PHP_CURL_URL_WRAPPERS */

/* Whether to build curl as dynamic module */
/* #undef COMPILE_DL_CURL */

/*   */
/* #undef QDBM_INCLUDE_FILE */

/*   */
/* #undef DBA_QDBM */

/*   */
/* #undef GDBM_INCLUDE_FILE */

/*   */
/* #undef DBA_GDBM */

/*   */
/* #undef NDBM_INCLUDE_FILE */

/*   */
/* #undef DBA_NDBM */

/*   */
/* #undef DBA_DB4 */

/*   */
/* #undef DB4_INCLUDE_FILE */

/*   */
/* #undef DBA_DB3 */

/*   */
/* #undef DB3_INCLUDE_FILE */

/*   */
/* #undef DBA_DB2 */

/*   */
/* #undef DB2_INCLUDE_FILE */

/*   */
/* #undef DBM_INCLUDE_FILE */

/*   */
/* #undef DBM_VERSION */

/*   */
/* #undef DBM_VERSION */

/*   */
/* #undef DBA_DBM */

/*   */
/* #undef DBA_CDB_BUILTIN */

/*   */
/* #undef DBA_CDB_MAKE */

/*   */
/* #undef DBA_CDB */

/*   */
/* #undef CDB_INCLUDE_FILE */

/*   */
/* #undef DBA_CDB */

/*   */
/* #undef DBA_CDB_BUILTIN */

/*   */
/* #undef DBA_CDB_MAKE */

/*   */
/* #undef DBA_CDB */

/*   */
/* #undef DBA_INIFILE */

/*   */
/* #undef DBA_INIFILE */

/*   */
/* #undef DBA_FLATFILE */

/*   */
/* #undef DBA_FLATFILE */

/*   */
/* #undef HAVE_DBA */

/* Whether to build dba as dynamic module */
/* #undef COMPILE_DL_DBA */

/*   */
/* #undef DBASE */

/* Whether to build dbase as dynamic module */
/* #undef COMPILE_DL_DBASE */

/* Whether to build dbx as dynamic module */
/* #undef COMPILE_DL_DBX */

/* Whether to build dio as dynamic module */
/* #undef COMPILE_DL_DIO */

/*   */
#define HAVE_LIBXML 1

/*   */
#define HAVE_DOM 1

/* Whether to build dom as dynamic module */
/* #undef COMPILE_DL_DOM */

/* Whether you want EXIF (metadata from images) support */
/* #undef HAVE_EXIF */

/* Whether to build exif as dynamic module */
/* #undef COMPILE_DL_EXIF */

/*   */
/* #undef HAVE_FAMLIB */

/* Whether to build fam as dynamic module */
/* #undef COMPILE_DL_FAM */

/* Whether you have FrontBase */
/* #undef HAVE_FBSQL */

/* Whether to build fbsql as dynamic module */
/* #undef COMPILE_DL_FBSQL */

/*   */
/* #undef HAVE_FDFTK_H_LOWER */

/*   */
/* #undef HAVE_FDFTK_5 */

/* Whether to build fdf as dynamic module */
/* #undef COMPILE_DL_FDF */

/*   */
/* #undef HAVE_FDFLIB */

/*   */
/* #undef HAVE_FILEPRO */

/* Whether to build filepro as dynamic module */
/* #undef COMPILE_DL_FILEPRO */

/* Whether you want FTP support */
/* #undef HAVE_FTP */

/* Whether to build ftp as dynamic module */
/* #undef COMPILE_DL_FTP */

/*   */
/* #undef USE_GD_IMGSTRTTF */

/*   */
/* #undef USE_GD_IMGSTRTTF */

/*   */
/* #undef HAVE_LIBFREETYPE */

/*   */
/* #undef HAVE_LIBTTF */

/*   */
/* #undef HAVE_LIBT1 */

/*   */
/* #undef HAVE_LIBGD */

/*   */
/* #undef HAVE_LIBGD13 */

/*   */
/* #undef HAVE_LIBGD15 */

/*   */
/* #undef HAVE_LIBGD20 */

/*   */
/* #undef HAVE_LIBGD204 */

/*   */
/* #undef HAVE_GD_IMAGESETTILE */

/*   */
/* #undef HAVE_GD_IMAGESETBRUSH */

/*   */
/* #undef HAVE_GDIMAGECOLORRESOLVE */

/*   */
/* #undef HAVE_COLORCLOSESTHWB */

/*   */
/* #undef HAVE_GD_WBMP */

/*   */
/* #undef HAVE_GD_GD2 */

/*   */
/* #undef HAVE_GD_PNG */

/*   */
/* #undef HAVE_GD_XBM */

/*   */
/* #undef HAVE_GD_BUNDLED */

/*   */
/* #undef HAVE_GD_GIF_READ */

/*   */
/* #undef HAVE_GD_GIF_CREATE */

/*   */
/* #undef HAVE_GD_IMAGEELLIPSE */

/*   */
/* #undef HAVE_GD_FONTCACHESHUTDOWN */

/*   */
/* #undef HAVE_GD_DYNAMIC_CTX_EX */

/*   */
/* #undef HAVE_GD_GIF_CTX */

/*   */
/* #undef HAVE_GD_JPG */

/*   */
/* #undef HAVE_GD_XPM */

/*   */
/* #undef HAVE_GD_STRINGFT */

/*   */
/* #undef HAVE_GD_STRINGFTEX */

/*   */
/* #undef USE_GD_JISX0208 */

/*   */
/* #undef USE_GD_IMGSTRTTF */

/*   */
/* #undef USE_GD_IMGSTRTTF */

/*   */
/* #undef HAVE_LIBFREETYPE */

/*   */
/* #undef HAVE_LIBTTF */

/*   */
/* #undef HAVE_LIBT1 */

/*   */
/* #undef HAVE_LIBGD */

/*   */
/* #undef HAVE_LIBGD13 */

/*   */
/* #undef HAVE_LIBGD15 */

/*   */
/* #undef HAVE_GD_PNG */

/*   */
/* #undef HAVE_GD_GIF_READ */

/*   */
/* #undef HAVE_GD_GIF_CREATE */

/*   */
/* #undef HAVE_GD_WBMP */

/*   */
/* #undef HAVE_GD_JPG */

/*   */
/* #undef HAVE_GD_XPM */

/*   */
/* #undef HAVE_GD_GD2 */

/*   */
/* #undef HAVE_LIBGD20 */

/*   */
/* #undef HAVE_GD_IMAGESETTILE */

/*   */
/* #undef HAVE_GD_IMAGEELLIPSE */

/*   */
/* #undef HAVE_GD_IMAGESETBRUSH */

/*   */
/* #undef HAVE_GD_STRINGTTF */

/*   */
/* #undef HAVE_GD_STRINGFT */

/*   */
/* #undef HAVE_GD_STRINGFTEX */

/*   */
/* #undef HAVE_COLORCLOSESTHWB */

/*   */
/* #undef HAVE_GDIMAGECOLORRESOLVE */

/*   */
/* #undef HAVE_GD_GIF_CTX */

/*   */
/* #undef HAVE_GD_CACHE_CREATE */

/*   */
/* #undef HAVE_GD_FONTCACHESHUTDOWN */

/*   */
/* #undef HAVE_GD_FREEFONTCACHE */

/*   */
/* #undef HAVE_GD_DYNAMIC_CTX_EX */

/*   */
/* #undef HAVE_LIBGD204 */

/* Whether to build gd as dynamic module */
/* #undef COMPILE_DL_GD */

/*   */
/* #undef HAVE_LIBINTL */

/* Whether to build gettext as dynamic module */
/* #undef COMPILE_DL_GETTEXT */

/*   */
/* #undef HAVE_NGETTEXT */

/*   */
/* #undef HAVE_DNGETTEXT */

/*   */
/* #undef HAVE_DCNGETTEXT */

/*   */
/* #undef HAVE_BIND_TEXTDOMAIN_CODESET */

/* Whether to build gmp as dynamic module */
/* #undef COMPILE_DL_GMP */

/*   */
/* #undef HAVE_GMP */

/*   */
/* #undef HAVE_HWAPI */

/* Whether to build hwapi as dynamic module */
/* #undef COMPILE_DL_HWAPI */

/*   */
/* #undef HAVE_GICONV_H */

/*   */
/* #undef HAVE_ICONV */

/* Which iconv implementation to use */
#define PHP_ICONV_IMPL "glibc"

/* Konstantin Chuguev's iconv implementation */
/* #undef HAVE_BSD_ICONV */

/* Which iconv implementation to use */
#define PHP_ICONV_IMPL "glibc"

/* glibc's iconv implementation */
#define HAVE_GLIBC_ICONV 1

/* Which iconv implementation to use */
#define PHP_ICONV_IMPL "glibc"

/* Whether iconv supports error no or not */
#define ICONV_SUPPORTS_ERRNO 1

/* Whether iconv supports error no or not */
#define ICONV_SUPPORTS_ERRNO 1

/* Path to iconv.h */
#define PHP_ICONV_H_PATH </usr/include/iconv.h>

/* Whether to build iconv as dynamic module */
/* #undef COMPILE_DL_ICONV */

/* Whether to build imap as dynamic module */
/* #undef COMPILE_DL_IMAP */

/*   */
/* #undef HAVE_IMAP */

/*   */
/* #undef HAVE_IMAP2000 */

/*   */
/* #undef HAVE_IMAP2000 */

/*   */
/* #undef HAVE_IMAP2000 */

/*   */
/* #undef HAVE_IMAP2000 */

/*   */
/* #undef HAVE_IMAP2000 */

/*   */
/* #undef HAVE_IMAP2000 */

/*   */
/* #undef HAVE_IMAP2004 */

/*   */
/* #undef HAVE_IMAP2001 */

/*   */
/* #undef HAVE_LIBPAM */

/*   */
/* #undef HAVE_LIBCRYPT */

/*   */
/* #undef HAVE_IMAP_KRB */

/*   */
/* #undef HAVE_IMAP_SSL */

/*   */
/* #undef HAVE_IMAP_AUTH_GSS */

/*   */
/* #undef IFX_VERSION */

/*   */
/* #undef HAVE_IFX_IUS */

/* Whether to build informix as dynamic module */
/* #undef COMPILE_DL_INFORMIX */

/*   */
/* #undef HAVE_IFX */

/* Whether you have Ingres II */
/* #undef HAVE_II */

/* Whether to build ingres_ii as dynamic module */
/* #undef COMPILE_DL_INGRES_II */

/*   */
/* #undef HAVE_IBASE */

/* Whether to build interbase as dynamic module */
/* #undef COMPILE_DL_INTERBASE */

/* Whether to build ircg as dynamic module */
/* #undef COMPILE_DL_IRCG */

/* Whether you want IRCG support */
/* #undef HAVE_IRCG */

/* Whether to build ldap as dynamic module */
/* #undef COMPILE_DL_LDAP */

/*   */
/* #undef HAVE_NSLDAP */

/*   */
/* #undef HAVE_NSLDAP */

/*   */
/* #undef HAVE_NSLDAP */

/*   */
/* #undef HAVE_NSLDAP */

/*   */
/* #undef HAVE_ORALDAP */

/*   */
/* #undef HAVE_ORALDAP_10 */

/*   */
/* #undef HAVE_LDAP */

/* Whether 3 arg set_rebind_proc() */
/* #undef HAVE_3ARG_SETREBINDPROC */

/*   */
/* #undef HAVE_LDAP_SASL_SASL_H */

/*   */
/* #undef HAVE_LDAP_SASL_H */

/* LDAP SASL support */
/* #undef HAVE_LDAP_SASL */

/* whether to have multibyte string support */
/* #undef HAVE_MBSTRING */

/* Define if stdarg.h is available */
/* #undef HAVE_STDARG_PROTOTYPES */

/* whether to have multibyte regex support */
/* #undef HAVE_MBREGEX */

/* Whether to build mbstring as dynamic module */
/* #undef COMPILE_DL_MBSTRING */

/*   */
/* #undef HAVE_LIBMCRYPT */

/*   */
/* #undef HAVE_LIBMCRYPT */

/* Whether to build mcrypt as dynamic module */
/* #undef COMPILE_DL_MCRYPT */

/* Whether or not we're using libmonetra 4.2 or higher  */
/* #undef LIBMONETRA_BELOW_4_2 */

/*   */
/* #undef HAVE_MCVE */

/* Whether to build mcve as dynamic module */
/* #undef COMPILE_DL_MCVE */

/* Whether to build mhash as dynamic module */
/* #undef COMPILE_DL_MHASH */

/*   */
/* #undef HAVE_LIBMHASH */

/* Whether to build mime_magic as dynamic module */
/* #undef COMPILE_DL_MIME_MAGIC */

/* magic file path */
/* #undef PHP_MIME_MAGIC_FILE_PATH */

/*   */
/* #undef HAVE_MING */

/*   */
/* #undef HAVE_DESTROY_SWF_BLOCK */

/*   */
/* #undef HAVE_SWFPREBUILTCLIP */

/*   */
/* #undef HAVE_NEW_MING */

/*   */
/* #undef HAVE_MING_ZLIB */

/* Whether to build ming as dynamic module */
/* #undef COMPILE_DL_MING */

/*   */
/* #undef HAVE_MNOGOSEARCH */

/* Whether to build mnogosearch as dynamic module */
/* #undef COMPILE_DL_MNOGOSEARCH */

/*   */
/* #undef HAVE_MSESSION */

/* Whether to build msession as dynamic module */
/* #undef COMPILE_DL_MSESSION */

/*   */
/* #undef HAVE_MSQL */

/* Whether to build msql as dynamic module */
/* #undef COMPILE_DL_MSQL */

/*   */
/* #undef MSQL1 */

/*   */
/* #undef MSQL1 */

/* Whether to build mssql as dynamic module */
/* #undef COMPILE_DL_MSSQL */

/*   */
/* #undef HAVE_LIBDNET_STUB */

/*   */
/* #undef HAVE_MSSQL */

/*   */
/* #undef HAVE_FREETDS */

/* Whether you have MySQL */
/* #undef HAVE_MYSQL */

/*   */
/* #undef MYSQL_UNIX_ADDR */

/*   */
/* #undef MYSQL_UNIX_ADDR */

/* Whether to build mysql as dynamic module */
/* #undef COMPILE_DL_MYSQL */

/* embedded MySQL support enabled */
/* #undef HAVE_EMBEDDED_MYSQLI */

/*   */
/* #undef HAVE_MYSQLILIB */

/* Whether to build mysqli as dynamic module */
/* #undef COMPILE_DL_MYSQLI */

/*   */
/* #undef HAVE_NCURSES_H */

/*   */
/* #undef HAVE_NCURSESLIB */

/*   */
/* #undef HAVE_NCURSES_PANEL */

/*   */
/* #undef HAVE_NCURSES_COLOR_SET */

/*   */
/* #undef HAVE_NCURSES_SLK_COLOR */

/*   */
/* #undef HAVE_NCURSES_ASSUME_DEFAULT_COLORS */

/*   */
/* #undef HAVE_NCURSES_USE_EXTENDED_NAMES */

/* Whether to build ncurses as dynamic module */
/* #undef COMPILE_DL_NCURSES */

/*   */
/* #undef HAVE_OCI8_ATTR_STATEMENT */

/*   */
/* #undef HAVE_OCI8_ATTR_STATEMENT */

/*   */
/* #undef HAVE_OCI_9_2 */

/*   */
/* #undef HAVE_OCI8_ATTR_STATEMENT */

/*   */
/* #undef HAVE_OCI_9_2 */

/*   */
/* #undef HAVE_OCI8_TEMP_LOB */

/*   */
/* #undef PHP_OCI8_HAVE_COLLECTIONS */

/*   */
/* #undef HAVE_OCI8_TEMP_LOB */

/*   */
/* #undef HAVE_OCI8_TEMP_LOB */

/*   */
/* #undef PHP_OCI8_HAVE_COLLECTIONS */

/* Whether to build oci8 as dynamic module */
/* #undef COMPILE_DL_OCI8 */

/*   */
/* #undef HAVE_OCI8 */

/*   */
/* #undef HAVE_OCI8_ATTR_STATEMENT */

/*   */
/* #undef HAVE_OCI_9_2 */

/*   */
/* #undef HAVE_OCI8_TEMP_LOB */

/*   */
/* #undef PHP_OCI8_HAVE_COLLECTIONS */

/*   */
/* #undef HAVE_OCI_INSTANT_CLIENT */

/* Whether to build oci8 as dynamic module */
/* #undef COMPILE_DL_OCI8 */

/*   */
/* #undef HAVE_OCI8 */

/*   */
/* #undef HAVE_ADABAS */

/*   */
/* #undef HAVE_SAPDB */

/*   */
/* #undef HAVE_SOLID_35 */

/*   */
/* #undef HAVE_SOLID_30 */

/*   */
/* #undef HAVE_SOLID */

/* Needed in sqlunix.h  */
/* #undef SS_LINUX */

/* Needed in sqlunix.h  */
/* #undef SS_LINUX */

/* Needed in sqlunix.h for wchar defs  */
/* #undef SS_FBX */

/* Needed in sqlunix.h for wchar defs  */
/* #undef SS_FBX */

/*   */
/* #undef HAVE_IBMDB2 */

/*   */
/* #undef HAVE_EMPRESS */

/*   */
/* #undef HAVE_EMPRESS */

/*   */
/* #undef AIX */

/*   */
/* #undef HPUX */

/*   */
/* #undef LINUX */

/*   */
/* #undef NEUTRINO */

/*   */
/* #undef ISOLARIS */

/*   */
/* #undef SOLARIS */

/*   */
/* #undef UNIXWARE */

/*   */
/* #undef HAVE_BIRDSTEP */

/*   */
/* #undef HAVE_CODBC */

/*   */
/* #undef HAVE_IODBC */

/*   */
/* #undef HAVE_ODBC2 */

/*   */
/* #undef HAVE_ESOOB */

/*   */
/* #undef HAVE_UNIXODBC */

/* Whether you want DBMaker */
/* #undef HAVE_DBMAKER */

/*   */
/* #undef HAVE_SQLDATASOURCES */

/*   */
/* #undef HAVE_UODBC */

/* Whether to build odbc as dynamic module */
/* #undef COMPILE_DL_ODBC */

/* Whether to build oracle as dynamic module */
/* #undef COMPILE_DL_ORACLE */

/*   */
/* #undef HAVE_ORACLE */

/* Whether to build ovrimos as dynamic module */
/* #undef COMPILE_DL_OVRIMOS */

/*   */
/* #undef HAVE_LIBSQLCLI */

/*   */
#define HAVE_FORK 1

/*   */
/* #undef HAVE_WAITPID */

/*   */
/* #undef HAVE_SIGACTION */

/* Whether to build pcntl as dynamic module */
/* #undef COMPILE_DL_PCNTL */

/* Whether to build pcre as dynamic module */
/* #undef COMPILE_DL_PCRE */

/*   */
#define HAVE_BUNDLED_PCRE 1

/*   */
/* #undef HAVE_PCRE */

/* Whether to build pcre as dynamic module */
/* #undef COMPILE_DL_PCRE */

/* Version of SDK */
/* #undef PFPRO_VERSION */

/* Whether to build pfpro as dynamic module */
/* #undef COMPILE_DL_PFPRO */

/*   */
/* #undef HAVE_PFPRO */

/* Whether to have pg_config.h */
/* #undef HAVE_PG_CONFIG_H */

/* Whether to have pg_config.h */
/* #undef HAVE_PG_CONFIG_H */

/* Whether to build PostgreSQL support or not */
/* #undef HAVE_PGSQL */

/* PostgreSQL 7.2.0 or later */
/* #undef HAVE_PQESCAPE */

/* PostgreSQL 7.0.x or later */
/* #undef HAVE_PQSETNONBLOCKING */

/* Broken libpq under windows */
/* #undef HAVE_PQCMDTUPLES */

/* Older PostgreSQL */
/* #undef HAVE_PQOIDVALUE */

/* PostgreSQL 7.0.x or later */
/* #undef HAVE_PQCLIENTENCODING */

/* PostgreSQL 7.4 or later */
/* #undef HAVE_PQPARAMETERSTATUS */

/* PostgreSQL 7.4 or later */
/* #undef HAVE_PQPROTOCOLVERSION */

/* PostgreSQL 7.4 or later */
/* #undef HAVE_PGTRANSACTIONSTATUS */

/* Whether libpq is compiled with --enable-multibyte */
/* #undef HAVE_PGSQL_WITH_MULTIBYTE_SUPPORT */

/* Whether to build pgsql as dynamic module */
/* #undef COMPILE_DL_PGSQL */

/* whether to include POSIX-like functions */
#define HAVE_POSIX 1

/* Whether to build posix as dynamic module */
/* #undef COMPILE_DL_POSIX */

/* Whether to build pspell as dynamic module */
/* #undef COMPILE_DL_PSPELL */

/*   */
/* #undef HAVE_PSPELL */

/* Whether to build readline as dynamic module */
/* #undef COMPILE_DL_READLINE */

/*   */
/* #undef HAVE_LIBREADLINE */

/* Whether to build readline as dynamic module */
/* #undef COMPILE_DL_READLINE */

/*   */
/* #undef HAVE_LIBEDIT */

/* Whether we have librecode 3.5 */
/* #undef HAVE_BROKEN_RECODE */

/* Whether we have librecode 3.5 or higher */
/* #undef HAVE_LIBRECODE */

/* Whether to build recode as dynamic module */
/* #undef COMPILE_DL_RECODE */

/*   */
#define HAVE_PWRITE 1

/* whether pwrite64 is default */
/* #undef PHP_PWRITE_64 */

/*   */
#define HAVE_PREAD 1

/* whether pread64 is default */
/* #undef PHP_PREAD_64 */

/* Whether to build session as dynamic module */
/* #undef COMPILE_DL_SESSION */

/*   */
#define HAVE_PHP_SESSION 1

/* Whether you have libmm */
/* #undef HAVE_LIBMM */

/*   */
/* #undef HAVE_SHMOP */

/* Whether to build shmop as dynamic module */
/* #undef COMPILE_DL_SHMOP */

/*   */
#define HAVE_LIBXML 1

/*   */
#define HAVE_SIMPLEXML 1

/* Whether to build simplexml as dynamic module */
/* #undef COMPILE_DL_SIMPLEXML */

/*   */
/* #undef HAVE_NET_SNMP */

/*   */
/* #undef HAVE_SNMP_PARSE_OID */

/*   */
/* #undef HAVE_SNMP */

/* Whether to build snmp as dynamic module */
/* #undef COMPILE_DL_SNMP */

/*   */
/* #undef UCD_SNMP_HACK */

/*   */
#define HAVE_LIBXML 1

/*   */
/* #undef HAVE_SOAP */

/* Whether to build soap as dynamic module */
/* #undef COMPILE_DL_SOAP */

/* Whether you have struct cmsghdr */
/* #undef HAVE_CMSGHDR */

/*   */
/* #undef MISSING_MSGHDR_MSGFLAGS */

/*   */
/* #undef HAVE_SOCKETS */

/* Whether to build sockets as dynamic module */
/* #undef COMPILE_DL_SOCKETS */

/*   */
#define HAVE_GETHOSTBYNAME2 1

/* Whether you want SPL (Standard PHP Library) support */
#define HAVE_SPL 1

/* Whether to build spl as dynamic module */
/* #undef COMPILE_DL_SPL */

/*   */
/* #undef HAVE_SQLITELIB */

/* Whether to build sqlite as dynamic module */
/* #undef COMPILE_DL_SQLITE */

/* Whether to build sqlite as dynamic module */
/* #undef COMPILE_DL_SQLITE */

/* Size of a pointer */
#define SQLITE_PTR_SZ SIZEOF_CHAR_P

/*   */
/* #undef SQLITE_UTF8 */

/*   */
#define HAVE_CRYPT 1

/* Whether the system supports standard DES salt */
#define PHP_STD_DES_CRYPT 1

/* Whether the system supports extended DES salt */
#define PHP_EXT_DES_CRYPT 0

/* Whether the system supports MD5 salt */
#define PHP_MD5_CRYPT 1

/* Whether the system supports BlowFish salt */
#define PHP_BLOWFISH_CRYPT 0

/* Define if flush should be called explicitly after a buffered io. */
/* #undef HAVE_FLUSHIO */

/*   */
#define HAVE_REGEX_T_RE_MAGIC 1

/*   */
#define HSREGEX 1

/*   */
#define REGEX 1

/*   */
#define REGEX 1

/* 1 */
#define HAVE_REGEX_T_RE_MAGIC 1

/*  see #24142  */
#define PHP_ROUND_FUZZ 0.50000000001

/* Define if your system has fork/vfork/CreateProcess */
#define PHP_CAN_SUPPORT_PROC_OPEN 1

/* Whether to enable chroot() function */
#define ENABLE_CHROOT_FUNC 1

/*   */
#define HAVE_RES_NMKQUERY 1

/*   */
#define HAVE_RES_NMKQUERY 1

/*   */
#define HAVE_LIBRESOLV 1

/*   */
#define HAVE_RES_NMKQUERY 1

/*   */
/* #undef HAVE_LIBBIND */

/*   */
#define HAVE_RES_NMKQUERY 1

/*   */
/* #undef HAVE_LIBSOCKET */

/*   */
#define HAVE_RES_NSEND 1

/*   */
#define HAVE_RES_NSEND 1

/*   */
#define HAVE_LIBRESOLV 1

/*   */
#define HAVE_RES_NSEND 1

/*   */
/* #undef HAVE_LIBBIND */

/*   */
#define HAVE_RES_NSEND 1

/*   */
/* #undef HAVE_LIBSOCKET */

/*   */
#define HAVE_DN_EXPAND 1

/*   */
#define HAVE_DN_EXPAND 1

/*   */
#define HAVE_LIBRESOLV 1

/*   */
#define HAVE_DN_EXPAND 1

/*   */
/* #undef HAVE_LIBBIND */

/*   */
#define HAVE_DN_EXPAND 1

/*   */
/* #undef HAVE_LIBSOCKET */

/* whether atof() accepts NAN */
#define HAVE_ATOF_ACCEPTS_NAN 1

/* whether atof() accepts INF */
#define HAVE_ATOF_ACCEPTS_INF 1

/* whether HUGE_VAL == INF */
#define HAVE_HUGE_VAL_INF 1

/* whether HUGE_VAL + -HUGEVAL == NAN */
#define HAVE_HUGE_VAL_NAN 1

/* Define if your system has mbstate_t in wchar.h */
#define HAVE_MBSTATE_T 1

/* Whether to build standard as dynamic module */
/* #undef COMPILE_DL_STANDARD */

/* Whether to build sybase as dynamic module */
/* #undef COMPILE_DL_SYBASE */

/*   */
/* #undef HAVE_LIBDNET_STUB */

/*   */
/* #undef HAVE_SYBASE */

/*   */
/* #undef PHP_SYBASE_DBOPEN */

/*   */
/* #undef DBMFIX */

/*   */
/* #undef PHP_SYBASE_DBOPEN */

/*   */
/* #undef HAVE_SYBASE_CT */

/* Whether to build sybase_ct as dynamic module */
/* #undef COMPILE_DL_SYBASE_CT */

/*   */
/* #undef HAVE_SYSVMSG */

/* Whether to build sysvmsg as dynamic module */
/* #undef COMPILE_DL_SYSVMSG */

/* Whether to build sysvsem as dynamic module */
/* #undef COMPILE_DL_SYSVSEM */

/*   */
/* #undef HAVE_SYSVSEM */

/*   */
/* #undef HAVE_SEMUN */

/*   */
/* #undef HAVE_SEMUN */

/*   */
/* #undef HAVE_SYSVSHM */

/* Whether to build sysvshm as dynamic module */
/* #undef COMPILE_DL_SYSVSHM */

/* Whether to build tidy as dynamic module */
/* #undef COMPILE_DL_TIDY */

/*   */
/* #undef HAVE_TIDY */

/* Whether to build tokenizer as dynamic module */
/* #undef COMPILE_DL_TOKENIZER */

/*   */
/* #undef HAVE_WDDX */

/* Whether to build wddx as dynamic module */
/* #undef COMPILE_DL_WDDX */

/*   */
#define HAVE_LIBXML 1

/*   */
/* #undef HAVE_LIBEXPAT */

/* Whether to build xml as dynamic module */
/* #undef COMPILE_DL_XML */

/*   */
#define HAVE_XML 1

/*   */
/* #undef HAVE_XMLRPC */

/*   */
/* #undef HAVE_LIBEXPAT2 */

/*   */
/* #undef HAVE_GICONV_H */

/*   */
/* #undef HAVE_ICONV */

/*   */
/* #undef UNDEF_THREADS_HACK */

/* Whether to build xmlrpc as dynamic module */
/* #undef COMPILE_DL_XMLRPC */

/*   */
/* #undef HAVE_XSL_EXSLT */

/*   */
/* #undef HAVE_XSL */

/* Whether to build xsl as dynamic module */
/* #undef COMPILE_DL_XSL */

/*   */
/* #undef HAVE_YP */

/* Whether to build yp as dynamic module */
/* #undef COMPILE_DL_YP */

/*   */
/* #undef SOLARIS_YP */

/* Define if int32_t type is present.  */
#define HAVE_INT32_T 1

/* Define if uint32_t type is present.  */
#define HAVE_UINT32_T 1

/* Whether sprintf is broken */
#define ZEND_BROKEN_SPRINTF 0

/* whether floatingpoint.h defines fp_except */
/* #undef HAVE_FP_EXCEPT */

/* Define if dlsym() requires a leading underscore in symbol names.  */
/* #undef DLSYM_NEEDS_UNDERSCORE */

/*   */
#define ZEND_DEBUG 0

/*   */
#define ZEND_DEBUG 0

/* Use Zend memory manager */
#define USE_ZEND_ALLOC 1

/* Use Zend memory manager */
#define USE_ZEND_ALLOC 1

/*   */
/* #undef ZTS */

/* Memory limit */
#define MEMORY_LIMIT 0

/* Memory limit */
#define MEMORY_LIMIT 0

/*   */
/* #undef ZEND_MULTIBYTE */

/*   */
#define ZEND_MM_ALIGNMENT 4

/*   */
#define ZEND_MM_ALIGNMENT_LOG2 2

/*   */
/* #undef ZTS */

/* Whether you use GNU Pth */
/* #undef GNUPTH */

/*   */
/* #undef TSRM_ST */

/* Whether to use native BeOS threads */
/* #undef BETHREADS */

/* Whether to use Pthreads */
/* #undef PTHREADS */

/* PHP build date */
#define PHP_BUILD_DATE "2005-11-10"

/* hardcode for each of the cross compiler host */
#define PHP_OS "Linux"

/* hardcode for each of the cross compiler host */
#define PHP_UNAME "Linux localhost.localdomain 2.4.21-4.ELsmp #1 SMP Fri Oct 3 17:52:56 EDT 2003 i686 i686 i386 GNU/Linux"

/* uname -a output */
#define PHP_UNAME "Linux localhost.localdomain 2.4.21-4.ELsmp #1 SMP Fri Oct 3 17:52:56 EDT 2003 i686 i686 i386 GNU/Linux"

/* uname output */
#define PHP_OS "Linux"

/*   */
#define HAVE_BUILD_DEFS_H 1


#ifndef ZEND_ACCONFIG_H_NO_C_PROTOS

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#ifdef HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif

#if ZEND_BROKEN_SPRINTF
int zend_sprintf(char *buffer, const char *format, ...);
#else
# define zend_sprintf sprintf
#endif

#include <math.h>

#ifndef zend_isnan
#ifdef HAVE_ISNAN
#define zend_isnan(a) isnan(a)
#elif defined(HAVE_FPCLASS)
#define zend_isnan(a) ((fpclass(a) == FP_SNAN) || (fpclass(a) == FP_QNAN))
#else
#define zend_isnan(a) 0
#endif
#endif

#ifdef HAVE_ISINF
#define zend_isinf(a) isinf(a)
#elif defined(INFINITY)
/* Might not work, but is required by ISO C99 */
#define zend_isinf(a) (((a)==INFINITY)?1:0)
#elif defined(HAVE_FPCLASS)
#define zend_isinf(a) ((fpclass(a) == FP_PINF) || (fpclass(a) == FP_NINF))
#else
#define zend_isinf(a) 0
#endif

#ifdef HAVE_FINITE
#define zend_finite(a) finite(a)
#elif defined(HAVE_ISFINITE) || defined(isfinite)
#define zend_finite(a) isfinite(a)
#elif defined(fpclassify)
#define zend_finite(a) ((fpclassify((a))!=FP_INFINITE&&fpclassify((a))!=FP_NAN)?1:0)
#else
#define zend_finite(a) (zend_isnan(a) ? 0 : zend_isinf(a) ? 0 : 1)
#endif

#endif /* ifndef ZEND_ACCONFIG_H_NO_C_PROTOS */

#ifdef NETWARE 
#ifdef USE_WINSOCK
#/*This detection against winsock is of no use*/ undef HAVE_SOCKLEN_T 
#/*This detection against winsock is of no use*/ undef HAVE_SYS_SOCKET_H
#endif
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
/* #undef PTHREADS */
