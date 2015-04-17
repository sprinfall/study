#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([Noodle-Engine], [0.1.0], [dennis.wangkk@gmail.com])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([boost/asio.hpp])
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h memory.h netdb.h netinet/in.h stdlib.h string.h sys/socket.h sys/time.h sys/timeb.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([atexit dup2 ftime gethostbyname gettimeofday inet_ntoa memchr memset select socket strchr strerror strspn strstr strtol])

AC_CONFIG_FILES([Makefile console/Makefile src/Makefile plugins/Makefile plugins/plugin_fastcgi/Makefile test/Makefile test/test_fastcgi_plugin/Makefile] test/test_proxy_stub/Makefile plugins/plugin_console/Makefile plugins/plugin_proxy/Makefile plugins/plugin_stub/Makefile)
AC_OUTPUT
