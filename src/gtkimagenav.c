/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * Copyright © 2007-2011 Björn Lindqvist <bjourne@gmail.com>
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
 * SECTION: gtkimagenav
 * @short_description: Navigation window showing a thumbnailed
 * overview of a #GtkImageView.
 * <mediaobject>
 *  <imageobject align = "center">
 *    <imagedata fileref = "navwin-open.png" align = "center"/>
 *  </imageobject>
 *  <caption>
 *    <emphasis>Screenshot of an active navigation window</emphasis>
 *  </caption>  
 * </mediaobject>
 *
 * <para>
 *   GtkImageNav is a popup window that shows a downscaled preview of
 *   the pixbuf that #GtkImageView is showing. The user can drag
 *   around a rectangle which indicates the current view of the image.
 * </para>
 * <para>
 *   This class is used by #GtkImageScrollWin itself. It is probably not
 *   very useful for clients of this library.
 * </para>
 * <para>
 *   GtkImageNav has the same keybindings that #GtkImageView has. All
 *   keypresses that it receives are passed along to the view.
 * </para>
 **/
#include "gtkimagenav.h"

G_DEFINE_TYPE (GtkImageNav, gtk_image_nav, GTK_TYPE_WINDOW);

/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/
static gdouble
gtk_image_nav_get_zoom (GtkImageNav *nav)
{
    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (nav->view);
    int img_width = gdk_pixbuf_get_width (pixbuf);
	int img_height = gdk_pixbuf_get_height (pixbuf);

	gdouble width_zoom =
		(gdouble)GTK_IMAGE_NAV_MAX_WIDTH / (gdouble)img_width;
	gdouble height_zoom =
		(gdouble)GTK_IMAGE_NAV_MAX_HEIGHT / (gdouble)img_height;
	return MIN (width_zoom, height_zoom);
}


static Size
gtk_image_nav_get_preview_size (GtkImageNav *nav)
{
	GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (nav->view);
    if (!pixbuf)
        return (Size){GTK_IMAGE_NAV_MAX_WIDTH, GTK_IMAGE_NAV_MAX_HEIGHT};
	int img_width = gdk_pixbuf_get_width (pixbuf);
	int img_height = gdk_pixbuf_get_height (pixbuf);

	gdouble zoom = gtk_image_nav_get_zoom (nav);

	Size s;
	s.width = (int) (img_width * zoom + 0.5);
	s.height = (int) (img_height * zoom + 0.5);
	return s;
}

static gdouble
gtk_image_nav_get_zoom2nav_factor (GtkImageNav *nav)
{
    gdouble nav_zoom = gtk_image_nav_get_zoom (nav);
    gdouble view_zoom = gtk_image_view_get_zoom (nav->view);
    return view_zoom / nav_zoom;
}

static void
gtk_image_nav_draw_rectangle (GtkImageNav *nav,
                              gboolean     clear_last)
{
    /* Convert Zoom space to Nav space coordinates. */
    GdkRectangle rect;
	gtk_image_view_get_viewport (nav->view, &rect);

    gdouble zoom2nav_factor = gtk_image_nav_get_zoom2nav_factor (nav);
    rect.x = (gdouble)rect.x / zoom2nav_factor;
	rect.y = (gdouble)rect.y / zoom2nav_factor;
	rect.width = (gdouble)rect.width / zoom2nav_factor;
	rect.height = (gdouble)rect.height / zoom2nav_factor;

    GdkWindow *preview_window = gtk_widget_get_window (nav->preview);

    /* Clear the last drawn rectangle. */
    if (clear_last)
        gdk_draw_rectangle (preview_window, nav->gc, FALSE,
                            nav->last_rect.x, nav->last_rect.y,
                            nav->last_rect.width, nav->last_rect.height);
    
    gdk_draw_rectangle (preview_window,
                        nav->gc, FALSE,
                        rect.x, rect.y, rect.width, rect.height);
    nav->last_rect = rect;
}

static void
gtk_image_nav_update_position (GtkImageNav *nav)
{
    /* Calculate position of popup. */
    Size pw = gtk_image_nav_get_preview_size (nav);

    int x = nav->center_x - (pw.width / 2);
    int y = nav->center_y - (pw.height / 2);
    
    /* 10 is an approximation for the width and height of the window
       border. This is to ensure that the window is fully visible on
       screen. */
    x = CLAMP (x, 0, gdk_screen_width() - pw.width - 10);
    y = CLAMP (y, 0, gdk_screen_height() - pw.height - 10);
    gtk_window_move (GTK_WINDOW (nav), x, y);
}

