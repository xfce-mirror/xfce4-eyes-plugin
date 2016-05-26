/* Copyright (c) Benedikt Meurer <benedikt.meurer@unix-ag.uni-siegen.de>
 * Copyright (c) Danny Milosavljevic <danny_milo@gmx.net>
 * Copyright (c) Dave Camp <dave@davec.dhs.org>
 * Copyright (c) Davyd Madeley <davyd@madeley.id.au>
 * Copyright (c) Nick Schermer <nick@xfce.org>
 * Copyright (c) Harald Judt <h.judt@gmx.at>
 * Copyright (c) 2016 Andre Miranda <andre42m@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_MATH_H
#include <math.h>
#endif

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include <libxfce4util/libxfce4util.h>
#include <libxfce4ui/libxfce4ui.h>

#include "eyes.h"
#include "themes.h"

/* for xml: */
#define EYES_ROOT      "Eyes"
#define DEFAULTTHEME   "Tango"
#define UPDATE_TIMEOUT 50


/***************************
 *** Function Prototypes ***
 ***************************/
static void eyes_write_rc_file (XfcePanelPlugin *plugin,
                                EyesPlugin      *eyes);

static gboolean eyes_set_size (XfcePanelPlugin *plugin,
                               gint             size,
                               EyesPlugin      *eyes);


/*****************************
 *** Eyes Plugin Functions ***
 *****************************/
static void
calculate_pupil_xy (EyesPlugin *eyes_applet,
                    gint x,
                    gint y,
                    gint *pupil_x,
                    gint *pupil_y,
                    GtkWidget *widget)
{
	double nx, ny;
	double distance;
	double angle, angle_z;
	double radius_x, radius_y;

	GtkAllocation allocation;
	GtkAlign halign, valign;
	gfloat xalign = 0, yalign = 0;
	gint width, height;

	gtk_widget_get_allocation (GTK_WIDGET(widget), &allocation);

	width = allocation.width;
	height = allocation.height;
	halign = gtk_widget_get_halign (GTK_WIDGET(widget));
	valign = gtk_widget_get_valign (GTK_WIDGET(widget));

	if (halign == GTK_ALIGN_CENTER) xalign = 0.5;
	else if (halign == GTK_ALIGN_END) xalign = 1;

	if (valign == GTK_ALIGN_CENTER) yalign = 0.5;
	else if (valign == GTK_ALIGN_END) yalign = 1;

    /* calculate x,y pointer offsets wrt to eye center */
	nx = x - MAX(width - eyes_applet->eye_width, 0) * xalign
		- eyes_applet->eye_width / 2 - allocation.x;
	ny = y - MAX(height - eyes_applet->eye_height, 0) * yalign
		- eyes_applet->eye_height / 2 - allocation.y;

	/* calculate eye sizes */
	radius_x = (eyes_applet->eye_width -
				eyes_applet->wall_thickness -
				eyes_applet->pupil_width) / 2.0;
	radius_y = (eyes_applet->eye_height -
				eyes_applet->wall_thickness -
				eyes_applet->pupil_height) / 2.0;

	/* by default assume the z-axis distance of 3 radius_x */
	distance = 3 * radius_x;

	/* correction factor for aspect ratio of the eye */
	if (radius_y != 0)
		ny *= radius_x / radius_y;

	/* calculate 3D rotation angles */
	angle_z = atan2 (ny, nx);
	angle = atan2 (hypot (nx, ny), distance);

	/* perform 3D rotation of a vector [0,0,1] (z=1) */
	*pupil_x = radius_x * sin (angle) * cos (angle_z) + eyes_applet->eye_width / 2;
	*pupil_y = radius_y * sin (angle) * sin (angle_z) + eyes_applet->eye_height / 2;
}



static void
draw_eye (EyesPlugin *eyes,
          gint    eye_num,
          gint    pupil_x,
          gint    pupil_y)
{
    GdkPixbuf *pixbuf;
    GdkRectangle rect, r1, r2;

    pixbuf = gdk_pixbuf_copy (eyes->eye_image);
    r1.x = pupil_x - eyes->pupil_width / 2;
    r1.y = pupil_y - eyes->pupil_height / 2;
    r1.width = eyes->pupil_width;
    r1.height = eyes->pupil_height;
    r2.x = 0;
    r2.y = 0;
    r2.width = eyes->eye_width;
    r2.height = eyes->eye_height;
    gdk_rectangle_intersect (&r1, &r2, &rect);
    gdk_pixbuf_composite (eyes->pupil_image, pixbuf,
                       rect.x,
                       rect.y,
                       rect.width,
                             rect.height,
                             pupil_x - eyes->pupil_width / 2,
                       pupil_y - eyes->pupil_height / 2, 1.0, 1.0,
                             GDK_INTERP_BILINEAR,
                           255);
    gtk_image_set_from_pixbuf (GTK_IMAGE (eyes->eyes[eye_num]),
                          pixbuf);
    g_object_unref (G_OBJECT (pixbuf));
}



