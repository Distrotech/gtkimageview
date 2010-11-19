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
#ifndef __GTK_IMAGE_SCROLL_WIN_H__
#define __GTK_IMAGE_SCROLL_WIN_H__
/**
 * GtkImageScrollWin is a class that implements a type of
 * GtkScrolledWindow which is more integrated with GtkImageView.
 **/

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "gtkimagenav.h"
#include "gtkimageview.h"

G_BEGIN_DECLS

#define GTK_TYPE_IMAGE_SCROLL_WIN               (gtk_image_scroll_win_get_type ())
#define GTK_IMAGE_SCROLL_WIN(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IMAGE_SCROLL_WIN, GtkImageScrollWin))
#define GTK_IMAGE_SCROLL_WIN_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IMAGE_SCROLL_WIN, GtkImageScrollWinClass))
#define GTK_IS_IMAGE_SCROLL_WIN(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IMAGE_SCROLL_WIN))
#define GTK_IS_IMAGE_SCROLL_WIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IMAGE_SCROLL_WIN))
#define GTK_IMAGE_SCROLL_WIN_GET_CLASS(obj)     (G_TYPE_CHECK_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IMAGE_SCROLL_WIN, GtkImageScrollWinClass))

typedef struct _GtkImageScrollWin GtkImageScrollWin;
typedef struct _GtkImageScrollWinClass GtkImageScrollWinClass;

struct _GtkImageScrollWin
{
    GtkTable     parent;

    GtkWidget   *hscroll;
    GtkWidget   *vscroll;
    GtkWidget   *nav_box;
    GtkWidget   *nav;

    /* The GtkImage that shows the nav_button icon. */
    GtkWidget   *nav_image;

    /* The normal and the highlighted nav_button pixbuf. */
    GdkPixbuf   *nav_button;
    GdkPixbuf   *nav_button_hc;
};

struct _GtkImageScrollWinClass
{
    GtkTableClass parent_class;
};

GType         gtk_image_scroll_win_get_type  (void) G_GNUC_CONST;

/* Constructors */
GtkWidget    *gtk_image_scroll_win_new       (GtkImageView    *view);

G_END_DECLS

#endif
