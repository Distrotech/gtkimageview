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
 * SECTION:gtkimagetoolselector
 * @see_also: #GtkIImageTool, #GtkImageToolDragger, gtk_image_view_set_tool()
 * @short_description: Image tool for selecting rectangular regions
 * <mediaobject>
 *   <imageobject align = "center">
 *     <imagedata fileref = "ex-monitor-selection.png" align = "center"/>
 *   </imageobject>
 *   <caption>
 *     <emphasis>Screenshot showing a GtkImageToolSelector with an
 *     active selection on a GtkImageView</emphasis>
 *   </caption>  
 * </mediaobject>
 *
 * <para>
 *   GtkImageToolSelector is a tool for selecting areas of an image. It
 *   is useful for cropping an image, for example. The tool is an
 *   implementor of the #GtkIImageTool inteface which means that it can
 *   be plugged into a #GtkImageView by using the
 *   gtk_image_view_set_tool() method.
 * </para>
 * <para>
 *   GtkImageToolSelector changes the default display of the
 *   GtkImageView. It darkens down the unselected region of the image
 *   which provides a nice effect and makes it clearer what part of the
 *   image that is currently selected. Unfortunately, this effect is
 *   somewhat incompatible with how #GtkImageNav behaves because that
 *   widget will show the image without darkening it.
 * </para>
 * <para>
 *   The tool also changes the default behaviour of the mouse. When a
 *   GtkImageToolSelector is set on a GtkImageView, mouse presses do not
 *   "grab" the image and you cannot scroll by dragging. Instead mouse
 *   presses and dragging is used to resize and move the selection
 *   rectangle. When the mouse drags the selection rectangle to the
 *   border of the widget, the view autoscrolls which is a convenient way
 *   for a user to position the selection.
 * </para>
 * <para>
 *   Please note that GtkImageToolSelector draws the image in two
 *   layers. One darkened and the selection rectangle in normal
 *   luminosity. Because it uses two draw operations instead one one like
 *   #GtkImageToolDragger does, it is significantly slower than that
 *   tool. Therefore, it makes sense for a user of this library to set
 *   the interpolation to %GDK_INTERP_NEAREST when using this tool to
 *   ensure that performance is acceptable to the users of the program.
 * </para>  
 * <refsect2>
 *   <title>Zoom bug</title>
 *   <para>
 *     There is a small bug in GtkImageToolSelector that becomes apparent
 *     when the zoom factor is greater than about 30. The edge of the
 *     selection rectangle may in that case intersect a pixel:
 *   </para>  
 *   <mediaobject>
 *     <imageobject align = "center">
 *       <imagedata fileref = "big-zoom-sel-bug.png" align = "center"/>
 *     </imageobject>
 *     <caption>
 *       <emphasis>The selection rectangle intersects the border pixel</emphasis>
 *     </caption>  
 *   </mediaobject>
 *   <para>
 *     The bug is caused by bug <ulink url =
 *     "http://bugzilla.gnome.org/show_bug.cgi?id=389832">389832</ulink> in
 *     <ulink url =
 *     "http://developer.gnome.org/doc/API/2.0/gdk-pixbuf/">gdk-pixbuf</ulink>. There
 *     is no way to solve this bug on GtkImageView's level (but if
 *     someone knows how, I'd really like to know).
 *   </para>  
 * </refsect2>
 **/ 

#include <stdlib.h>
#include "cursors.h"
#include "gtkimagetoolselector.h"

#define MIN_AUTOSCROLL      6

/*************************************************************/
/***** Private data ******************************************/
/*************************************************************/
static guint gtk_image_tool_selector_signals[1] = {0};