static void
gtk_image_nav_update_pixbuf (GtkImageNav *nav)
{
    if (nav->pixbuf)
    {
        g_object_unref (nav->pixbuf);
        nav->pixbuf = NULL;
    }
    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (nav->view);
    if (!pixbuf)
        return;

    Size pw = gtk_image_nav_get_preview_size (nav);

    int col1, col2;
    gtk_image_view_get_check_colors (nav->view, &col1, &col2);
    nav->pixbuf = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (pixbuf),
                                  gdk_pixbuf_get_has_alpha (pixbuf),
                                  8,
                                  pw.width, pw.height);
    gdk_pixbuf_scale_blend (pixbuf, nav->pixbuf,
                            0, 0, pw.width, pw.height,
                            0, 0,
                            gtk_image_nav_get_zoom (nav),
                            GDK_INTERP_BILINEAR,
                            0, 0,
                            16, col1, col2);
    // Lower the flag so the pixbuf isn't recreated more than
    // necessarily.
    nav->update_when_shown = FALSE;
}


/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/
static gboolean
gtk_image_nav_expose_drawing_area (GtkWidget         *widget,
								   GdkEventExpose    *ev,
								   GtkImageNav       *nav)
{
    if (!nav->pixbuf)
        return FALSE;

    GdkWindow *window = gtk_widget_get_window (nav->preview);
    GtkStyle *style = gtk_widget_get_style (nav->preview);
    gdk_draw_pixbuf (window,
                     style->white_gc,
                     nav->pixbuf,
                     0, 0,
                     0, 0,
                     -1, -1,
                     GDK_RGB_DITHER_MAX,
                     0, 0);
	gtk_image_nav_draw_rectangle (nav, FALSE);
	return TRUE;
}

/**
 * gtk_image_nav_key_press:
 *
 * Keyboard events are catched just to forward them to the
 * #GtkImageView which responds to them. That way, keyboard navigation
 * in #GtkImageNav behaves consistently with #GtkImageView.
 **/
static int
gtk_image_nav_key_press (GtkWidget   *widget,
                         GdkEventKey *ev)
{
    GtkImageNav *nav = GTK_IMAGE_NAV (widget);
    int retval = gtk_bindings_activate (GTK_OBJECT (nav->view),
                                        ev->keyval,
                                        ev->state);
    gtk_image_nav_draw_rectangle (nav, TRUE);
    return retval;
}

static int
gtk_image_nav_motion_notify (GtkWidget      *widget,
							 GdkEventMotion *ev)
{
    GtkImageNav *nav = GTK_IMAGE_NAV (widget);
    GdkWindow *window = gtk_widget_get_window (widget);
	int mx, my;
	gdk_window_get_pointer (window, &mx, &my, NULL);
    
    /* Make coordinates relative to window. */
    mx -= 4;
    my -= 4;

	/* Convert Nav space to Zoom space coordinates. */
    gdouble zoom2nav_factor = gtk_image_nav_get_zoom2nav_factor (nav);
    GdkRectangle rect;
	gtk_image_view_get_viewport (nav->view, &rect);

    /* Convert Zoom space to Nav space coordinates. */
    rect.width = (gdouble)rect.width / zoom2nav_factor;
    rect.height = (gdouble)rect.height / zoom2nav_factor;

    /* Subtract half of the XOR rectangles size from the coordinates. */
    mx -= (rect.width / 2);
    my -= (rect.height / 2);

    /* Do the adjusted coordinate conversion. */
    gdouble zoom_x_ofs = (gdouble)mx * zoom2nav_factor;
    gdouble zoom_y_ofs = (gdouble)my * zoom2nav_factor;

    gtk_image_view_set_offset (nav->view, zoom_x_ofs, zoom_y_ofs, FALSE);
    gtk_image_nav_draw_rectangle (nav, TRUE);
	
	return TRUE;
}

