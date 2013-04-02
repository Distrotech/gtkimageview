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

/**
 * SECTION:gtkiimagetool
 * @see_also: <ulink url =
 * "http://developer.gnome.org/doc/API/2.0/gobject/gtype-non-instantiable-classed.html">Non-instantiable
 * classed types: Interfaces</ulink>
 * @short_description: Interface for objects capable of being used as
 * tools by #GtkImageView
 *
 * <para>
 *   GtkIImageTool is an interface that defines how GtkImageView
 *   interacts with objects that acts as tools. GtkImageView delegates
 *   many of its most important tasks (such as drawing) to its tool which
 *   carries out all the hard work. The GtkImageView package comes with
 *   two tools; #GtkImageToolDragger and #GtkImageToolSelector, but by
 *   implementing your own tool it is possible to extend GtkImageView to
 *   do stuff its author (thats me) didn't imagine.
 * </para>
 * <para>
 *   GtkImageView uses #GtkImageToolDragger by default, as that tool is
 *   he most generally useful one. However, it is trivial to make it use
 *   another tool.
 *   <informalexample>
 *     <programlisting>
 *       GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
 *       GtkIImageTool *tool = gtk_image_tool_selector_new (view);
 *       gtk_image_view_set_tool (view, tool);
 *     </programlisting>  
 *   </informalexample>
 *   Using the above code makes the view use the selector tool instead of
 *   the default dragger tool.
 * </para>
 **/
#include "gtkiimagetool.h"

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
GType
gtk_iimage_tool_get_type (void)
{
    static GType type = 0;
    if (type)
        return type;
    static const GTypeInfo info = {
        sizeof (GtkIImageToolClass),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0,
        0,
        NULL
    };
    type = g_type_register_static (G_TYPE_INTERFACE,
                                   "GtkIImageTool",
                                   &info,
                                   0);
    return type;
}

/*************************************************************/
/***** Pseudo signal handlers ********************************/
/*************************************************************/
gboolean
gtk_iimage_tool_button_press (GtkIImageTool  *tool,
                              GdkEventButton *ev)
{
    return GTK_IIMAGE_TOOL_GET_CLASS (tool)->button_press (tool, ev);
}

/**
 * gtk_iimage_tool_button_release:
 * @tool: the tool
 * @ev: a %GdkEventButton event
 * @returns: %TRUE if the event was handled, %FALSE otherwise
 *
 * Handles a %GdkEventButton event of a button release. The
 * implementing function should return %TRUE if it handled the event,
 * for examply by releasing the grab, and %FALSE otherwise.
 **/
gboolean
gtk_iimage_tool_button_release (GtkIImageTool  *tool,
                                GdkEventButton *ev)
{
    return GTK_IIMAGE_TOOL_GET_CLASS (tool)->button_release (tool, ev);
}

/**
 * gtk_iimage_tool_motion_notify:
 * @tool: the tool
 * @ev: a %GdkEventMotion event
 * @returns: %TRUE if the event was handled, %FALSE otherwise
 *
 * Handles a %GdkEventMotion event. The implementing function should
 * return %TRUE if it handled the event and %FALSE otherwise.
 **/
gboolean
gtk_iimage_tool_motion_notify (GtkIImageTool  *tool,
                               GdkEventMotion *ev)
{
    return GTK_IIMAGE_TOOL_GET_CLASS (tool)->motion_notify (tool, ev);
}

/**
 * gtk_iimage_tool_pixbuf_changed:
 * @tool: the tool
 * @reset_fit: whether the view is resetting its fit mode or not
 * @rect: rectangle containing the changed area or %NULL
 *
 * Indiate to the tool that either a part of, or the whole pixbuf that
 * the image view shows has changed. This method is called by the view
 * whenever its pixbuf or its tool changes. That is, when any of the
 * following methods are used:
 *
 * <itemizedlist>
 *   <listitem>gtk_image_view_set_pixbuf()</listitem>
 *   <listitem>gtk_image_view_set_tool()</listitem>
 *   <listitem>gtk_image_view_damage_pixels()</listitem>
 * </itemizedlist>
 *
 * If the @reset_fit parameter is %TRUE, it means that a new pixbuf
 * has been loaded into the view. @rect is a rectangle in image space
 * coordinates that indicates which rectangular region of the pixbufs
 * pixels that is modified. If @rect is %NULL, then all of the pixbuf
 * has changed. See also GtkImageView::pixbuf-changed.
 **/
void
gtk_iimage_tool_pixbuf_changed (GtkIImageTool *tool,
                                gboolean       reset_fit,
                                GdkRectangle  *rect)
{
    GtkIImageToolClass *klass = GTK_IIMAGE_TOOL_GET_CLASS (tool);
    klass->pixbuf_changed (tool, reset_fit, rect);
}

/**
 * gtk_iimage_tool_paint_image:
 * @tool: the tool
 * @opts: the #GdkPixbufDrawOpts to use in this draw
 * @drawable: a #GdkDrawable to draw on
 *
 * Called whenever the image view decides that any part of the image
 * it shows needs to be redrawn.
 **/
void
gtk_iimage_tool_paint_image (GtkIImageTool     *tool,
                             GdkPixbufDrawOpts *opts,
                             GdkDrawable       *drawable)
{
    GtkIImageToolClass *klass = GTK_IIMAGE_TOOL_GET_CLASS (tool);
    return klass->paint_image (tool, opts, drawable);
}

/*************************************************************/
/***** Read-only properties **********************************/
/*************************************************************/
/**
 * gtk_iimage_tool_cursor_at_point:
 * @x: the mouse pointers X-coordinate
 * @y: the mouse pointers Y-coordinate
 * @returns: The appropriate cursor or %NULL if the widgets default
 *   cursor should be used.
 *
 * Ask the tool what cursor it wants displayed. E.g. if the tool keeps
 * track of image maps in the image, then this method can be
 * implemented to return an appropriate cursor when the mouse pointer
 * hovers at the specified point in the widget.
 **/
GdkCursor*
gtk_iimage_tool_cursor_at_point (GtkIImageTool *tool,
                                 int            x,
                                 int            y)
{
    return GTK_IIMAGE_TOOL_GET_CLASS (tool)->cursor_at_point (tool, x, y);
}

