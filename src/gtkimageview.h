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
#ifndef __GTK_IMAGE_VIEW_H__
#define __GTK_IMAGE_VIEW_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "gtkiimagetool.h"

G_BEGIN_DECLS

#define GTK_TYPE_IMAGE_VIEW             (gtk_image_view_get_type ())
#define GTK_IMAGE_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IMAGE_VIEW, GtkImageView))
#define GTK_IMAGE_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IMAGE_VIEW, GtkImageViewClass))
#define GTK_IS_IMAGE_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IMAGE_VIEW))
#define GTK_IS_IMAGE_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IMAGE_VIEW))
#define GTK_IMAGE_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IMAGE_VIEW, GtkImageViewClass))

typedef struct _GtkImageView GtkImageView;
typedef struct _GtkImageViewClass GtkImageViewClass;

/**
 * GtkImageTransp:
 *
 * This enum defines the valid transparency settings for how the image
 * view should draw transparent parts of alpha images. Their primary
 * use is as a value for the first parameter to the
 * gtk_image_view_set_transp() method.
 *
 * Their interpretation is as follows:
 *
 * <itemizedlist>
 *   <listitem>GTK_IMAGE_TRANSP_COLOR : Use a single color.</listitem>
 *   <listitem>GTK_IMAGE_TRANSP_BACKGROUND : Use the background color
 *   of the widget</listitem>
 *   <listitem>GTK_IMAGE_TRANSP_GRID : Use light and dark gray
 *   checkerboard pattern.</listitem>
 * </itemizedlist>
 **/
typedef enum
{
    GTK_IMAGE_TRANSP_COLOR = 0,
    GTK_IMAGE_TRANSP_BACKGROUND,
    GTK_IMAGE_TRANSP_GRID
} GtkImageTransp;

/**
 * GtkImageView:
 *
 * GtkImageView is the main class in the library. All of its fields
 * are private, they are only shown here for completeness. Use
 * gtk_image_view_new() to instantiate GtkImageView objects.
 **/
struct _GtkImageView
{
    GtkWidget        parent;
    
    /* Private */
    gboolean         black_bg;
    gboolean         is_rendering;
    GdkInterpType    interp;
    gboolean         fitting;
    GdkPixbuf       *pixbuf;
    gdouble          zoom;
    /* Offset in zoom space coordinates of the image area in the
       widget. */
    int              offset_x;
    int              offset_y;
    gboolean         show_frame;
    gboolean         show_cursor;
    GdkCursor       *void_cursor;
    GtkAdjustment   *hadj;
    GtkAdjustment   *vadj;

    GtkIImageTool   *tool;

    GtkImageTransp   transp;
    int              check_color1;
    int              check_color2;
};

struct _GtkImageViewClass
{
    GtkWidgetClass parent_class;

	/* Keybinding signals. */
	void (* set_zoom)                        (GtkImageView    *view,
											  gdouble          zoom);
	void (* zoom_in)                         (GtkImageView    *view);
	void (* zoom_out)                        (GtkImageView    *view);

	void (* set_fitting)                     (GtkImageView    *view,
                                              gboolean         fitting);
	void (* scroll)							 (GtkImageView    *view,
											  GtkScrollType    xscroll,
											  GtkScrollType    yscroll);
	/* Non-keybinding signals. */
	void (* mouse_wheel_scroll)              (GtkImageView    *view,
											  GdkScrollDirection direction);
	void (* set_scroll_adjustments)			 (GtkImageView    *view,
											  GtkAdjustment   *hadj,
											  GtkAdjustment   *vadj);
	void (* zoom_changed)					 (GtkImageView    *view);
	void (* pixbuf_changed)                  (GtkImageView    *view);
};

GType         gtk_image_view_get_type        (void) G_GNUC_CONST;

/* Constructors */
GtkWidget    *gtk_image_view_new             (void);

/* Read-only properties */
gboolean      gtk_image_view_get_viewport    (GtkImageView    *view,
											  GdkRectangle    *rect);
gboolean      gtk_image_view_get_draw_rect   (GtkImageView    *view,
                                              GdkRectangle    *rect);
void          gtk_image_view_get_check_colors (GtkImageView  *view,
                                               int           *check_color1,
                                               int           *check_color2);
gboolean      gtk_image_view_widget_to_image_rect (GtkImageView *view,
                                                   GdkRectangle *rect_in,
                                                   GdkRectangle *rect_out);
gboolean      gtk_image_view_image_to_widget_rect (GtkImageView *view,
                                                   GdkRectangle *rect_in,
                                                   GdkRectangle *rect_out);

/* Write-only properties */
void          gtk_image_view_set_offset      (GtkImageView    *view,
											  gdouble          x,
											  gdouble          y,
                                              gboolean         invalidate);
void          gtk_image_view_set_transp      (GtkImageView    *view,
                                              GtkImageTransp   transp,
                                              int              transp_color);

/* Read-write properties */
gboolean      gtk_image_view_get_fitting     (GtkImageView    *view);
void          gtk_image_view_set_fitting     (GtkImageView    *view,
                                              gboolean         fitting);
GdkPixbuf    *gtk_image_view_get_pixbuf      (GtkImageView    *view);
void          gtk_image_view_set_pixbuf      (GtkImageView    *view,
                                              GdkPixbuf       *pixbuf,
                                              gboolean         reset_fit);
gdouble       gtk_image_view_get_zoom        (GtkImageView    *view);
void          gtk_image_view_set_zoom        (GtkImageView    *view,
                                              gdouble          zoom);

void          gtk_image_view_set_black_bg    (GtkImageView    *view,
											  gboolean         black_bg);
gboolean      gtk_image_view_get_black_bg    (GtkImageView    *view);

void          gtk_image_view_set_show_frame  (GtkImageView    *view,
                                              gboolean         show_frame);
gboolean      gtk_image_view_get_show_frame  (GtkImageView    *view);

void          gtk_image_view_set_interpolation (GtkImageView  *view,
                                                GdkInterpType  interp);
GdkInterpType gtk_image_view_get_interpolation (GtkImageView  *view);

void          gtk_image_view_set_show_cursor (GtkImageView    *view,
                                              gboolean         show_cursor);
gboolean      gtk_image_view_get_show_cursor (GtkImageView    *view);

void          gtk_image_view_set_tool        (GtkImageView    *view,
                                              GtkIImageTool   *tool);
GtkIImageTool *gtk_image_view_get_tool       (GtkImageView    *view);

/* Actions */
void          gtk_image_view_zoom_in	     (GtkImageView    *view);
void          gtk_image_view_zoom_out	     (GtkImageView    *view);
void          gtk_image_view_damage_pixels   (GtkImageView    *view,
                                              GdkRectangle    *rect);

/* Version info */
const char   *gtk_image_view_library_version (void);

G_END_DECLS
#endif
