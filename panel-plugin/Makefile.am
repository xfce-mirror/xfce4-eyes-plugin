plugindir = $(libexecdir)/xfce4/panel-plugins
plugin_PROGRAMS = xfce4-eyes-plugin

xfce4_eyes_plugin_SOURCES =						\
	eyes.h								\
	eyes.c								\
	themes.h							\
	themes.c

xfce4_eyes_plugin_CFLAGS =						\
	-I$(top_srcdir)							\
	$(LIBXFCEGUI4_CFLAGS)						\
	$(LIBXFCE4PANEL_CFLAGS)						\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"				\
	-DTHEMESDIR=\"$(datadir)/xfce4/eyes/themes\"

xfce4_eyes_plugin_LDADD =						\
	$(LIBXFCE4PANEL_LIBS)						\
	$(LIBXFCEGUI4_LIBS)

desktopdir = $(datadir)/xfce4/panel-plugins
desktop_in_in_files = eyes.desktop.in.in
desktop_in_files = $(desktop_in_in_files:.desktop.in.in=.desktop.in)
%.desktop.in: %.desktop.in.in
	sed -e "s,\@libexecdir\@,$(libexecdir),g" < $< > $@
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST =								\
	$(desktop_in_in_files)

CLEANFILES =								\
	$(desktop_in_files)						\
	$(desktop_DATA)

