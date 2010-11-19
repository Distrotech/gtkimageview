/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * Copyright © 2007 Björn Lindqvist <bjourne@gmail.com>
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
#ifndef __UTILS_H__
#define __UTILS_H__

#include <gdk/gdk.h>

typedef struct
{
    int width;
    int height;
} Size;

void          gdk_pixbuf_shade               (GdkPixbuf       *pixbuf,
                                              GdkRectangle    *rect);
void          gdk_pixbuf_scale_blend         (GdkPixbuf       *src,
                                              GdkPixbuf       *dst,
                                              int              dst_x,
                                              int              dst_y,
                                              int              dst_width,
                                              int              dst_height,
                                              gdouble          offset_x,
                                              gdouble          offset_y,
                                              gdouble          zoom,
                                              GdkInterpType    interp,
                                              int              check_x,
                                              int              check_y,
                                              int              check_size,
                                              int              color1,
                                              int              color2);
char         *gdk_rectangle_to_str           (GdkRectangle     rect);
gboolean      gdk_rectangle_eq               (GdkRectangle     r1,
                                              GdkRectangle     r2);
gboolean      gdk_rectangle_eq2              (GdkRectangle     rect,
                                              int              x,
                                              int              y,
                                              int              width,
                                              int              height);
gboolean      gdk_rectangle_contains         (GdkRectangle     rect,
                                              int              x,
                                              int              y);
void          gdk_window_draw_inset_frame    (GdkWindow       *window,
                                              GdkRectangle    *rect,
                                              GdkGC           *light_gc,
                                              GdkGC           *dark_gc);
void          gdk_draw_rect                  (GdkDrawable     *drawable,
                                              GdkGC           *gc,
                                              gboolean         filled,
                                              GdkRectangle    *rect);
void          gdk_rectangle_get_rects_around (GdkRectangle    *outer,
                                              GdkRectangle    *inner,
                                              GdkRectangle     around[4]);

#endif