static void
gtk_image_nav_pixbuf_changed (GtkImageNav *nav)
{
    Size pw = gtk_image_nav_get_preview_size (nav);

    // Set the new size and position of the preview. 
    gtk_widget_set_size_request (GTK_WIDGET (nav->preview),
                                 pw.width, pw.height);
    gtk_image_nav_update_position (nav);

    // If the widget is showing, then create the downsampled pixbuf.
    // Otherwise, just set a flag so that it is done later.
    nav->update_when_shown = TRUE;
    if (!gtk_widget_get_visible (GTK_WIDGET (nav)))
        return;
    gtk_image_nav_update_pixbuf (nav);
    gtk_widget_queue_draw (GTK_WIDGET (nav));
}

/**
 * gtk_image_nav_zoom_changed:
 *
 * Callback that is called whenever the zoom of the #GtkImageView
 * changes.
 *
 * This function is needed because the zoom of the view may change
 * after a call to gtk_image_view_set_zoom() due to the widget being
 * queued for size allocation.
 **/
static void
gtk_image_nav_zoom_changed (GtkImageNav *nav)
{
    gtk_image_nav_draw_rectangle (nav, TRUE);
}

/**
 * gtk_image_nav_button_released:
 *
 * Callback that is called whenever a mouse button is released. If LMB
 * is released, the nav is hidden.
 **/
static void
gtk_image_nav_button_released (GtkImageNav    *nav,
                               GdkEventButton *ev)
{
    if (ev->button != 1)
        return;
    gtk_image_nav_release (nav);
    gtk_widget_hide (GTK_WIDGET (nav));
}

static void
gtk_image_nav_realize (GtkWidget *widget)
{
    GTK_WIDGET_CLASS (gtk_image_nav_parent_class)->realize (widget);
    GtkImageNav *nav = GTK_IMAGE_NAV (widget);
    GdkWindow *window = gtk_widget_get_window (widget);
    nav->gc = gdk_gc_new (window);
    gdk_gc_set_function (nav->gc, GDK_INVERT);
    gdk_gc_set_line_attributes (nav->gc,
                                3,
                                GDK_LINE_SOLID,
                                GDK_CAP_BUTT,
                                GDK_JOIN_MITER);
}

static void
gtk_image_nav_unrealize (GtkWidget *widget)
{
    g_object_unref (GTK_IMAGE_NAV (widget)->gc);
    GTK_WIDGET_CLASS (gtk_image_nav_parent_class)->unrealize (widget);
}

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
gtk_image_nav_init (GtkImageNav *nav)
{
	nav->view = NULL;
    nav->gc = NULL;
    nav->last_rect = (GdkRectangle){-1, -1, -1, -1};
    nav->update_when_shown = FALSE;
	
	GtkWidget *out_frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (out_frame), GTK_SHADOW_OUT);
	gtk_container_add (GTK_CONTAINER (nav), out_frame);

	GtkWidget *in_frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (in_frame), GTK_SHADOW_IN);
	gtk_container_add (GTK_CONTAINER (out_frame), in_frame);

	nav->preview = gtk_drawing_area_new ();
	gtk_container_add (GTK_CONTAINER (in_frame), nav->preview);
    g_signal_connect (G_OBJECT (nav->preview),
                      "expose_event",
                      G_CALLBACK (gtk_image_nav_expose_drawing_area),
                      nav);
	
	gtk_window_set_wmclass (GTK_WINDOW (nav), "", "gtkimagenav");
}

static void
gtk_image_nav_finalize (GObject *object)
{
	GtkImageNav *nav = GTK_IMAGE_NAV (object);
	if (nav->pixbuf)
	{
		g_object_unref (nav->pixbuf);
		nav->pixbuf = NULL;
	}
	
	/* Chain up. */
	G_OBJECT_CLASS (gtk_image_nav_parent_class)->finalize (object);
}

enum
{
    PROP_IMAGE_VIEW = 1
};

