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
#ifndef __GTK_IMAGE_TOOL_CAIRO_H__
#define __GTK_IMAGE_TOOL_CAIRO_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "gtkiimagecairoshaper.h"
#include "gtkimageview.h"
#include "mouse_handler.h"

G_BEGIN_DECLS

#define GTK_TYPE_IMAGE_TOOL_CAIRO               (gtk_image_tool_cairo_get_type ())
#define GTK_IMAGE_TOOL_CAIRO(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IMAGE_TOOL_CAIRO, GtkImageToolCairo))
#define GTK_IMAGE_TOOL_CAIRO_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IMAGE_TOOL_CAIRO, GtkImageToolCairoClass))
#define GTK_IS_IMAGE_TOOL_CAIRO(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IMAGE_TOOL_CAIRO))
#define GTK_IS_IMAGE_TOOL_CAIRO_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IMAGE_TOOL_CAIRO))
#define GTK_IMAGE_TOOL_CAIRO_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IMAGE_TOOL_CAIRO, GtkImageToolCairoClass))

typedef struct _GtkImageToolCairo GtkImageToolCairo;
typedef struct _GtkImageToolCairoClass GtkImageToolCairoClass;

struct _GtkImageToolCairo
{
    GObject               parent;
    GtkImageView         *view;
    GdkPixbufDrawCache   *cache;
    GdkCursor            *crosshair;

    /* A surface that clients can draw on using cairo. */
    cairo_surface_t      *image_surface;

    /* Middle surface which stuff is drawn on before the draw
       operation is completed. */
    cairo_surface_t      *draw_surface;
    
    /* References the pixbuf the view is showing. It is compared in
       the pixbuf_changed handler to determine if the view shows a
       different pixbuf. */
    GdkPixbuf            *pixbuf;

    /* Whether the cairo tool is allowed to ignore pixbuf changes. */
    gboolean              overwritable_pixbuf;

    /* Cairo shaper instance that draws in response to mouse
       movements. */
    GtkIImageCairoShaper *shaper;

    /* Current image area to redraw. */
    GdkRectangle          redraw_area;

    MouseHandler         *mouse_handler;
};

struct _GtkImageToolCairoClass
{
    GObjectClass parent;
};

GType          gtk_image_tool_cairo_get_type    (void);

/* Constructors */
GtkIImageTool *gtk_image_tool_cairo_new         (GtkImageView         *view,
                                                 GtkIImageCairoShaper *shaper);

/* Read-only properties */
cairo_surface_t *gtk_image_tool_cairo_get_image_surface (GtkImageToolCairo *cairo);

/* Read-write properties */
gboolean       gtk_image_tool_cairo_get_overwritable_pixbuf (GtkImageToolCairo *cairo);
void           gtk_image_tool_cairo_set_overwritable_pixbuf (GtkImageToolCairo *cairo,
                                                               gboolean           overwritable_pixbuf);

GtkIImageCairoShaper *gtk_image_tool_cairo_get_cairo_shaper (GtkImageToolCairo *cairo);
void           gtk_image_tool_cairo_set_cairo_shaper (GtkImageToolCairo    *cairo,
                                                      GtkIImageCairoShaper *shaper);

/* Actions */
void           gtk_image_tool_cairo_damage_surface (GtkImageToolCairo *cairo,
                                                    GdkRectangle      *rect);

G_END_DECLS
#endif
