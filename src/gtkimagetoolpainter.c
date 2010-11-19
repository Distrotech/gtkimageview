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
#include <math.h>
#include "gtkimagetoolpainter.h"

/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/
static void
gtk_image_tool_painter_paint (GtkImageToolPainter *painter,
                              GdkRectangle        *rect)
{
    GtkImageView *view = painter->view;
    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (view);
    guchar *pixels = gdk_pixbuf_get_pixels (pixbuf);
    int stride = gdk_pixbuf_get_rowstride (pixbuf);
    int n_chans = gdk_pixbuf_get_n_channels (pixbuf);

    for (int y = rect->y; y < rect->y + rect->height; y++)
        for (int x = rect->x; x < rect->x + rect->width; x++)
        {
            int ofs = y * stride + x * n_chans;
            for (int n = 0; n < 3; n++)
                pixels[ofs + n] = 0x00;
            if (n_chans == 4)
                pixels[ofs + 3] = 0xff;
        }
}

static void
gtk_image_tool_painter_paint_at (GtkImageToolPainter *painter,
                                 int                  wx,
                                 int                  wy)
{
    GtkImageView *view = painter->view;

    GdkRectangle wid_rect = {wx, wy, 4, 4};
    GdkRectangle image_rect;
    if (!gtk_image_view_widget_to_image_rect (view, &wid_rect, &image_rect))
        return;

    // Clip it to the pixbufs area.
    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (view); 
    GdkRectangle pb_rect = {0, 0, 
                            gdk_pixbuf_get_width (pixbuf), 
                            gdk_pixbuf_get_height (pixbuf)};
    gdk_rectangle_intersect (&pb_rect, &image_rect, &image_rect);
    if (!image_rect.width || !image_rect.height)
        return;

    gtk_image_tool_painter_paint (painter, &image_rect);
    gtk_image_view_damage_pixels (view, &image_rect);
}

/*************************************************************/
/***** Implementation of the GtkIImageTool interface *********/
/*************************************************************/
static GdkCursor*
cursor_at_point (GtkIImageTool *tool,
                 int            x,
                 int            y)
{
    GtkImageToolPainter *painter = GTK_IMAGE_TOOL_PAINTER (tool);
    return painter->crosshair;
}

static gboolean
button_press (GtkIImageTool  *tool,
              GdkEventButton *ev)
{
    GtkImageToolPainter *painter = GTK_IMAGE_TOOL_PAINTER (tool);
    if (ev->button != 1)
        return FALSE;

    gtk_image_tool_painter_paint_at (painter, ev->x, ev->y);

    return mouse_handler_button_press (painter->mouse_handler, ev);
}

static gboolean
button_release (GtkIImageTool  *tool,
                GdkEventButton *ev)
{
    GtkImageToolPainter *painter = GTK_IMAGE_TOOL_PAINTER (tool);
    return mouse_handler_button_release (painter->mouse_handler, ev);
}

static gboolean
motion_notify (GtkIImageTool  *tool,
               GdkEventMotion *ev)
{
    GtkImageToolPainter *painter = GTK_IMAGE_TOOL_PAINTER (tool);
    mouse_handler_motion_notify (painter->mouse_handler, ev);
    if (!painter->mouse_handler->dragging)
        return FALSE;

    gtk_image_tool_painter_paint_at (painter, ev->x, ev->y);

    return FALSE;
}

static void
pixbuf_changed (GtkIImageTool *tool,
                gboolean       reset_fit,
                GdkRectangle  *rect)
{
    GtkImageToolPainter *painter = GTK_IMAGE_TOOL_PAINTER (tool);
    gdk_pixbuf_draw_cache_invalidate (painter->cache);
}

static void
paint_image (GtkIImageTool     *tool,
             GdkPixbufDrawOpts *opts,
             GdkDrawable       *drawable)
{
    GtkImageToolPainter *painter = GTK_IMAGE_TOOL_PAINTER (tool);
    gdk_pixbuf_draw_cache_draw (painter->cache, opts, drawable);
}

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
gtk_iimage_tool_interface_init (gpointer g_iface,
                                gpointer iface_data)
{
    GtkIImageToolClass *klass = (GtkIImageToolClass *) g_iface;
    klass->cursor_at_point = cursor_at_point;
    klass->button_press = button_press;
    klass->button_release = button_release;
    klass->motion_notify = motion_notify;
    klass->pixbuf_changed = pixbuf_changed;
    klass->paint_image = paint_image;
}

G_DEFINE_TYPE_EXTENDED (GtkImageToolPainter,
                        gtk_image_tool_painter,
                        G_TYPE_OBJECT,
                        0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_IIMAGE_TOOL,
                                               gtk_iimage_tool_interface_init));

static void
gtk_image_tool_painter_finalize (GObject *object)
{
    GtkImageToolPainter *painter = GTK_IMAGE_TOOL_PAINTER (object);
    gdk_pixbuf_draw_cache_free (painter->cache);
    gdk_cursor_unref (painter->crosshair);
    g_free (painter->mouse_handler);

    /* Chain up */
    G_OBJECT_CLASS (gtk_image_tool_painter_parent_class)->finalize (object);
}

static void
gtk_image_tool_painter_class_init (GtkImageToolPainterClass *klass)
{
    GObjectClass *object_class = (GObjectClass *) klass;
    object_class->finalize = gtk_image_tool_painter_finalize;
}

static void
gtk_image_tool_painter_init (GtkImageToolPainter *tool)
{
    tool->crosshair = gdk_cursor_new (GDK_CROSSHAIR);
    tool->cache = gdk_pixbuf_draw_cache_new ();
    tool->mouse_handler = mouse_handler_new (tool->crosshair);
}

GtkIImageTool*
gtk_image_tool_painter_new (GtkImageView *view)
{
    g_return_val_if_fail (view, NULL);
    GtkImageToolPainter *painter = (GtkImageToolPainter *)
        g_object_new (GTK_TYPE_IMAGE_TOOL_PAINTER, NULL);
    painter->view = view;
    return GTK_IIMAGE_TOOL (painter);
}