/*************************************************************/
/***** The hotspot list object *******************************/
/*************************************************************/
static void
hotspot_list_init (Hotspot *hotspots)
{
    hotspots[0].cursor = cursor_get (CURSOR_HAND_OPEN);
    hotspots[1].cursor = gdk_cursor_new (GDK_TOP_LEFT_CORNER);
    hotspots[2].cursor = gdk_cursor_new (GDK_TOP_RIGHT_CORNER);
    hotspots[3].cursor = gdk_cursor_new (GDK_BOTTOM_LEFT_CORNER);
    hotspots[4].cursor = gdk_cursor_new (GDK_BOTTOM_RIGHT_CORNER);
    hotspots[5].cursor = gdk_cursor_new (GDK_TOP_SIDE);
    hotspots[6].cursor = gdk_cursor_new (GDK_RIGHT_SIDE);
    hotspots[7].cursor = gdk_cursor_new (GDK_BOTTOM_SIDE);
    hotspots[8].cursor = gdk_cursor_new (GDK_LEFT_SIDE);
    hotspots[9].cursor = gdk_cursor_new (GDK_CROSSHAIR);
}

static void
hotspot_list_free (Hotspot *hotspots)
{
    for (int n = 0; n < HOTSPOT_LAST; n++)
        gdk_cursor_unref (hotspots[n].cursor);
}

static void
hotspot_list_recalculate (Hotspot      *hotspots,
                          GdkRectangle  rect)
{
    int border = 5;
    hotspots[HOTSPOT_INSIDE].rect = (GdkRectangle) {
        rect.x + border,
        rect.y + border,
        rect.width - border * 2,
        rect.height - border * 2
    };
    hotspots[HOTSPOT_RESIZE_NORTH_WEST].rect = (GdkRectangle) {
        rect.x - border,
        rect.y - border,
        border * 2,
        border * 2
    };
    hotspots[HOTSPOT_RESIZE_NORTH_EAST].rect = (GdkRectangle) {
        rect.x + rect.width - border,
        rect.y - border,
        border * 2,
        border * 2
    };
    hotspots[HOTSPOT_RESIZE_SOUTH_WEST].rect = (GdkRectangle) {
        rect.x - border,
        rect.y + rect.height - border,
        border * 2,
        border * 2
    };
    hotspots[HOTSPOT_RESIZE_SOUTH_EAST].rect = (GdkRectangle) {
        rect.x + rect.width - border,
        rect.y + rect.height - border,
        border * 2,
        border * 2
    };
    hotspots[HOTSPOT_RESIZE_NORTH].rect = (GdkRectangle) {
        rect.x + border,
        rect.y - border,
        rect.width - border * 2,
        border * 2
    };
    hotspots[HOTSPOT_RESIZE_EAST].rect = (GdkRectangle) {
        rect.x + rect.width - border,
        rect.y + border,
        border * 2,
        rect.height - border * 2
    };
    hotspots[HOTSPOT_RESIZE_SOUTH].rect = (GdkRectangle) {
        rect.x + border,
        rect.y + rect.height - border,
        rect.width - border * 2,
        border * 2
    };
    hotspots[HOTSPOT_RESIZE_WEST].rect = (GdkRectangle) {
        rect.x - border,
        rect.y + border,
        border * 2,
        rect.height - border * 2
    };
    
    // The sentinel hotspot, this one should catch every mouse event.
    hotspots[HOTSPOT_OUTSIDE].rect = (GdkRectangle) {
        -10000,
        -10000,
        20000,
        20000
    };
}

static HotspotType
hotspot_list_find_hotspot (Hotspot hotspots[HOTSPOT_LAST],
                           int     x,
                           int     y)
{
    for (int n = 0; n < HOTSPOT_LAST; n++)
        if (gdk_rectangle_contains (hotspots[n].rect, x, y))
            return n;
    g_assert_not_reached ();
    return HOTSPOT_LAST;
}

/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/
/**
 * gtk_image_tool_selector_update_selection:
 *
 * Sets the position of the tools selection rectangle. The position is
 * calculated based on the adjusted mouse drag delta which is
 * converted to image space coordinates.
 **/
