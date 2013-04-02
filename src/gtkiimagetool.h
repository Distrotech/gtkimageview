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
#ifndef __GTK_IIMAGE_TOOL_H__
#define __GTK_IIMAGE_TOOL_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include "gdkpixbufdrawcache.h"

G_BEGIN_DECLS

#define GTK_TYPE_IIMAGE_TOOL            (gtk_iimage_tool_get_type ())
#define GTK_IIMAGE_TOOL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IIMAGE_TOOL, GtkIImageTool))
#define GTK_IIMAGE_TOOL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IIMAGE_TOOL, GtkIImageToolClass))
#define GTK_IS_IIMAGE_TOOL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IIMAGE_TOOL))
#define GTK_IS_IIMAGE_TOOL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IIMAGE_TOOL))
#define GTK_IIMAGE_TOOL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTK_TYPE_IIMAGE_TOOL, GtkIImageToolClass))

typedef struct _GtkIImageTool GtkIImageTool;
typedef struct _GtkIImageToolClass GtkIImageToolClass;

struct _GtkIImageToolClass
{
    GTypeInterface parent;
    gboolean       (*button_press)           (GtkIImageTool  *tool,
                                              GdkEventButton *ev);
    gboolean       (*button_release)         (GtkIImageTool  *tool,
                                              GdkEventButton *ev);
    gboolean       (*motion_notify)          (GtkIImageTool  *tool,
                                              GdkEventMotion *ev);
    GdkCursor*     (*cursor_at_point)        (GtkIImageTool  *tool,
                                              int             x,
                                              int             y);
    void           (*pixbuf_changed)         (GtkIImageTool  *tool,
                                              gboolean        reset_fit,
                                              GdkRectangle   *rect);
    void           (*paint_image)            (GtkIImageTool  *tool,
                                              GdkPixbufDrawOpts *opts,
                                              GdkDrawable    *drawable);
};

GType         gtk_iimage_tool_get_type       (void) G_GNUC_CONST;


/* Pseudo signal handlers. */
gboolean      gtk_iimage_tool_button_press   (GtkIImageTool  *tool,
                                              GdkEventButton *ev);
gboolean      gtk_iimage_tool_button_release (GtkIImageTool  *tool,
                                              GdkEventButton *ev);
gboolean      gtk_iimage_tool_motion_notify  (GtkIImageTool  *tool,
                                              GdkEventMotion *ev);
void          gtk_iimage_tool_pixbuf_changed (GtkIImageTool  *tool,
                                              gboolean        reset_fit,
                                              GdkRectangle   *rect);
void          gtk_iimage_tool_paint_image    (GtkIImageTool  *tool,
                                              GdkPixbufDrawOpts *opts,
                                              GdkDrawable    *drawable);

/* Read-only properties. */
GdkCursor    *gtk_iimage_tool_cursor_at_point (GtkIImageTool *tool,
                                               int            x,
                                               int            y);

G_END_DECLS
#endif