static gint
timer_cb(EyesPlugin *eyes)
{
    gint x, y;
    gint pupil_x, pupil_y;
    gint i;
    GdkWindow *window;
    GdkDevice *mouse_device;

#if GTK_CHECK_VERSION(3,2,0)
    GdkSeat *seat = gdk_display_get_default_seat (gdk_display_get_default());
    mouse_device = gdk_seat_get_pointer (seat);
#else
    GdkDeviceManager *devman = gdk_display_get_device_manager (gdk_display_get_default());
    mouse_device = gdk_device_manager_get_client_pointer (devman);
#endif

    for (i = 0; i < eyes->num_eyes; i++)
    {
        if (gtk_widget_get_realized (eyes->eyes[i]))
        {
            window = gtk_widget_get_window (GTK_WIDGET (eyes->eyes[i]));
            gdk_window_get_device_position (window, mouse_device, &x, &y, NULL);

            if ((x != eyes->pointer_last_x[i]) || (y != eyes->pointer_last_y[i]))
            {
                calculate_pupil_xy (eyes, x, y, &pupil_x, &pupil_y, eyes->eyes[i]);
                draw_eye (eyes, i, pupil_x, pupil_y);

                eyes->pointer_last_x[i] = x;
                eyes->pointer_last_y[i] = y;
            }
            else
            {
                /* pointer position did not change since last poll, so
                   why would it be different for the remaining eyes? */
                break;
            }
        }
    }

    return TRUE;
}



static void
properties_load(EyesPlugin *eyes)
{
    gchar *path;

    if (eyes->active_theme)
        path = g_build_filename(THEMESDIR, eyes->active_theme, NULL);
    else
        path = g_build_filename(THEMESDIR, DEFAULTTHEME, NULL);

    load_theme(eyes, path);

    g_free(path);
}



static void
setup_eyes(EyesPlugin *eyes)
{
    int i;

    if (eyes->hbox != NULL)
    {
        gtk_widget_destroy(eyes->hbox);
        eyes->hbox = NULL;
    }

    eyes->hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(eyes->align), GTK_WIDGET(eyes->hbox));

    eyes->eyes = g_new0 (GtkWidget *, eyes->num_eyes);
	eyes->pointer_last_x = g_new0 (gint, eyes->num_eyes);
	eyes->pointer_last_y = g_new0 (gint, eyes->num_eyes);

    for (i = 0; i < eyes->num_eyes; i++)
    {
        eyes->eyes[i] = gtk_image_new ();

        gtk_widget_set_size_request(GTK_WIDGET(eyes->eyes[i]),
                                    eyes->eye_width,
                                    eyes->eye_height);

        gtk_widget_show(eyes->eyes[i]);

        gtk_box_pack_start(GTK_BOX(eyes->hbox), eyes->eyes[i],
                           FALSE, FALSE, 0);

		if ((eyes->num_eyes != 1) && (i == 0))
		{
			gtk_widget_set_halign (GTK_WIDGET (eyes->eyes[i]), GTK_ALIGN_END);
			gtk_widget_set_valign (GTK_WIDGET (eyes->eyes[i]), GTK_ALIGN_CENTER);
		}
		else if ((eyes->num_eyes != 1) && (i == eyes->num_eyes - 1))
		{
			gtk_widget_set_halign (GTK_WIDGET (eyes->eyes[i]), GTK_ALIGN_START);
			gtk_widget_set_valign (GTK_WIDGET (eyes->eyes[i]), GTK_ALIGN_CENTER);
		}
		else
		{
			gtk_widget_set_halign (GTK_WIDGET (eyes->eyes[i]), GTK_ALIGN_CENTER);
			gtk_widget_set_valign (GTK_WIDGET (eyes->eyes[i]), GTK_ALIGN_CENTER);
		}

		eyes->pointer_last_x[i] = G_MAXINT;
		eyes->pointer_last_y[i] = G_MAXINT;

		draw_eye (eyes, i,
			      eyes->eye_width / 2,
                  eyes->eye_height / 2);
    }

    gtk_widget_show(eyes->hbox);
}



static gboolean
eyes_applet_fill(EyesPlugin *eyes)
{
    gtk_widget_show_all(GTK_WIDGET(eyes->align));

    if (eyes->timeout_id == 0)
    {
        eyes->timeout_id = g_timeout_add (UPDATE_TIMEOUT,
                                          (GSourceFunc)timer_cb, eyes);
    }

    return TRUE;
}



/*************************
 *** Properties Dialog ***
 *************************/
