# Configure with program name, version number, and email
AC_INIT([drillwriter], [7], [vernonrjones@gmail.com])
# Autoconf prereq
AC_PREREQ([2.64])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
# Automake
AM_INIT_AUTOMAKE([1.11 -Wall no-define silent-rules])
AM_SILENT_RULES([yes])
# Use a C compiler
#AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AM_PATH_GTK_2_0([2.24.0],,AC_MSG_ERROR([Gtk+-2.24.0 or higher required.]))
AC_CHECK_HEADERS([math.h])
#AC_SUBST(GTK_CFLAGS)
#AC_SUBST(LDFLAGS)
#AC_SUBST(GTK_LIBS)
# Generate a makefile
AC_CONFIG_FILES([Makefile src/Makefile])
# Output information
AC_OUTPUT
