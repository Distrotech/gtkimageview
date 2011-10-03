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
 * SECTION:gtkimagescrollwin
 * @see_also: <ulink url =
 * "http://developer.gnome.org/doc/API/2.0/gtk/GtkScrolledWindow.html">GtkScrolledWindow</ulink>,
 * the GTK widget that GtkImageScrollWin mimics.
 * @short_description: Scrollable window suitable for #GtkImageView
 *
 * <para>
 *   #GtkImageScrollWin provides a widget similar in appearance to
 *   GtkScrollableWindow that is more suitable for displaying
 *   #GtkImageView's.
 * </para>
 * <refsect2>
 *   <title>WTF!</title>
 *   <para>
 *     Moo
 *   </para>
 * </refsect2>
 **/
#include <glib/gi18n.h>
#include "gtkimagescrollwin.h"
#include "gtkimagenav.h"

/*************************************************************/
/***** PRIVATE DATA ******************************************/
/*************************************************************/
#ifdef __GNUC__
static const guint8 nav_button[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 nav_button[] = 
#endif
{
    ""
    /* Pixbuf magic (0x47646b50) */
    "GdkP"
    /* length: header (24) + pixel_data (784) */
    "\0\0\3("
    /* pixdata_type (0x1010002) */
    "\1\1\0\2"
    /* rowstride (56) */
    "\0\0\0""8"
    /* width (14) */
    "\0\0\0\16"
    /* height (14) */
    "\0\0\0\16"
    /* pixel_data: */
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\377"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\377\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\377"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377"
    "\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\377\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\377\0\0\0\377\0\0\0\0\0\0\0\377\0\0\0\377\0\0\0\377"
    "\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377"
    "\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377"
    "\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377"
    "\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\0\0\0\0\377\0\0\0\377"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\377\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\377\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377"
    "\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0"
    "\377\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\377\0\0\0\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0"
};

#ifdef __GNUC__
static const guint8 nav_button_hc[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 nav_button_hc[] = 
#endif
{
    ""
    /* Pixbuf magic (0x47646b50) */
    "GdkP"
    /* length: header (24) + pixel_data (784) */
    "\0\0\3("
    /* pixdata_type (0x1010002) */
    "\1\1\0\2"
    /* rowstride (56) */
    "\0\0\0""8"
    /* width (14) */
    "\0\0\0\16"
    /* height (14) */
    "\0\0\0\16"
    /* pixel_data: */
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0>>>\377>>>\377\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0>>>\377>>>\377>>>\377>>>\377\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0>>>\377>>>\377>>>\377>"
    ">>\377>>>\377>>>\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0>>>\377>>>\377\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0>>>\377\0\0\0\0\0\0\0\0\0\0"
    "\0\0>>>\377>>>\377\0\0\0\0\0\0\0\0\0\0\0\0>>>\377\0\0\0\0\0\0\0\0\0\0"
    "\0\0>>>\377>>>\377\0\0\0\0\0\0\0\0\0\0\0\0>>>\377>>>\377\0\0\0\0\0\0"
    "\0\0\0\0\0\0>>>\377>>>\377\0\0\0\0>>>\377>>>\377>>>\377>>>\377>>>\377"
    ">>>\377>>>\377>>>\377>>>\377>>>\377>>>\377>>>\377>>>\377>>>\377>>>\377"
    ">>>\377>>>\377>>>\377>>>\377>>>\377>>>\377>>>\377>>>\377>>>\377>>>\377"
    ">>>\377>>>\377>>>\377\0\0\0\0>>>\377>>>\377\0\0\0\0\0\0\0\0\0\0\0\0>"
    ">>\377>>>\377\0\0\0\0\0\0\0\0\0\0\0\0>>>\377>>>\377\0\0\0\0\0\0\0\0\0"
    "\0\0\0>>>\377\0\0\0\0\0\0\0\0\0\0\0\0>>>\377>>>\377\0\0\0\0\0\0\0\0\0"
    "\0\0\0>>>\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0>>>\377>>>\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0>>>\377>>>\377>>>\377>>>\377>>>\377"
    ">>>\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0>>>\377>>>\377>>>\377>>>\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
    "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0>>>\377>>>"
    "\377\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
};

G_DEFINE_TYPE(GtkImageScrollWin, gtk_image_scroll_win, GTK_TYPE_TABLE);

/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/
static void
gtk_image_scroll_win_adjustment_changed (GtkAdjustment     *adj,
                                         GtkImageScrollWin *window)
{
    GtkAdjustment *hadj, *vadj;
    hadj = gtk_range_get_adjustment (GTK_RANGE (window->hscroll));
    vadj = gtk_range_get_adjustment (GTK_RANGE (window->vscroll));

    /* We compare with the allocation size for the window instead of
       hadj->page_size and vadj->page_size. If the scrollbars are
       shown the views size is about 15 pixels shorter and thinner,
       which makes the page sizes inaccurate. The scroll windows
       allocation, on the other hand, always gives the correct number
       of pixels that COULD be shown if the scrollbars weren't
       there.
    */
    GtkAllocation alloc;
    gtk_widget_get_allocation (GTK_WIDGET (window), &alloc);

    gdouble h_upper = gtk_adjustment_get_upper (hadj);
    gdouble v_upper = gtk_adjustment_get_upper (vadj);
    
    gboolean hide_hscr = (h_upper <= alloc.width);
    gboolean hide_vscr = (v_upper <= alloc.height);

    if (hide_hscr && hide_vscr)
    {
        gtk_widget_hide (window->vscroll);
        gtk_widget_hide (window->hscroll);
        gtk_widget_hide (window->nav_box);
    }
    else
    {
        gtk_widget_show_now (window->vscroll);
        gtk_widget_show_now (window->hscroll);
        gtk_widget_show_now (window->nav_box);
    }
}

static void
gtk_image_scroll_win_nav_btn_clicked (GtkImageScrollWin *window,
									  GdkEventButton    *ev)
{
	gtk_image_nav_show_and_grab (GTK_IMAGE_NAV (window->nav),
								 ev->x_root, ev->y_root);
}

static void
gtk_image_scroll_win_enter_notify (GtkImageScrollWin *window,
                                   GdkEventCrossing  *ev)
{
    gtk_image_set_from_pixbuf (GTK_IMAGE (window->nav_image),
                               window->nav_button_hc);
}

static void
gtk_image_scroll_win_leave_notify (GtkImageScrollWin *window,
                                   GdkEventCrossing  *ev)
{
    gtk_image_set_from_pixbuf (GTK_IMAGE (window->nav_image),
                               window->nav_button);
}

static void
gtk_image_scroll_win_set_view (GtkImageScrollWin *window,
                               GtkImageView      *view)
{
    // Setup the scrollbars 
    GtkAdjustment *hadj;
    hadj = (GtkAdjustment *) g_object_new (GTK_TYPE_ADJUSTMENT, NULL);

    GtkAdjustment *vadj;
    vadj = (GtkAdjustment *) g_object_new (GTK_TYPE_ADJUSTMENT, NULL);

    window->hscroll = gtk_hscrollbar_new (hadj);
    window->vscroll = gtk_vscrollbar_new (vadj);

    // We want to be notified when the adjustments change. 
    g_signal_connect (hadj, "changed",
                      G_CALLBACK (gtk_image_scroll_win_adjustment_changed),
                      window);
    g_signal_connect (vadj, "changed",
                      G_CALLBACK (gtk_image_scroll_win_adjustment_changed),
                      window);

    // Output the adjustments to the widget. 
    gtk_widget_set_scroll_adjustments (GTK_WIDGET (view), hadj, vadj);

    // Add the widgets to the table.
    gtk_widget_push_composite_child ();
    gtk_table_attach (GTK_TABLE (window), GTK_WIDGET (view), 0, 1, 0, 1,
                      GTK_EXPAND | GTK_FILL, GTK_EXPAND | GTK_FILL,
                      0, 0);
    gtk_table_attach (GTK_TABLE (window), window->vscroll, 1, 2, 0, 1,
                      GTK_FILL, GTK_EXPAND | GTK_FILL,
                      0, 0);
    gtk_table_attach (GTK_TABLE (window), window->hscroll, 0, 1, 1, 2,
                      GTK_EXPAND | GTK_FILL, GTK_FILL,
                      0, 0);
    gtk_table_attach (GTK_TABLE (window), window->nav_box, 1, 2, 1, 2,
                      GTK_SHRINK, GTK_SHRINK,
                      0, 0);
    gtk_widget_pop_composite_child ();
    
	// Create the GtkImageNav popup.
	window->nav = gtk_image_nav_new (view);
}

/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/
/* The size request signal needs to be implemented and return two
   constant dummy values, otherwise an infinite loop may occur when
   GtkImageScrollWin is placed in a non-bounded container.

   When the scroll adjustments of GtkImageView changes,
   gtk_image_scroll_win_adjustment_changed will be called which may
   instruct GTK to show the scrollbars. When the scrollbars are shown
   the size has to be renegotiated. Because GtkImageScrollWin now
   shows the widgets which it didn't before, it will be allocated a
   bigger space.

   The bigger space allocation is propagated down to GtkImageView
   which updates its adjustments accordingly. Because of the bigger
   space, the scrollbars might not be needed
   anymore. gtk_image_scroll_win_adjustment_changed is invoked again
   which may hide the scrollbars. Because GtkImageScrollWin now hides
   widgets it previously showed, the size has to be
   renegotiated. GtkImageScrollWin finds out that the size is now to
   small so the scrollbars has to be shown after all.

   And so it continues.
 */
static void
gtk_image_scroll_win_size_request (GtkWidget      *widget,
                                   GtkRequisition *req)
{
    /* Chain up. */
    GTK_WIDGET_CLASS (gtk_image_scroll_win_parent_class)->size_request
        (widget, req);
    req->width = req->height = 80;
}

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
gtk_image_scroll_win_init (GtkImageScrollWin *window)
{
    window->hscroll = NULL;
    window->vscroll = NULL;
    window->nav_box = NULL;
    window->nav = NULL;

    // Setup the navigator button.
    window->nav_button =
        gdk_pixbuf_new_from_inline (-1, nav_button, FALSE, NULL);
    window->nav_button_hc =
        gdk_pixbuf_new_from_inline (-1, nav_button_hc, FALSE, NULL);
    window->nav_image = gtk_image_new_from_pixbuf (window->nav_button);
    
    window->nav_box = gtk_event_box_new ();
    gtk_container_add (GTK_CONTAINER (window->nav_box), window->nav_image);
    g_signal_connect_swapped (G_OBJECT (window->nav_box),
							  "button_press_event",
							  G_CALLBACK (gtk_image_scroll_win_nav_btn_clicked),
							  window);
    g_signal_connect_swapped (G_OBJECT (window->nav_box),
                              "enter_notify_event",
                              G_CALLBACK (gtk_image_scroll_win_enter_notify),
                              window);
    g_signal_connect_swapped (G_OBJECT (window->nav_box),
                              "leave_notify_event",
                              G_CALLBACK (gtk_image_scroll_win_leave_notify),
                              window);
    
    // Deprecated by 2.12, but to recent to change yet.
    gtk_tooltips_set_tip (gtk_tooltips_new (), window->nav_box,
                          _("Open the navigator window"), "");
}

static void
gtk_image_scroll_win_finalize (GObject *object)
{
    GtkImageScrollWin *window = GTK_IMAGE_SCROLL_WIN (object);
    g_object_unref (window->nav_button);
    g_object_unref (window->nav_button_hc);
	/* Maybe window->nav should be unreferenced here.. But uh I don't
	   know how. */
	gtk_widget_destroy (window->nav);
	
    /* Chain up. */
    G_OBJECT_CLASS (gtk_image_scroll_win_parent_class)->finalize (object);
}

enum
{
    PROP_IMAGE_VIEW = 1
};

static void
gtk_image_scroll_win_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
    GtkImageScrollWin *window = GTK_IMAGE_SCROLL_WIN (object);
    if (prop_id == PROP_IMAGE_VIEW)
        gtk_image_scroll_win_set_view (window, g_value_get_object (value));
    else
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gtk_image_scroll_win_class_init (GtkImageScrollWinClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = gtk_image_scroll_win_finalize;
    object_class->set_property = gtk_image_scroll_win_set_property;

    GParamSpec *pspec =
        g_param_spec_object ("view",
                             "Image View",
                             "Image View to navigate",
                             GTK_TYPE_IMAGE_VIEW,
                             G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
    g_object_class_install_property (object_class, PROP_IMAGE_VIEW, pspec);

    GtkWidgetClass *widget_class = (GtkWidgetClass *) klass;
    widget_class->size_request = gtk_image_scroll_win_size_request;
}

/**
 * gtk_image_scroll_win_new:
 * @view: a #GtkImageView to show.
 * @returns: A new #GtkImageScrollWin.
 *
 * Creates a new #GtkImageScrollWin containing the #GtkImageView.
 *
 * The widget is built using four subwidgets arranged inside a
 * #GtkTable with two columns and two rows. Two scrollbars, one
 * navigator button (the decorations) and one #GtkImageView.
 *
 * When the #GtkImageView fits inside the window, the decorations are
 * hidden.
 **/
GtkWidget *
gtk_image_scroll_win_new (GtkImageView *view)
{
    gpointer data = g_object_new (GTK_TYPE_IMAGE_SCROLL_WIN,
                                  "n-columns", 2,
                                  "n-rows", 2,
                                  "homogeneous", FALSE,
                                  "view", view,
                                  NULL);
    return GTK_WIDGET (data);
}
