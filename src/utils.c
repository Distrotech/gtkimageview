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
#include "utils.h"

/**
 * gdk_pixbuf_shade:
 * @pixbuf: a #GdkPixbuf
 * @rect: a #GdkRectangle or %NULL.
 *
 * Make each pixel in the rectangle in the pixbuf half as bright, but
 * preserve its alpha. If @rect is %NULL, shade all pixels.
 **/
void
gdk_pixbuf_shade (GdkPixbuf    *pixbuf,
                  GdkRectangle *rect)
{
    GdkRectangle area;
    if (!rect)
    {
        area = (GdkRectangle){
            0, 0,
            gdk_pixbuf_get_width (pixbuf), gdk_pixbuf_get_height (pixbuf)
        };
    }
    else
        area = *rect;

    guchar *p = gdk_pixbuf_get_pixels (pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride (pixbuf);
    int n_chans = gdk_pixbuf_get_n_channels (pixbuf);
    for (int y = 0; y < area.height; y++)
    {
        int i = (area.y + y) * rowstride + area.x * n_chans;
        for (int x = 0; x < area.width; x++)
        {
            p[i] >>= 1;
            p[i + 1] >>= 1;
            p[i + 2] >>= 1;
            i += n_chans;
        }
    }
}

/**
 * gdk_pixbuf_scale_blend:
 *
 * A utility function that either scales or composites color depending
 * on the number of channels in the source image. The last four
 * parameters are only used in the composite color case.
 **/
void
gdk_pixbuf_scale_blend (GdkPixbuf    *src,
                        GdkPixbuf    *dst,
                        int           dst_x,
                        int           dst_y,
                        int           dst_width,
                        int           dst_height,
                        gdouble       offset_x,
                        gdouble       offset_y,
                        gdouble       zoom,
                        GdkInterpType interp,
                        int           check_x,
                        int           check_y,
                        int           check_size,
                        int           color1,
                        int           color2)
{
    if (gdk_pixbuf_get_has_alpha (src))
        gdk_pixbuf_composite_color (src, dst,
                                    dst_x, dst_y, dst_width, dst_height,
                                    offset_x, offset_y,
                                    zoom, zoom,
                                    interp,
                                    255,
                                    check_x, check_y,
                                    check_size,
                                    color1, color2);
    else
        gdk_pixbuf_scale (src, dst,
                          dst_x, dst_y, dst_width, dst_height,
                          offset_x, offset_y,
                          zoom, zoom,
                          interp);
}

/**
 * gdk_rectangle_to_str:
 * @rect: a #GdkRectangle
 * @returns: a static string representation of the rectangle.
 **/
char *
gdk_rectangle_to_str (GdkRectangle rect)
{
    static gchar buf[256];
    g_snprintf (buf, 256, "(%d, %d)-[%d, %d]",
                rect.x, rect.y, rect.width, rect.height);
    return buf;
}

/**
 * gdk_rectangle_eq:
 * @r1: a #GdkRectangle
 * @r2: a #GdkRectangle
 * @returns: %TRUE if all parts of @r1 equals @r2, %FALSE otherwise.
 **/
gboolean
gdk_rectangle_eq (GdkRectangle r1,
                  GdkRectangle r2)
{
    return r1.x == r2.x && r1.y == r2.y &&
        r1.width == r2.width && r1.height == r2.height;
}

/**
 * gdk_rectangle_eq2:
 * @rect: a #GdkRectangle
 * @x: x of second rectangle
 * @y: y of second rectangle
 * @width: width of second rectangle
 * @height: height of second rectangle
 * @returns: %TRUE if @rect equals the rectangle specified by @x, @y,
 *   @width and @height
 *
 * Utility function identical to gdk_rectangle_eq() but with the
 * second rectangle specified as four integers for convenience.
 **/
gboolean      
gdk_rectangle_eq2 (GdkRectangle     rect,
                   int              x,
                   int              y,
                   int              width,
                   int              height)
{
    return gdk_rectangle_eq (rect, (GdkRectangle){x, y, width, height});
}

/**
 * gdk_rectangle_contains:
 * @rect: a #GdkRectangle
 * @returns: %TRUE if the x,y-coordinate is contained in the
 *   rectangle, %FALSE otherwise. Border pixels are considered to be
 *   contained.
 **/
gboolean
gdk_rectangle_contains (GdkRectangle rect,
                        int          x,
                        int          y)
{
    if (x >= rect.x && x <= (rect.x + rect.width) &&
        y >= rect.y && y <= (rect.y + rect.height))
        return TRUE;
    return FALSE;
}

void
gdk_window_draw_inset_frame (GdkWindow    *window,
                             GdkRectangle *rect,
                             GdkGC        *light_gc,
                             GdkGC        *dark_gc)
{
    int frame_x1 = rect->x - 1;
    int frame_y1 = rect->y - 1;
    int frame_x2 = rect->x + rect->width;
    int frame_y2 = rect->y + rect->height;

    gdk_draw_line (window, light_gc, frame_x2, frame_y1, frame_x2, frame_y2);
    gdk_draw_line (window, light_gc, frame_x1, frame_y2, frame_x2, frame_y2);
    gdk_draw_line (window, dark_gc, frame_x1, frame_y1, frame_x1, frame_y2);
    gdk_draw_line (window, dark_gc, frame_x1, frame_y1, frame_x2, frame_y1);
    
}

/**
 * gdk_draw_rect:
 *
 * This function is a fixed version of gdk_draw_rectangle. The GDK
 * function is broken in that drawing a the rectangle (0,0)-[0,0] will
 * draw a pixel at position (0,0).
 **/
void
gdk_draw_rect (GdkDrawable    *drawable,
               GdkGC          *gc,
               gboolean        filled,
               GdkRectangle   *rect)
{
    if (rect->width <= 0 || rect->height <= 0)
        return;
    gdk_draw_rectangle (drawable, gc, filled,
                        rect->x, rect->y, rect->width - 1, rect->height - 1);
}

void
gdk_rectangle_get_rects_around (GdkRectangle *outer,
                                GdkRectangle *inner,
                                GdkRectangle  around[4])
{
    // Top
    around[0] = (GdkRectangle) {
        outer->x,
        outer->y,
        outer->width,
        inner->y - outer->y
    };
    // Left
    around[1] = (GdkRectangle) {
        outer->x,
        inner->y,
        inner->x - outer->x,
        inner->height
    };
    // Right
    around[2] = (GdkRectangle) {
        inner->x + inner->width,
        inner->y,
        (outer->x + outer->width) - (inner->x + inner->width),
        inner->height
    };
    // Bottom
    around[3] = (GdkRectangle) {
        outer->x,
        inner->y + inner->height,
        outer->width,
        (outer->y + outer->height) - (inner->y + inner->height)
    };
}

