/* Copyright (C) 1999 Dave Camp <dave@davec.dhs.org>
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

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <ctype.h>

#include <gtk/gtk.h>

#include "eyes.h"
#include "themes.h"

gchar *theme_directories[] = {
    THEMESDIR
};
#define NUM_THEME_DIRECTORIES 1

static void
parse_theme_file (EyesPlugin *eyes, FILE *theme_file)
{
    gchar line_buf [512]; /* prolly overkill */
    gchar *token;
    fgets (line_buf, 512, theme_file);
    while (!feof (theme_file)) {
        token = strtok (line_buf, "=");
        if (strncmp (token, "wall-thickness",
                 strlen ("wall-thickness")) == 0)
        {
            token += strlen ("wall-thickness");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->wall_thickness);
        }
        else if (strncmp (token, "num-eyes", strlen ("num-eyes")) == 0)
        {
            token += strlen ("num-eyes");
            while (!isdigit (*token))
            {
                token++;
            }
            sscanf (token, "%d", &eyes->num_eyes);
        }
        else if (strncmp (token, "eye-pixmap", strlen ("eye-pixmap")) == 0)
        {
            token = strtok (NULL, "\"");
            token = strtok (NULL, "\"");
            if (eyes->eye_filename != NULL)
                g_free (eyes->eye_filename);
            eyes->eye_filename = g_strdup_printf ("%s%s",
                                    eyes->theme_dir,
                                    token);
        }
        else if (strncmp (token, "pupil-pixmap", strlen ("pupil-pixmap")) == 0)
        {
            token = strtok (NULL, "\"");
            token = strtok (NULL, "\"");
			if (eyes->pupil_filename != NULL)
				g_free (eyes->pupil_filename);
			eyes->pupil_filename
				= g_strdup_printf ("%s%s",
						   eyes->theme_dir,
						   token);
        }
        fgets (line_buf, 512, theme_file);
    }
}



void
load_theme (EyesPlugin  *eyes,
            const gchar *theme_dir)
{
	FILE* theme_file;
    gchar *file_name;

    eyes->theme_dir = g_strdup_printf ("%s/", theme_dir);

    file_name = g_strdup_printf("%s%s",theme_dir,"/config");
    theme_file = fopen (file_name, "r");
    if (theme_file == NULL) {
        g_error ("Unable to open theme file.");
    }

    parse_theme_file (eyes, theme_file);
    fclose (theme_file);

    eyes->theme_name = g_strdup (theme_dir);

    if (eyes->eye_image)
    	g_object_unref (eyes->eye_image);

    eyes->eye_image = gdk_pixbuf_new_from_file (eyes->eye_filename, NULL);

    if (eyes->pupil_image)
    	g_object_unref (eyes->pupil_image);

    eyes->pupil_image = gdk_pixbuf_new_from_file (eyes->pupil_filename, NULL);

	eyes->eye_height = gdk_pixbuf_get_height (eyes->eye_image);
    eyes->eye_width = gdk_pixbuf_get_width (eyes->eye_image);
    eyes->pupil_height = gdk_pixbuf_get_height (eyes->pupil_image);
    eyes->pupil_width = gdk_pixbuf_get_width (eyes->pupil_image);

    g_free (file_name);

}
