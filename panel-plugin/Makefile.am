AM_CPPFLAGS = 										\
	-I$(top_srcdir)									\
	-DPACKAGE_LOCALE_DIR=\"$(localedir)\"			\
	-DTHEMESDIR=\"$(datadir)/xfce4/eyes/themes\"

plugindir = $(libdir)/xfce4/panel/plugins
plugin_LTLIBRARIES = libeyes.la

libeyes_la_SOURCES =					\
	eyes.h								\
	eyes.c								\
	themes.h							\
	themes.c

libeyes_la_CFLAGS =						\
	$(LIBXFCE4UI_CFLAGS)				\
	$(LIBXFCE4PANEL_CFLAGS)

libeyes_la_LIBADD =						\
	$(LIBXFCE4PANEL_LIBS)				\
	$(LIBXFCE4UI_LIBS)

libeyes_la_LDFLAGS =					\
	-avoid-version 						\
	-module 							\
	-no-undefined						\
	-export-symbols-regex '^xfce_panel_module_(preinit|init|construct)' \
	$(PLATFORM_LDFLAGS)

desktopdir = $(datadir)/xfce4/panel/plugins
desktop_in_files = eyes.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

%.desktop: %.desktop.in
	$(AM_V_GEN) $(MSGFMT) --desktop --template $< -d $(top_srcdir)/po -o $@

EXTRA_DIST =							\
	$(desktop_in_files)					\
	meson.build							\
	$(NULL)

CLEANFILES = $(desktop_DATA)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