static void
gtk_image_tool_selector_update_selection (GtkImageToolSelector *selector)
{
    int dx, dy;
    mouse_handler_get_drag_delta (selector->mouse_handler, &dx, &dy);
    gdouble zoom = gtk_image_view_get_zoom (selector->view);
    int img_dx = dx / zoom;
    int img_dy = dy / zoom;

    GdkRectangle new_sel = selector->sel_drag_start;
    int x1 = new_sel.x;
    int y1 = new_sel.y;
    int x2 = new_sel.x + new_sel.width;
    int y2 = new_sel.y + new_sel.height;

    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (selector->view);
    int pb_width = gdk_pixbuf_get_width (pixbuf);
    int pb_height = gdk_pixbuf_get_height (pixbuf);

    if (selector->hotspot_type == HOTSPOT_INSIDE)
    {
        x1 = CLAMP (x1 - img_dx, 0, pb_width - new_sel.width);
        y1 = CLAMP (y1 - img_dy, 0, pb_height - new_sel.height);
        x2 = x1 + new_sel.width;
        y2 = y1 + new_sel.height;
    }
    else if (selector->hotspot_type == HOTSPOT_RESIZE_NORTH)
        y1 -= img_dy;
    else if (selector->hotspot_type == HOTSPOT_RESIZE_EAST)
        x2 -= img_dx;
    else if (selector->hotspot_type == HOTSPOT_RESIZE_SOUTH)
        y2 -= img_dy;
    else if (selector->hotspot_type == HOTSPOT_RESIZE_WEST)
        x1 -= img_dx;
    else if (selector->hotspot_type == HOTSPOT_RESIZE_NORTH_WEST)
    {
        x1 -= img_dx;
        y1 -= img_dy;
    }
    else if (selector->hotspot_type == HOTSPOT_RESIZE_NORTH_EAST)
    {
        x2 -= img_dx;
        y1 -= img_dy;
    }
    else if (selector->hotspot_type == HOTSPOT_RESIZE_SOUTH_WEST)
    {
        x1 -= img_dx;
        y2 -= img_dy;
    }
    else if (selector->hotspot_type == HOTSPOT_RESIZE_SOUTH_EAST)
    {
        x2 -= img_dx;
        y2 -= img_dy;
    }

    int tmp;
    if (y1 > y2)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    if (x1 > x2)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }

    // Make it so the coordinates can't go outside the area of the
    // pixbuf.
    y1 = MAX (y1, 0);
    x1 = MAX (x1, 0);
    x2 = MIN (x2, pb_width);
    y2 = MIN (y2, pb_height);

    new_sel.x = x1;
    new_sel.y = y1;
    new_sel.width = x2 - x1;
    new_sel.height = y2 - y1;

    gtk_image_tool_selector_set_selection (selector, &new_sel);
}

/**
 * gtk_image_tool_selector_calc_autoscroll:
 * @mouse_x: X-coordinate of the mouse
 * @mouse_y: Y-coordinate of the mouse
 * @scroll_x: Returned X-coordinate of the calculated autoscroll offset
 * @scroll_y: Returned Y-coordinate of the calculated autoscroll offset.
 * 
 * Calculate the best autoscroll offset based on the mouse pointers
 * position.
 *
 * To avoid graphical artifacts, the offset should be a multiple of
 * the views zoom factor. If zoom is 4, then the offset should be 4n
 * where 'n' is an integer. autoscroll_cb is the function that does
 * the autoscrolling, it moves both the selection rectangle and the
 * viewport. If the offset is not a multiple of the zoom factor, then
 * the selection rectangles movement will appear jumpy.
 *
 * If the zoom factor is not an integer (7.5 for example), then the
 * autoscrolling will appear jumpy anyway. 
 **/
static void
gtk_image_tool_selector_calc_autoscroll (GtkImageToolSelector *selector,
                                         int                   mouse_x,
                                         int                   mouse_y,
                                         int                  *scroll_x,
                                         int                  *scroll_y)
{
    gdouble zoom = gtk_image_view_get_zoom (selector->view);
    GtkAllocation alloc = GTK_WIDGET (selector->view)->allocation;

    /* Ensure that we atleast scroll something if the mouse is outside
       the widget. */
    zoom = MAX (zoom, 1.0);

    *scroll_x = 0;
    if (mouse_x <= 0)
        *scroll_x = -zoom - (abs (mouse_x) / 4) * zoom;
    else if (mouse_x >= alloc.width)
        *scroll_x = zoom + (abs (mouse_x - alloc.width) / 4) * zoom;

    *scroll_y = 0;
    if (mouse_y <= 0)
        *scroll_y = -zoom - (abs (mouse_y) / 4) * zoom;
    else if (mouse_y >= alloc.height)
        *scroll_y = zoom + (abs (mouse_y - alloc.height) / 4) * zoom;
}

