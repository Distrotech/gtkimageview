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
#ifndef __GTK_IMAGE_NAV_H__
#define __GTK_IMAGE_NAV_H__
/**
 * #GtkImageNav is a popup window that shows a map of a
 * #GtkImageView. The user can then zoom around in the area.
 **/

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "gtkimageview.h"

G_BEGIN_DECLS

#define GTK_TYPE_IMAGE_NAV                (gtk_image_nav_get_type ())
#define GTK_IMAGE_NAV(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IMAGE_NAV, GtkImageNav))
#define GTK_IMAGE_NAV_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IMAGE_NAV, GtkImageNavClass))
#define GTK_IS_IMAGE_NAV(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IMAGE_NAV))
#define GTK_IS_IMAGE_NAV_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IMAGE_NAV))
#define GTK_IMAGE_NAV_GET_CLASS(obj)      (G_TYPE_CHECK_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IMAGE_NAV, GtkImageNavClass))

/**
 * GTK_IMAGE_NAV_MAX_WIDTH:
 *
 * The maximum width of the drawing area in the widget.
 **/
#define GTK_IMAGE_NAV_MAX_WIDTH			  192
/**
 * GTK_IMAGE_NAV_MAX_HEIGHT:
 *
 * The maximum height of the drawing area in the widget.
 **/
#define GTK_IMAGE_NAV_MAX_HEIGHT          128

typedef struct _GtkImageNav GtkImageNav;
typedef struct _GtkImageNavClass GtkImageNavClass;

struct _GtkImageNav
{
    GtkWindow       parent;

    /* A GtkDrawingArea to draw the image in. */
    GtkWidget      *preview;

    /* The GtkImageView that is navigated. */
    GtkImageView   *view;

    /* A downsampled version of the GtkImageView's pixbuf to display. */
    GdkPixbuf      *pixbuf;

    /* The last drawn XOR rectangle. */
    GdkRectangle    last_rect;

    /* Center coordinate of where GtkImageNav is mapped. */
    int             center_x;
    int             center_y;

    /* To draw the preview square. */
    GdkGC          *gc;

    /* A flag indicating wheter the pixbuf needs to be recreated when
       the navigator is shown. */
    gboolean        update_when_shown;
};

struct _GtkImageNavClass
{
    GtkWindowClass parent_class;
};

GType         gtk_image_nav_get_type         (void) G_GNUC_CONST;

/* Constructors */
GtkWidget    *gtk_image_nav_new              (GtkImageView *view);

/* Read-only properties */
GdkPixbuf    *gtk_image_nav_get_pixbuf       (GtkImageNav *nav);

/* Actions */
void          gtk_image_nav_grab             (GtkImageNav *nav);
void          gtk_image_nav_release          (GtkImageNav *nav);

/* Runner function */
void          gtk_image_nav_show_and_grab    (GtkImageNav *nav,
											  int          center_x,
											  int          center_y);

G_END_DECLS


#endif
