/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * Copyright © 2007-2008 Björn Lindqvist <bjourne@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#include <src/gtkimagescrollwin.h>
#include <src/gtkimagetoolcairo.h>
#include <src/gtkiimagecairoshaper.h>

/*************************************************************/
/***** LineShaper class **************************************/
/*************************************************************/
#define TYPE_LINE_SHAPER                        (line_shaper_get_type ())
#define LINE_SHAPER(obj)                        (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_LINE_SHAPER, LineShaper))
#define LINE_SHAPER_CLASS(klass)                (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_LINE_SHAPER, LineShaperClass))

typedef struct _LineShaper LineShaper;
typedef struct _LineShaperClass LineShaperClass;

struct _LineShaper
{
    GObject parent;
    int     base_x;
    int     base_y;
    int     ofs_x;
    int     ofs_y;
};

struct _LineShaperClass
{
    GObjectClass parent;
};

GType line_shaper_get_type (void);

static int LINE_WIDTH = 5;

/*************************************************************/
/***** The interface methods *********************************/
/*************************************************************/
static void
start (GtkIImageCairoShaper *shaper,
       int                   x,
       int                   y)
{
    LineShaper *line = LINE_SHAPER (shaper);
    line->base_x = x;
    line->base_y = y;
    line->ofs_x = x;
    line->ofs_y = y;
}

static void
motion (GtkIImageCairoShaper *shaper,
        int                   x,
        int                   y)
{
    LineShaper *line = LINE_SHAPER (shaper);
    line->ofs_x = x;
    line->ofs_y = y;
}

static void
reset (GtkIImageCairoShaper *shaper)
{
    start (shaper, -1, -1);
}

static void
get_damage_area (GtkIImageCairoShaper *shaper,
                 GdkRectangle         *damage_area)
{
    LineShaper *line = LINE_SHAPER (shaper);
    int x1 = MIN (line->base_x, line->ofs_x);
    int y1 = MIN (line->base_y, line->ofs_y);
    int x2 = MAX (line->base_x, line->ofs_x);
    int y2 = MAX (line->base_y, line->ofs_y);
    damage_area->x = x1 - LINE_WIDTH;
    damage_area->y = y1 - LINE_WIDTH;
    damage_area->width = x2 - x1 + 2 * LINE_WIDTH;
    damage_area->height = y2 - y1 + 2 * LINE_WIDTH;
}

static void
draw (GtkIImageCairoShaper *shaper,
      cairo_t              *cr)
{
    LineShaper *line = LINE_SHAPER (shaper);
    // If no movement has been recorded.
    if (line->base_x == -1)
        return;
    
    cairo_set_line_width (cr, LINE_WIDTH);
    cairo_set_source_rgba (cr, 0, 0, 1, 0.4);
    cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
    cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);

    cairo_move_to (cr, line->base_x, line->base_y);
    cairo_line_to (cr, line->ofs_x, line->ofs_y);
    cairo_stroke (cr);
}


/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
gtk_iimage_cairo_shaper_interface_init (gpointer g_iface,
                                        gpointer iface_data)
{
    GtkIImageCairoShaperClass *klass = (GtkIImageCairoShaperClass *) g_iface;
    klass->start = start;
    klass->motion = motion;
    klass->reset = reset;
    klass->get_damage_area = get_damage_area;
    klass->draw = draw;
}

G_DEFINE_TYPE_EXTENDED (LineShaper,
                        line_shaper,
                        G_TYPE_OBJECT,
                        0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_IIMAGE_CAIRO_SHAPER,
                                               gtk_iimage_cairo_shaper_interface_init));

static void
line_shaper_class_init (LineShaperClass *klass)
{
}

static void
line_shaper_init (LineShaper *line)
{
    start (GTK_IIMAGE_CAIRO_SHAPER (line), -1, -1);
}

static GtkIImageCairoShaper*
line_shaper_new ()
{
    gpointer data = g_object_new (TYPE_LINE_SHAPER, NULL);
    return GTK_IIMAGE_CAIRO_SHAPER (data);
}

int
main (int argc, char *argv[])
{
    printf ("This program demonstrates how to draw stuff using cairo\n"
            "on the pixbuf GtkImageView shows.\n");

    char **filenames = NULL;
    GOptionEntry options[] = {
        {
            G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY,
            &filenames, NULL, "[FILE...]"
        },
        {NULL}
    };
    GOptionContext *ctx = g_option_context_new ("ex-abssize");
    g_option_context_add_main_entries (ctx, options, "moo");
    g_option_context_parse (ctx, &argc, &argv, NULL);
    g_option_context_free (ctx);
    
    gtk_init (&argc, &argv);
    if (!filenames)
    {
        printf ("Supply name of image.\n");
        return 1;
    }

    GtkWidget *view = gtk_image_view_new ();

    gtk_image_view_set_interpolation (GTK_IMAGE_VIEW (view),
                                      GDK_INTERP_NEAREST);
    
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (filenames[0], NULL);
    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, TRUE);
    GtkIImageCairoShaper *shaper = line_shaper_new ();
    GtkIImageTool *cairo =
        gtk_image_tool_cairo_new (GTK_IMAGE_VIEW (view), shaper);
    gtk_image_view_set_tool (GTK_IMAGE_VIEW (view), cairo);

    GtkWidget *scroll_win = gtk_image_scroll_win_new (GTK_IMAGE_VIEW(view));

    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), 500, 300);
    gtk_container_add (GTK_CONTAINER (window), scroll_win);
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (gtk_main_quit), NULL);

    gtk_widget_show_all (window);
    gtk_main ();
}