/**
 * gtk_image_tool_selector_get_hotspot_at_point:
 *
 * Return the type of the hotspot at the point @x, @y. If there is no
 * pixbuf in the view, or if it is realized, or if the point is
 * outside the views area return %HOTSPOT_LAST.
 **/
static HotspotType
gtk_image_tool_selector_get_hotspot_at_point (GtkImageToolSelector *selector,
                                              int                   x,
                                              int                   y)
{
    GdkRectangle image_rect;
    gtk_image_view_get_draw_rect (selector->view, &image_rect);
    if (!gdk_rectangle_contains (image_rect, x, y))
        return HOTSPOT_LAST;
    GdkRectangle wid_sel;
    if (!gtk_image_view_image_to_widget_rect (selector->view,
                                              &selector->sel_rect,
                                              &wid_sel))
        return HOTSPOT_LAST;
    hotspot_list_recalculate (selector->hotspots, wid_sel);
    return hotspot_list_find_hotspot (selector->hotspots, x, y);
}


/*************************************************************/
/***** Implementation of the GtkIImageTool interface *********/
/*************************************************************/
static GdkCursor*
cursor_at_point (GtkIImageTool *tool,
                 int            x,
                 int            y)
{
    GtkImageToolSelector *selector = GTK_IMAGE_TOOL_SELECTOR (tool);
    HotspotType type =
        gtk_image_tool_selector_get_hotspot_at_point (selector, x, y);
    if (type == HOTSPOT_LAST)
        return NULL;
    return selector->hotspots[type].cursor;
}

static gboolean
button_press (GtkIImageTool  *tool,
              GdkEventButton *ev)
{
    GtkImageToolSelector *selector = GTK_IMAGE_TOOL_SELECTOR (tool);
    if (ev->button != 1)
        return FALSE;

    selector->hotspot_type =
        gtk_image_tool_selector_get_hotspot_at_point (selector, ev->x, ev->y);
    if (selector->hotspot_type == HOTSPOT_LAST)
        return FALSE;
    if (selector->hotspot_type == HOTSPOT_OUTSIDE)
    {
        GdkRectangle widget_rect = {ev->x, ev->y, 1, 1};
        GdkRectangle image_rect;
        
        gtk_image_view_widget_to_image_rect (selector->view,
                                             &widget_rect,
                                             &image_rect);
        image_rect.width = 0;
        image_rect.height = 0;
        gtk_image_tool_selector_set_selection (selector, &image_rect);
        selector->hotspot_type = HOTSPOT_RESIZE_SOUTH_EAST;
    }
    
    selector->sel_drag_start = selector->sel_rect;
    
    // Set which cursor the mouse handler should use for grabbing.
    GdkCursor *cursor = selector->hotspots[selector->hotspot_type].cursor;
    if (selector->hotspot_type == HOTSPOT_INSIDE)
        cursor = selector->drag_cursor;
    selector->mouse_handler->grab_cursor = cursor;
    return mouse_handler_button_press (selector->mouse_handler, ev);
}

static gboolean
button_release (GtkIImageTool  *tool,
                GdkEventButton *ev)
{
    GtkImageToolSelector *selector = GTK_IMAGE_TOOL_SELECTOR (tool);
    if (selector->timer_id && ev->button == 1)
    {
        g_source_remove (selector->timer_id);
        selector->timer_id = 0;
    }
    return mouse_handler_button_release (selector->mouse_handler, ev);
}

