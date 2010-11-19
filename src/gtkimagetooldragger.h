/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * Copyright © 2007-2009 Björn Lindqvist <bjourne@gmail.com>
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
#ifndef __GTK_IMAGE_TOOL_DRAGGER_H__
#define __GTK_IMAGE_TOOL_DRAGGER_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include "gtkiimagetool.h"
#include "gtkimageview.h"
#include "mouse_handler.h"

G_BEGIN_DECLS

#define GTK_TYPE_IMAGE_TOOL_DRAGGER             (gtk_image_tool_dragger_get_type ())
#define GTK_IMAGE_TOOL_DRAGGER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IMAGE_TOOL_DRAGGER, GtkImageToolDragger))
#define GTK_IMAGE_TOOL_DRAGGER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IMAGE_TOOL_DRAGGER, GtkImageToolDraggerClass))
#define GTK_IS_IMAGE_TOOL_DRAGGER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IMAGE_TOOL_DRAGGER))
#define GTK_IS_IMAGE_TOOL_DRAGGER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IMAGE_TOOL_DRAGGER))
#define GTK_IMAGE_TOOL_DRAGGER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IMAGE_TOOL_DRAGGER, GtkImageToolDraggerClass))

typedef struct _GtkImageToolDragger GtkImageToolDragger;
typedef struct _GtkImageToolDraggerClass GtkImageToolDraggerClass;

struct _GtkImageToolDragger
{
    GObject             parent;
    GdkCursor          *open_hand;
    GdkCursor          *closed_hand;
    MouseHandler       *mouse_handler;
    GtkImageView       *view;
    GdkPixbufDrawCache *cache;
};

struct _GtkImageToolDraggerClass
{
    GObjectClass parent;
};

GType         gtk_image_tool_dragger_get_type     (void);

/* Constructors */
GtkIImageTool *gtk_image_tool_dragger_new   (GtkImageView *view);

G_END_DECLS
#endif