static void
eyes_properties_dialog_response (GtkWidget  *dlg,
                                 gint        response,
                                 EyesPlugin *eyes)
{
    xfce_panel_plugin_unblock_menu (eyes->plugin);

    eyes_write_rc_file (eyes->plugin, eyes);

    gtk_widget_destroy (dlg);
}



static void
combobox_changed (GtkComboBox    *combobox,
                  EyesPlugin     *eyes)
{
	gchar *selected = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (combobox));

	if (eyes->active_theme)
		g_free (eyes->active_theme);

	eyes->active_theme = g_strdup (selected);
	g_free (selected);

	properties_load(eyes);
    setup_eyes(eyes);
    eyes_applet_fill(eyes);

    eyes_set_size(eyes->plugin, xfce_panel_plugin_get_size(eyes->plugin),
                  eyes);
}



static void
check_single_row_toggled (GtkWidget  *check,
                          EyesPlugin *eyes)
{
    eyes->single_row = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));
    eyes_set_size(eyes->plugin, xfce_panel_plugin_get_size(eyes->plugin),
                  eyes);
}



static void
eyes_properties_dialog (XfcePanelPlugin *plugin,
                        EyesPlugin      *eyes)
{
	GtkWidget   *dlg, *hbox, *label, *combobox, *check, *area;
	GDir        *dir;
	gint         i;
	gchar       *current;
	const gchar *entry;

	xfce_panel_plugin_block_menu (plugin);

	dlg = xfce_titled_dialog_new_with_buttons (_("Eyes"),
                                                  GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plugin))),
                                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  "gtk-close", GTK_RESPONSE_OK,
                                                  NULL);

    gtk_window_set_position   (GTK_WINDOW (dlg), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_name  (GTK_WINDOW (dlg), "xfce4-settings");

    g_signal_connect (dlg, "response", G_CALLBACK (eyes_properties_dialog_response),
                      eyes);

	area = gtk_dialog_get_content_area (GTK_DIALOG (dlg));
	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	gtk_box_pack_start (GTK_BOX (area), hbox, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 6);

	label = gtk_label_new_with_mnemonic (_("_Select a theme:"));
	gtk_box_pack_start (GTK_BOX(hbox), label, FALSE, FALSE, 0);

	combobox = gtk_combo_box_text_new ();
    gtk_box_pack_start (GTK_BOX (hbox), combobox, FALSE, TRUE, 0);

    if (eyes->active_theme)
		current = g_strdup (eyes->active_theme);
	else
		current = g_strdup (DEFAULTTHEME);

    if ((dir = g_dir_open(THEMESDIR, 0, NULL)) == NULL)
    {
        gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox), current);
    }
    else
    {
        for (i = 0; (entry = g_dir_read_name(dir)) != NULL; i++)
        {
            gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox), entry);

            if (strcmp (entry, current) == 0)
				gtk_combo_box_set_active (GTK_COMBO_BOX(combobox), i);
		}

        g_dir_close(dir);
	}

	g_free (current);

	gtk_label_set_mnemonic_widget (GTK_LABEL (label), combobox);

    g_signal_connect(G_OBJECT(combobox), "changed",
            G_CALLBACK(combobox_changed), eyes);

    check = gtk_check_button_new_with_mnemonic
        (_("Use single _row in multi-row panel"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), eyes->single_row);
	gtk_box_pack_start (GTK_BOX (area), check, FALSE, FALSE, 0);
    g_signal_connect(check, "toggled", G_CALLBACK(check_single_row_toggled), eyes);

    gtk_widget_show_all (dlg);
}



/******************************
 *** Panel Plugin Functions ***
 ******************************/
static void
eyes_free_data(XfcePanelPlugin *plugin,
               EyesPlugin      *eyes)
{
    g_return_if_fail(plugin != NULL);
    g_return_if_fail(eyes != NULL);

    if (eyes->timeout_id != 0)
        g_source_remove (eyes->timeout_id);

    g_free (eyes->eyes);
	g_free (eyes->pointer_last_x);
	g_free (eyes->pointer_last_y);

	if (eyes->active_theme != NULL)
		g_free (eyes->active_theme);

    if (eyes->eye_image != NULL)
        g_object_unref (G_OBJECT (eyes->eye_image));

    if (eyes->pupil_image != NULL)
        g_object_unref (G_OBJECT (eyes->pupil_image));

    if (eyes->theme_dir != NULL)
        g_free(eyes->theme_dir);

    if (eyes->theme_name != NULL)
        g_free(eyes->theme_name);

    if (eyes->eye_filename != NULL)
        g_free(eyes->eye_filename);

    if (eyes->pupil_filename != NULL)
        g_free(eyes->pupil_filename);

    gtk_widget_destroy(eyes->align);
    g_free(eyes);
}



static gboolean
eyes_set_size (XfcePanelPlugin *plugin,
               gint             size,
               EyesPlugin      *eyes)
{
    xfce_panel_plugin_set_small (plugin, eyes->single_row);
    gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, -1);

    return TRUE;
}