static gboolean
autoscroll_cb (gpointer data)
{
    GtkImageToolSelector *selector = (GtkImageToolSelector *)data;
    GtkImageView *view = selector->view;

    GdkRectangle viewport;
    gtk_image_view_get_viewport (view, &viewport);
    gtk_image_view_set_offset (view,
                               viewport.x + selector->outside_x,
                               viewport.y + selector->outside_y,
                               TRUE);

    // How much was the view scrolled? 
    GdkRectangle viewport2;
    gtk_image_view_get_viewport (view, &viewport2);

    /* Tweak the base coordinate for the drag in accordance with how
       much the view really was scrolled. */
    selector->mouse_handler->drag_base_x -= viewport2.x - viewport.x;
    selector->mouse_handler->drag_base_y -= viewport2.y - viewport.y;

    gtk_image_tool_selector_update_selection (selector);
    return TRUE;
}

static gboolean
motion_notify (GtkIImageTool  *tool,
               GdkEventMotion *ev)
{
    GtkImageToolSelector *selector = GTK_IMAGE_TOOL_SELECTOR (tool);
    mouse_handler_motion_notify (selector->mouse_handler, ev);
    if (!selector->mouse_handler->dragging)
        return FALSE;
    
    gtk_image_tool_selector_update_selection (selector);

    /* Check if, and how much the view should be autoscrolled. */
    gtk_image_tool_selector_calc_autoscroll (selector,
                                             ev->x, ev->y,
                                             &selector->outside_x,
                                             &selector->outside_y);
    if (selector->outside_x || selector->outside_y)
    {
        if (!selector->timer_id)
            selector->timer_id = g_timeout_add (30, autoscroll_cb, selector);
    }
    else if (selector->timer_id)
    {
        g_source_remove (selector->timer_id);
        selector->timer_id = 0;
    }
    return TRUE;
}

static void
pixbuf_changed (GtkIImageTool *tool,
                gboolean       reset_fit,
                GdkRectangle  *rect)
{
    GtkImageToolSelector *selector = GTK_IMAGE_TOOL_SELECTOR (tool);
    if (reset_fit)
        selector->sel_rect = (GdkRectangle){0, 0, 0, 0};

    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (selector->view);
    if (!pixbuf)
        return;

    if (rect)
    {
        // Copy the damaged area from the foreground to the
        // background.
        gdk_pixbuf_copy_area (pixbuf,
                              rect->x, rect->y,
                              rect->width, rect->height,
                              selector->background,
                              rect->x, rect->y);
    }
    else
    {
        if (selector->background)
            g_object_unref (selector->background);
        selector->background = gdk_pixbuf_copy (pixbuf);
    }
    // Update the relevant area of the background.
    gdk_pixbuf_shade (selector->background, rect);

    // Clear caches
    gdk_pixbuf_draw_cache_invalidate (selector->bg_cache);
    gdk_pixbuf_draw_cache_invalidate (selector->fg_cache);
}

static void
paint_image (GtkIImageTool     *tool,
             GdkPixbufDrawOpts *opts,
             GdkDrawable       *drawable)
{
    GtkImageToolSelector *selector = GTK_IMAGE_TOOL_SELECTOR (tool);

    // Draw the shaded background.
    GdkPixbufDrawOpts bg_opts = *opts;
    bg_opts.pixbuf = selector->background;
    gdk_pixbuf_draw_cache_draw (selector->bg_cache, &bg_opts, drawable);

    // Draw the selected area.
    // Intersect the selection rectangle with the exposed rectangle.
    GdkRectangle zoom_sel_rect = {
        selector->sel_rect.x * opts->zoom,
        selector->sel_rect.y * opts->zoom,
        selector->sel_rect.width * opts->zoom,
        selector->sel_rect.height * opts->zoom
    };
    GdkRectangle zoom_paint_rect;
    if (!gdk_rectangle_intersect (&zoom_sel_rect,
                                  &opts->zoom_rect,
                                  &zoom_paint_rect))
        return;

    GdkPixbufDrawOpts fg_opts = *opts;

    fg_opts.zoom_rect = zoom_paint_rect;
    fg_opts.widget_x =
        opts->widget_x + zoom_paint_rect.x - opts->zoom_rect.x;
    fg_opts.widget_y =
        opts->widget_y + zoom_paint_rect.y - opts->zoom_rect.y;
    
    gdk_pixbuf_draw_cache_draw (selector->fg_cache, &fg_opts, drawable);

    // Draw the selection rectangle.
    GdkGC *rect_gc = gdk_gc_new (drawable);
    gdk_gc_copy (rect_gc, GTK_WIDGET (selector->view)->style->black_gc);
    gdk_gc_set_line_attributes (rect_gc, 1,
                                GDK_LINE_DOUBLE_DASH,
                                GDK_CAP_BUTT,
                                GDK_JOIN_MITER);
    GdkRectangle wid_rect;
    gtk_image_view_image_to_widget_rect (selector->view,
                                         &selector->sel_rect,
                                         &wid_rect);
    gdk_draw_rect (drawable, rect_gc, FALSE, &wid_rect);
    g_object_unref (rect_gc);
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

G_DEFINE_TYPE_EXTENDED (GtkImageToolSelector,
                        gtk_image_tool_selector,
                        G_TYPE_OBJECT,
                        0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_IIMAGE_TOOL,
                                               gtk_iimage_tool_interface_init));

