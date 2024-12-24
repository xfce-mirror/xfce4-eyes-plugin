dnl xfce4-eyes-plugin - eyes that spy on you
dnl

dnl ***************************
dnl *** Version information ***
dnl ***************************
XDT_VERSION_INIT([4.6.1], [git])

dnl ***************************
dnl *** Initialize autoconf ***
dnl ***************************
AC_COPYRIGHT([Copyright (c) 2006-2024 The Xfce development team])
AC_INIT([xfce4-eyes-plugin], [xdt_version], [https://gitlab.xfce.org/panel-plugins/xfce4-eyes-plugin/], [xfce4-eyes-plugin])
AC_PREREQ([2.69])
AC_CONFIG_MACRO_DIRS([m4])
AC_CANONICAL_TARGET()
AC_REVISION([xdt_version_build])

dnl ***************************
dnl *** Initialize automake ***
dnl ***************************
AM_INIT_AUTOMAKE([1.9 dist-bzip2 tar-ustar no-dist-gzip foreign])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE()
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl ********************************
dnl *** Check for basic programs ***
dnl ********************************
AC_PROG_CC()
LT_PATH_LD()
AC_PROG_INSTALL()
LT_PREREQ([2.2.6])
LT_INIT([disable-static])
AM_PROG_CC_C_O()

dnl **********************************
dnl *** Check for standard headers ***
dnl **********************************
AC_CHECK_HEADERS([math.h])

dnl ******************************
dnl *** Check for math library ***
dnl ******************************
AC_CHECK_LIB([m], [sin])

dnl ******************************
dnl *** Check for i18n support ***
dnl ******************************
GETTEXT_PACKAGE="$PACKAGE"
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Name of default gettext domain])
AC_SUBST([GETTEXT_PACKAGE])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

dnl ***********************************
dnl *** Check for required packages ***
dnl ***********************************
XDT_CHECK_PACKAGE([GLIB], [glib-2.0], [2.50.0])
XDT_CHECK_PACKAGE([GTK], [gtk+-3.0], [3.22.0])
XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-2], [4.16.0])
XDT_CHECK_PACKAGE([LIBXFCE4PANEL], [libxfce4panel-2.0], [4.16.0])

dnl ***********************************
dnl *** Check for debugging support ***
dnl ***********************************
XDT_FEATURE_DEBUG([xdt_debug_default])

AC_CONFIG_FILES([
Makefile
po/Makefile.in
panel-plugin/Makefile
icons/Makefile
icons/48x48/Makefile
icons/36x36/Makefile
icons/32x32/Makefile
icons/24x24/Makefile
icons/22x22/Makefile
icons/16x16/Makefile
themes/Makefile
themes/Bizarre/Makefile
themes/Bloodshot/Makefile
themes/Default/Makefile
themes/Default-tiny/Makefile
themes/Horrid/Makefile
themes/Tango/Makefile
themes/Chibi/Makefile
])
AC_OUTPUT
