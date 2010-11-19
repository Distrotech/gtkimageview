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
#ifndef __GTK_IIMAGE_CAIRO_SHAPER_H__
#define __GTK_IIMAGE_CAIRO_SHAPER_H__

#include <gdk/gdk.h>

G_BEGIN_DECLS

#define GTK_TYPE_IIMAGE_CAIRO_SHAPER            (gtk_iimage_cairo_shaper_get_type ())
#define GTK_IIMAGE_CAIRO_SHAPER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IIMAGE_CAIRO_SHAPER, GtkIImageCairoShaper))
#define GTK_IIMAGE_CAIRO_SHAPER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IIMAGE_CAIRO_SHAPER, GtkIImageCairoShaperClass))
#define GTK_IS_IIMAGE_CAIRO_SHAPER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IIMAGE_CAIRO_SHAPER))
#define GTK_IIMAGE_CAIRO_SHAPER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTK_TYPE_IIMAGE_CAIRO_SHAPER, GtkIImageCairoShaperClass))

typedef struct _GtkIImageCairoShaper GtkIImageCairoShaper;
typedef struct _GtkIImageCairoShaperClass GtkIImageCairoShaperClass;

struct _GtkIImageCairoShaperClass
{
    GTypeInterface parent;
    void           (*start)                  (GtkIImageCairoShaper *shaper,
                                              int                   x,
                                              int                   y);
    void           (*motion)                 (GtkIImageCairoShaper *shaper,
                                              int                   x,
                                              int                   y);
    void           (*reset)                  (GtkIImageCairoShaper *shaper);
    void           (*get_damage_area)        (GtkIImageCairoShaper *shaper,
                                              GdkRectangle         *damage_area);
    void           (*draw)                   (GtkIImageCairoShaper *shaper,
                                              cairo_t              *cr);
};

GType         gtk_iimage_cairo_shaper_get_type  (void);

void          gtk_iimage_cairo_shaper_start     (GtkIImageCairoShaper *shaper,
                                                 int                   x,
                                                 int                   y);
void          gtk_iimage_cairo_shaper_motion    (GtkIImageCairoShaper *shaper,
                                                 int                   x,
                                                 int                   y);
void          gtk_iimage_cairo_shaper_reset     (GtkIImageCairoShaper *shaper);
void          gtk_iimage_cairo_shaper_get_damage_area (GtkIImageCairoShaper *shaper,
                                                       GdkRectangle         *damage_area); 
void          gtk_iimage_cairo_shaper_draw      (GtkIImageCairoShaper *shaper,
                                                 cairo_t              *cr);

G_END_DECLS
#endif