static void
gtk_image_tool_selector_finalize (GObject *object)
{
    GtkImageToolSelector *selector = GTK_IMAGE_TOOL_SELECTOR (object);
    if (selector->background)
        g_object_unref (selector->background);
    gdk_pixbuf_draw_cache_free (selector->bg_cache);
    gdk_pixbuf_draw_cache_free (selector->fg_cache);

    gdk_cursor_unref (selector->drag_cursor);
    hotspot_list_free (selector->hotspots);
    
    g_free (selector->mouse_handler);
    /* Chain up */
    G_OBJECT_CLASS (gtk_image_tool_selector_parent_class)->finalize (object);
}

static void
gtk_image_tool_selector_init_signals (GtkImageToolSelectorClass *klass)
{
    /**
     * GtkImageToolSelector::selection-changed:
     * @selector: the selector that emitted the signal.
     *
     * The ::selection-changed signal is emitted when the selection
     * rectangle on the selector is moved or resized. It is inteded to
     * be used by applications that wants to print status
     * information. For example:
     *
     * <informalexample>
     *   <programlisting>     
     *     static void
     *     sel_changed_cb (GtkImageToolSelector *selector)
     *     {
     *         GdkRectangle sel;
     *         gtk_image_tool_selector_get_selection (selector, &sel);
     *         if (!sel.width || !sel.height)
     *             printf ("No selection\n");
     *         else
     *             printf ("The selection is %d, %d - %d, %d\n",
     *                     sel.x, sel.y, sel.width, sel.height);
     *     }
     *     ...
     *     // selector is an already initialized GtkImageToolSelector
     *     // object.
     *     g_signal_connect (G_OBJECT(selector), "selection_changed",
     *                       G_CALLBACK (sel_changed_cb), NULL);
     *   </programlisting>
     * </informalexample>     
     **/
    gtk_image_tool_selector_signals[0] =
        g_signal_new ("selection_changed",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST,
                      G_STRUCT_OFFSET (GtkImageToolSelectorClass,
                                       selection_changed),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);
}

enum
{
    PROP_IMAGE_VIEW = 1
};