static gboolean
eyes_mode_changed (XfcePanelPlugin     *plugin,
                   XfcePanelPluginMode  mode,
                   EyesPlugin          *eyes)
{
    if (mode == XFCE_PANEL_PLUGIN_MODE_VERTICAL ||
        mode == XFCE_PANEL_PLUGIN_MODE_DESKBAR)
    {
        gtk_widget_set_halign(GTK_WIDGET(eyes->align), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(eyes->align), GTK_ALIGN_CENTER);
        gtk_widget_set_hexpand(GTK_WIDGET(eyes->align), FALSE);
        gtk_widget_set_vexpand(GTK_WIDGET(eyes->align), TRUE);
    }
    else
    {
        gtk_widget_set_halign(GTK_WIDGET(eyes->align), GTK_ALIGN_CENTER);
        gtk_widget_set_valign(GTK_WIDGET(eyes->align), GTK_ALIGN_CENTER);
        gtk_widget_set_hexpand(GTK_WIDGET(eyes->align), TRUE);
        gtk_widget_set_vexpand(GTK_WIDGET(eyes->align), FALSE);
    }

    eyes_set_size (plugin, xfce_panel_plugin_get_size (plugin), eyes);

    return TRUE;
}



static void
eyes_read_rc_file (XfcePanelPlugin *plugin,
                   EyesPlugin      *eyes)
{
    XfceRc      *rc;
    gchar       *file;
    gchar const *tmp;

    if (eyes->active_theme != NULL)
    {
        g_free (eyes->active_theme);
        eyes->active_theme = NULL;
    }

    if ((file = xfce_panel_plugin_lookup_rc_file (plugin)) != NULL)
    {
        rc = xfce_rc_simple_open (file, TRUE);
        g_free (file);

        if (rc != NULL)
        {
            tmp = xfce_rc_read_entry (rc, "theme", DEFAULTTHEME);

            if (tmp != NULL)
                eyes->active_theme = g_strdup (tmp);

            eyes->single_row =
                xfce_rc_read_bool_entry (rc, "single_row", FALSE);

            xfce_rc_close (rc);
        }
    }

    if (eyes->active_theme == NULL)
		eyes->active_theme = g_strdup (DEFAULTTHEME);
}



static void
eyes_write_rc_file (XfcePanelPlugin *plugin,
                    EyesPlugin      *eyes)
{
    gchar  *file;
    XfceRc *rc;

    if (!(file = xfce_panel_plugin_save_location (plugin, TRUE)))
        return;

    rc = xfce_rc_simple_open (file, FALSE);
    g_free (file);

    if (!rc)
        return;

    if (eyes->active_theme != NULL)
        xfce_rc_write_entry (rc, "theme", eyes->active_theme);

    xfce_rc_write_bool_entry (rc, "single_row", eyes->single_row);

    xfce_rc_close (rc);
}



static EyesPlugin *
eyes_plugin_new (XfcePanelPlugin* plugin)
{
    EyesPlugin *eyes = g_new0(EyesPlugin, 1);

    eyes->plugin = plugin;

    eyes->ebox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (eyes->ebox), FALSE);
    gtk_widget_show(eyes->ebox);

    eyes->align = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
    gtk_widget_set_halign(GTK_WIDGET(eyes->align), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(eyes->align), GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(GTK_WIDGET(eyes->align), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(eyes->align), TRUE);

    gtk_container_add(GTK_CONTAINER(eyes->ebox), eyes->align);
    gtk_widget_show(eyes->align);

    eyes_read_rc_file (plugin, eyes);

    properties_load(eyes);
    setup_eyes(eyes);
    eyes_applet_fill(eyes);

    return eyes;
}



static void
eyes_construct (XfcePanelPlugin *plugin)
{
    EyesPlugin *eyes;

    xfce_textdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR, "UTF-8");

    eyes = eyes_plugin_new (plugin);

    g_signal_connect (plugin, "mode-changed",
              G_CALLBACK (eyes_mode_changed), eyes);

    g_signal_connect (plugin, "size-changed",
              G_CALLBACK (eyes_set_size), eyes);

    g_signal_connect (plugin, "free-data",
              G_CALLBACK (eyes_free_data), eyes);

    g_signal_connect (plugin, "save",
              G_CALLBACK (eyes_write_rc_file), eyes);

    xfce_panel_plugin_menu_show_configure (plugin);
    g_signal_connect (plugin, "configure-plugin",
              G_CALLBACK (eyes_properties_dialog), eyes);

    gtk_container_add (GTK_CONTAINER (plugin), eyes->ebox);

    xfce_panel_plugin_add_action_widget (plugin, eyes->ebox);
}

XFCE_PANEL_PLUGIN_REGISTER (eyes_construct)
