/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * Copyright © 2007-2010 Björn Lindqvist <bjourne@gmail.com>
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

/**
 * SECTION:gtkimagetooldragger
 * @see_also: #GtkImageView, #GtkIImageTool, #GtkImageToolSelector
 * @short_description: Default image tool for paning the image
 *
 * <para>
 *   GtkImageToolDragger is the default image tool for #GtkImageView.
 *   Its only feature is that it can drag the image around.
 * </para>
 **/
#include <stdlib.h>
#include "cursors.h"
#include "gtkimagetooldragger.h"

/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/
/**
 * gtk_image_tool_dragger_is_draggable:
 * @returns: %TRUE if the view can be dragged when the cursor is at
 *   the position specified by @x and @y.
 *
 * Returns %TRUE if the view can be dragged when the cursor is at the
 * position specified by @x and @y.
 **/
static gboolean
gtk_image_tool_dragger_is_draggable (GtkImageToolDragger *dragger,
                                     int                  x,
                                     int                  y)
{
    GtkImageView *view = dragger->view;
    
    GdkRectangle draw_rect;
    if (!gtk_image_view_get_draw_rect (view, &draw_rect))
        return FALSE;
    
    gdouble zoom = gtk_image_view_get_zoom (view);
    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (view);

    int pb_w = gdk_pixbuf_get_width (pixbuf);
    int pb_h = gdk_pixbuf_get_height (pixbuf);

    int zoom_w = (int) (pb_w * zoom + 0.5);
    int zoom_h = (int) (pb_h * zoom + 0.5);

    GtkAllocation alloc;
    gtk_widget_get_allocation (GTK_WIDGET (view), &alloc);
    if (gdk_rectangle_contains (draw_rect, x, y) &&
        (zoom_w > alloc.width || zoom_h > alloc.height))
        return TRUE;
    return FALSE;
}

/*************************************************************/
/***** Implementation of the GtkIImageTool interface *********/
/*************************************************************/
static GdkCursor*
cursor_at_point (GtkIImageTool *tool,
                 int            x,
                 int            y)
{
    GtkImageToolDragger *dragger = GTK_IMAGE_TOOL_DRAGGER (tool);
    if (gtk_image_tool_dragger_is_draggable (dragger, x, y))
        return dragger->open_hand;
    return NULL;
}

static gboolean
button_press (GtkIImageTool  *tool,
              GdkEventButton *ev)
{
    GtkImageToolDragger *dragger = GTK_IMAGE_TOOL_DRAGGER (tool);
    if (!gtk_image_tool_dragger_is_draggable (dragger, ev->x, ev->y))
        return FALSE;
    return mouse_handler_button_press (dragger->mouse_handler, ev);
}

static gboolean
button_release (GtkIImageTool  *tool,
                GdkEventButton *ev)
{
    GtkImageToolDragger *dragger = GTK_IMAGE_TOOL_DRAGGER (tool);
    return mouse_handler_button_release (dragger->mouse_handler, ev);
}

static gboolean
motion_notify (GtkIImageTool  *tool,
               GdkEventMotion *ev)
{
    GtkImageToolDragger *dragger = GTK_IMAGE_TOOL_DRAGGER (tool);
    MouseHandler *mouse_handler = dragger->mouse_handler;

    mouse_handler_motion_notify (mouse_handler, ev);
    if (!mouse_handler->dragging)
        return FALSE;

    int dx, dy;
    mouse_handler_get_drag_delta (mouse_handler, &dx, &dy);
    if (abs (dx) < 1 && abs (dy) < 1)
        return FALSE;
    
    GdkRectangle viewport;
    gtk_image_view_get_viewport (dragger->view, &viewport);

    int offset_x = viewport.x + dx;
    int offset_y = viewport.y + dy;

    gtk_image_view_set_offset (dragger->view, offset_x, offset_y, FALSE);

    mouse_handler->drag_base_x = mouse_handler->drag_ofs_x;
    mouse_handler->drag_base_y = mouse_handler->drag_ofs_y;
    
    return TRUE;
}

static void
pixbuf_changed (GtkIImageTool *tool,
                gboolean       reset_fit,
                GdkRectangle  *rect)
{
    GtkImageToolDragger *dragger = GTK_IMAGE_TOOL_DRAGGER (tool);
    gdk_pixbuf_draw_cache_invalidate (dragger->cache);
}

static void
paint_image (GtkIImageTool     *tool,
             GdkPixbufDrawOpts *opts,
             GdkDrawable       *drawable)
{
    GtkImageToolDragger *dragger = GTK_IMAGE_TOOL_DRAGGER (tool);
    gdk_pixbuf_draw_cache_draw (dragger->cache, opts, drawable);
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

G_DEFINE_TYPE_EXTENDED (GtkImageToolDragger,
                        gtk_image_tool_dragger,
                        G_TYPE_OBJECT,
                        0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_IIMAGE_TOOL,
                                               gtk_iimage_tool_interface_init));

static void
gtk_image_tool_dragger_finalize (GObject *object)
{
    GtkImageToolDragger *dragger = GTK_IMAGE_TOOL_DRAGGER (object);
    gdk_cursor_unref (dragger->open_hand);
    gdk_cursor_unref (dragger->closed_hand);
    g_free (dragger->mouse_handler);
    gdk_pixbuf_draw_cache_free (dragger->cache);
    
    /* Chain up */
    G_OBJECT_CLASS (gtk_image_tool_dragger_parent_class)->finalize (object);
}

enum
{
    PROP_IMAGE_VIEW = 1
};

static void
gtk_image_tool_dragger_set_property (GObject      *object,
                                     guint         prop_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
    GtkImageToolDragger *dragger = GTK_IMAGE_TOOL_DRAGGER (object);
    if (prop_id == PROP_IMAGE_VIEW)
        dragger->view = g_value_get_object (value);
    else
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gtk_image_tool_dragger_class_init (GtkImageToolDraggerClass *klass)
{
    GObjectClass *object_class = (GObjectClass *) klass;
    object_class->finalize = gtk_image_tool_dragger_finalize;
    object_class->set_property = gtk_image_tool_dragger_set_property;

    GParamSpec *pspec =
        g_param_spec_object ("view",
                             "Image View",
                             "Image View to navigate",
                             GTK_TYPE_IMAGE_VIEW,
                             G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
    g_object_class_install_property (object_class, PROP_IMAGE_VIEW, pspec);
}

static void
gtk_image_tool_dragger_init (GtkImageToolDragger *tool)
{
    tool->open_hand = cursor_get (CURSOR_HAND_OPEN);
    tool->closed_hand = cursor_get (CURSOR_HAND_CLOSED);
    tool->mouse_handler = mouse_handler_new (tool->closed_hand);
    tool->view = NULL;
    tool->cache = gdk_pixbuf_draw_cache_new ();
}

/**
 * gtk_image_tool_dragger_new:
 * @view: a #GtkImageView
 * @returns: a new #GtkImageToolDragger
 *
 * Creates and returns a new dragger tool.
 **/
GtkIImageTool*
gtk_image_tool_dragger_new (GtkImageView *view)
{
    g_return_val_if_fail (view, NULL);
    gpointer data = g_object_new (GTK_TYPE_IMAGE_TOOL_DRAGGER,
                                  "view", view,
                                  NULL);
    return GTK_IIMAGE_TOOL (data);
}