static void
gtk_image_tool_selector_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
    GtkImageToolSelector *selector = GTK_IMAGE_TOOL_SELECTOR (object);
    if (prop_id == PROP_IMAGE_VIEW)
        selector->view = g_value_get_object (value);
    else
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gtk_image_tool_selector_class_init (GtkImageToolSelectorClass *klass)
{
    gtk_image_tool_selector_init_signals (klass);
    
    GObjectClass *object_class = (GObjectClass *) klass;
    object_class->finalize = gtk_image_tool_selector_finalize;
    object_class->set_property = gtk_image_tool_selector_set_property;

    GParamSpec *pspec =
        g_param_spec_object ("view",
                             "Image View",
                             "Image View to navigate",
                             GTK_TYPE_IMAGE_VIEW,
                             G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
    g_object_class_install_property (object_class, PROP_IMAGE_VIEW, pspec);

    klass->selection_changed = NULL;
}

static void
gtk_image_tool_selector_init (GtkImageToolSelector *tool)
{
    tool->background = NULL;
    tool->view = NULL;
    tool->sel_rect = (GdkRectangle){0, 0, 0, 0};
    tool->bg_cache = gdk_pixbuf_draw_cache_new ();
    tool->fg_cache = gdk_pixbuf_draw_cache_new ();

    tool->drag_cursor = cursor_get (CURSOR_HAND_CLOSED);
    tool->mouse_handler = mouse_handler_new (tool->drag_cursor);
    tool->timer_id = 0;

    // Init hotspots cursors.
    hotspot_list_init (tool->hotspots);
}

/**
 * gtk_image_tool_selector_new:
 * @view: a #GtkImageView
 * @returns: a new #GtkImageToolSelector
 *
 * Creates a new selector tool for the specified view with default
 * values. The default values are:
 *
 * <itemizedlist>
 *   <listitem>selection : (0, 0) - [0, 0]</listitem>
 * </itemizedlist>
 **/
GtkIImageTool*
gtk_image_tool_selector_new (GtkImageView *view)
{
    g_return_val_if_fail (view, NULL);
    gpointer data = g_object_new (GTK_TYPE_IMAGE_TOOL_SELECTOR,
                                  "view", view,
                                  NULL);
    return GTK_IIMAGE_TOOL (data);
}

/*************************************************************/
/***** Read-write properties *********************************/
/*************************************************************/
/**
 * gtk_image_tool_selector_get_selection:
 * @selector: a #GtkImageToolSelector
 * @rect: a #GdkRectangle to fill in with the current selection
 * rectangle.
 *
 * Fills in @rect with the current selection rectangle. If either the
 * width or the height of @rect is zero, then nothing is selected and
 * the selection should be considered inactive. See
 * #GtkImageToolSelector::selection-changed for an example.
 **/
void
gtk_image_tool_selector_get_selection (GtkImageToolSelector *selector,
                                       GdkRectangle         *rect)
{
    *rect = selector->sel_rect;
}

/**
 * gtk_image_tool_selector_set_selection:
 * @selector: a #GtkImageToolSelector
 * @rect: Selection rectangle in image space coordinates.
 *
 * Sets the selection rectangle for the tool. Setting this attribute
 * will cause the widget to immediately repaint itself if its view is
 * realized.
 *
 * This method does nothing under the following circumstances:
 *
 * <itemizedlist>
 *   <listitem>If the views pixbuf is %NULL.</listitem>
 *   <listitem>If @rect is wider or taller than the size of the
 *   pixbuf</listitem>
 *   <listitem>If @rect equals the current selection
 *   rectangle.</listitem>
 * </itemizedlist>
 *
 * If the selection falls outside the pixbufs area, its position is
 * moved so that it is within the pixbuf.
 *
 * Calling this method causes the ::selection-changed signal to be
 * emitted.
 *
 * The default selection is (0,0) - [0,0].
 **/
void
gtk_image_tool_selector_set_selection (GtkImageToolSelector *selector,
                                       GdkRectangle         *rect)
{
    GtkImageView *view = selector->view;
    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (view);
    if (!pixbuf)
        return;

    int width = gdk_pixbuf_get_width (pixbuf);
    int height = gdk_pixbuf_get_height (pixbuf);
    if (rect->width > width || rect->height > height)
        return;

    rect->x = CLAMP (rect->x, 0, width - rect->width);
    rect->y = CLAMP (rect->y, 0, height - rect->height);
    if (gdk_rectangle_eq (*rect, selector->sel_rect))
        return;

    GdkRectangle wid_old = {0}, wid_new = {0};
    gtk_image_view_image_to_widget_rect (view, &selector->sel_rect, &wid_old);
    gtk_image_view_image_to_widget_rect (view, rect, &wid_new);

    selector->sel_rect = *rect;

    gtk_widget_queue_draw_area (GTK_WIDGET (view),
                                wid_old.x, wid_old.y,
                                wid_old.width, wid_old.height);
    gtk_widget_queue_draw_area (GTK_WIDGET (view),
                                wid_new.x, wid_new.y,
                                wid_new.width, wid_new.height);

    g_signal_emit (G_OBJECT (selector),
                   gtk_image_tool_selector_signals[0], 0);
}