static void
gtk_image_nav_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
    GtkImageNav *nav = GTK_IMAGE_NAV (object);
    if (prop_id == PROP_IMAGE_VIEW)
    {
        nav->view = g_value_get_object (value);
        
        /* Give initial state to the widget. */
        gtk_image_nav_pixbuf_changed (nav);

        g_signal_connect_swapped (G_OBJECT (nav->view), "pixbuf_changed",
                                  G_CALLBACK (gtk_image_nav_pixbuf_changed),
                                  nav);
    }
    else
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gtk_image_nav_class_init (GtkImageNavClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	object_class->finalize = gtk_image_nav_finalize;
    object_class->set_property = gtk_image_nav_set_property;

    GParamSpec *pspec =
        g_param_spec_object ("view",
                             "Image View",
                             "Image View to navigate",
                             GTK_TYPE_IMAGE_VIEW,
                             G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
    g_object_class_install_property (object_class, PROP_IMAGE_VIEW, pspec);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    widget_class->key_press_event = gtk_image_nav_key_press;
    widget_class->motion_notify_event = gtk_image_nav_motion_notify;
    widget_class->realize = gtk_image_nav_realize;
    widget_class->unrealize = gtk_image_nav_unrealize;
}

/**
 * gtk_image_nav_new:
 * @view: a #GtkImageView.
 * @returns: a new #GtkImageNav showing the view.
 *
 * Creates a new #GtkImageNav for showing thumbnails of the view. The
 * default values are:
 *
 * <itemizedlist>
 *   <listitem>pixbuf : %NULL</listitem> 
 * </itemizedlist>
 **/
GtkWidget *
gtk_image_nav_new (GtkImageView *view)
{
	g_return_val_if_fail (view, NULL);
	
	gpointer data = g_object_new
        (GTK_TYPE_IMAGE_NAV,
         "type", GTK_WINDOW_POPUP,
         /* The window must be non-resizable, otherwise it will not
            respond correctly to size requests which shrinks it. */
         "resizable", FALSE,
         "view", view,
         NULL);
    return GTK_WIDGET (data);
}


/*************************************************************/
/***** Read-only properties **********************************/
/*************************************************************/
/**
 * gtk_image_nav_get_pixbuf:
 * @nav: a #GtkImageNav.
 * @returns: the pixbuf in the navigation area this image navigator
 *  shows, or %NULL if none exist.
 *
 * Returns the downscaled pixbuf of the views pixbuf that this
 * #GtkImageNav shows, or %NULL if that pixbuf has not been created
 * yet.
 *
 * The pixbuf is by default %NULL.
 **/
GdkPixbuf*
gtk_image_nav_get_pixbuf (GtkImageNav *nav)
{
    return nav->pixbuf;
}



/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/
void
gtk_image_nav_grab (GtkImageNav *nav)
{
    GtkWidget *preview = nav->preview;

	gtk_grab_add (preview);

	GdkCursor *cursor = gdk_cursor_new (GDK_FLEUR);
	int mask = (GDK_POINTER_MOTION_MASK
				| GDK_POINTER_MOTION_HINT_MASK
				| GDK_BUTTON_RELEASE_MASK
				| GDK_EXTENSION_EVENTS_ALL);
    GdkWindow *window = gtk_widget_get_window (preview);
	gdk_pointer_grab (window, TRUE, mask, window, cursor, 0);
	gdk_cursor_unref (cursor);

	/* Capture keyboard events. */
	gdk_keyboard_grab (window, TRUE, GDK_CURRENT_TIME);
	gtk_widget_grab_focus (preview);
}

void
gtk_image_nav_release (GtkImageNav *nav)
{
	gdk_pointer_ungrab (GDK_CURRENT_TIME);
	
	/* Release keyboard focus. */
	gdk_keyboard_ungrab (GDK_CURRENT_TIME);
	gtk_grab_remove (nav->preview);
}



/*************************************************************/
/***** Runner function ***************************************/
/*************************************************************/
/**
 * gtk_image_nav_show_and_grab:
 * @nav: a #GtkImageNav
 * @center_x: x coordinate of center position
 * @center_y: y coordinate of center position
 *
 * Show the #GtkImageNav centered around the point (@center_x,
 * @center_y) and grab mouse and keyboard events. The grab continues
 * until a button release event is received which causes the widget to
 * hide.
 **/
void
gtk_image_nav_show_and_grab (GtkImageNav *nav,
                             int          center_x,
                             int          center_y)
{
    nav->center_x = center_x;
    nav->center_y = center_y;
    gtk_image_nav_update_position (nav);

    if (nav->update_when_shown)
        gtk_image_nav_update_pixbuf (nav);
    
    /* Connect signals and run! */
    gtk_widget_show_all (GTK_WIDGET (nav));
    gtk_image_nav_grab (nav);

    g_signal_connect (G_OBJECT (nav), "button-release-event", 
                      G_CALLBACK (gtk_image_nav_button_released), NULL);
    g_signal_connect_swapped (G_OBJECT (nav->view), "zoom_changed",
                              G_CALLBACK (gtk_image_nav_zoom_changed), nav);
}

