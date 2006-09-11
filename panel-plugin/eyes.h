


#ifndef __EYES_H__
#define __EYES_H__

#include <gtk/gtk.h>
#include <libxfce4panel/xfce-panel-plugin.h>

typedef struct
{
	XfcePanelPlugin *plugin;

	GtkWidget	*ebox;

	/* Properties */
	gchar *active_theme;

	/* Plugin */
	GtkWidget   *align;
	GtkWidget   *hbox;
	GtkWidget   **eyes;
	guint        timeout_id;
	gint 	    *pointer_last_x;
	gint 	    *pointer_last_y;

	/* Theme */
	GdkPixbuf *eye_image;
	GdkPixbuf *pupil_image;
	gchar *theme_dir;
	gchar *theme_name;
	gchar *eye_filename;
	gchar *pupil_filename;
	gint num_eyes;
	gint eye_height;
	gint eye_width;
	gint pupil_height;
	gint pupil_width;
	gint wall_thickness;
}
EyesPlugin;

#endif /* __EYES_H__ */
