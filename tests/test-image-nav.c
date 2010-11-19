/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*-
 *
 * This file contains general tests of the #GtkImageNav widgets.
 **/
#include <src/gtkimagenav.h>
#include <assert.h>

static GtkImageView *view = NULL;
static GtkImageNav *nav = NULL;

static void
setup ()
{
    view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);
    nav = GTK_IMAGE_NAV (gtk_image_nav_new (view));
    g_object_ref_sink (nav);
}

static void
teardown ()
{
    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
    gtk_widget_destroy (GTK_WIDGET (nav));
    g_object_unref (nav);
}

/**
 * test_size_request_of_preview:
 *
 * The objective of this test is to verify that the size request given
 * to the preview widget is correctly set in accordance with the size
 * of the pixbuf of the #GtkImageView.
 **/
static void
test_size_request_of_preview ()
{
    printf ("test_size_request_of_preview\n");
    setup ();
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 5, 5);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    gdouble width_zoom = (gdouble)GTK_IMAGE_NAV_MAX_WIDTH / (gdouble)500;
    gdouble height_zoom = (gdouble)GTK_IMAGE_NAV_MAX_HEIGHT / (gdouble)500;
    gdouble zoom = MIN (width_zoom, height_zoom);

    int width, height;
    gtk_widget_get_size_request (nav->preview, &width, &height);
    assert (width == (int)((gdouble)500 * zoom));
    assert (height == (int)((gdouble)500 * zoom));

    g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_null_values:
 *
 * The objective of this test is to verify that creating a
 * #GtkImageNav fails if the view argument is %NULL.
 **/
static void
test_null_values ()
{
    printf ("test_null_values (one error message expected)\n");
    GtkImageNav *nav = GTK_IMAGE_NAV (gtk_image_nav_new (NULL));
    assert (!nav);
}

/**
 * test_nav_showing_empty_view:
 *
 * The objective of this test is to verify that a #GtkImageNav can be
 * set to show a view with a %NULL pixbuf and that the size of it is
 * %GTK_IMAGE_NAV_MAX_WIDTH, %GTK_IMAGE_NAV_MAX_HEIGHT.
 **/
static void
test_nav_showing_empty_view ()
{
	printf ("test_nav_showing_empty_view\n");
    setup ();
	int width, height;
	gtk_widget_get_size_request (nav->preview, &width, &height);
	assert (width == GTK_IMAGE_NAV_MAX_WIDTH);
	assert (height == GTK_IMAGE_NAV_MAX_HEIGHT);
    teardown ();
}

/**
 * test_size_change_when_pixbuf_change:
 *
 * The objective of this test is to verify that the size of the
 * #GtkImageNav is updated when the pixbuf of the view is changed.
 **/
static void
test_size_change_when_pixbuf_change ()
{
	printf ("test_size_change_when_pixbuf_change\n");
    setup ();

	/* Load a pixbuf, ensure that size changes. */
	GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                        500, 500);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

	gdouble width_zoom = (gdouble)GTK_IMAGE_NAV_MAX_WIDTH / (gdouble)500;
    gdouble height_zoom = (gdouble)GTK_IMAGE_NAV_MAX_HEIGHT / (gdouble)500;
    gdouble zoom = MIN (width_zoom, height_zoom);
    int width, height;
    gtk_widget_get_size_request (nav->preview, &width, &height);
    assert (width == (int)((gdouble)500 * zoom));
    assert (height == (int)((gdouble)500 * zoom));

	g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_zoom_change_signal:
 *
 * The objective of this test is to verify that #GtkImageNav does not
 * act on the zoom-changed signal unless it is realized.
 **/
static void
test_zoom_change_signal ()
{
    printf ("test_zoom_change_signal\n");
    setup ();
    assert (gdk_rectangle_eq (nav->last_rect,
                              (GdkRectangle){-1, -1, -1, -1}));
    
    g_signal_emit_by_name (view, "zoom-changed");
    assert (gdk_rectangle_eq (nav->last_rect,
                              (GdkRectangle){-1, -1, -1, -1}));
    teardown ();
}

/**
 * test_pos_change_when_pixbuf_change:
 *
 * The objective of this test is to verify that the position of the
 * nav window is correctly updated, but still centered at the
 * specified position, when the pixbuf changes.
 **/
static void
test_pos_change_when_pixbuf_change ()
{
    printf ("test_pos_change_when_pixbuf_change\n");
    setup ();
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 50, 50);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
    gtk_image_nav_show_and_grab (nav, 200, 200);

    int x, y;
    
    gdouble nav_zoom = GTK_IMAGE_NAV_MAX_HEIGHT / 50.0;
    int nav_width = (int) (50 * nav_zoom + 0.5);
    int nav_height = (int) (50 * nav_zoom + 0.5);
    gtk_window_get_position (GTK_WINDOW (nav), &x, &y);
    assert (x == 200 - (nav_width / 2));
    assert (y == 200 - (nav_height / 2));

    GdkPixbuf *pixbuf2 = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                         100, 50);
    gtk_image_view_set_pixbuf (view, pixbuf2, TRUE);
    
    nav_zoom = GTK_IMAGE_NAV_MAX_WIDTH / 100.0;
    nav_width = (int) (100 * nav_zoom + 0.5);
    nav_height = (int) (50 * nav_zoom + 0.5);

    gtk_window_get_position (GTK_WINDOW (nav), &x, &y);

    assert (x == 200 - (nav_width / 2));
    assert (y == 200 - (nav_height / 2));

    g_object_unref (pixbuf2);
    g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_rmb_release:
 *
 * The objective of this test is to verify that a right mouse button
 * release event does not hide the nav.
 **/
static void
test_rmb_release ()
{
    printf ("test_rmb_release\n");
    setup ();

    gtk_image_nav_show_and_grab (nav, 100, 100);
    GdkEventButton ev_rel = {
        .type = GDK_BUTTON_RELEASE,
        .window = gtk_widget_get_window (GTK_WIDGET (nav)),
        .button = 3
    };
    gboolean retval;
    g_signal_emit_by_name (nav, "button-release-event", &ev_rel, &retval);
    assert (gtk_widget_get_visible (GTK_WIDGET (nav)));
    
    teardown ();
}

/**
 * test_lmb_release:
 *
 * The objective of this test is to verify that left mouse button
 * release event *does* hide the nav.
 **/
static void
test_lmb_release ()
{
    printf ("test_lmb_release\n");
    setup ();

    gtk_image_nav_show_and_grab (nav, 100, 100);
    GdkEventButton ev_rel = {
        .type = GDK_BUTTON_RELEASE,
        .window = gtk_widget_get_window (GTK_WIDGET (nav)),
        .button = 1
    };
    gboolean retval;
    g_signal_emit_by_name (nav, "button-release-event", &ev_rel, &retval);
    assert (!gtk_widget_get_visible (GTK_WIDGET (nav)));
    
    teardown ();
}

/**
 * The objective of this test is to verify that the nav does not
 * create a downsampled version of the pixbuf the view shows before it
 * is shown.
 **/
static void
test_delayed_scaling ()
{
    printf ("test_delayed_scaling\n");
    setup ();
    
    assert (!gtk_image_nav_get_pixbuf (nav));

    // Trigger the pixbuf-changed signal. This should not cause the
    // navigator to scale the pixbuf.
    GdkPixbuf *pixbuf2 = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                         100, 50);
    gtk_image_view_set_pixbuf (view, pixbuf2, TRUE);
    assert (!gtk_image_nav_get_pixbuf (nav));

    // But it should be created when we show the navigator ofcourse.
    gtk_image_nav_show_and_grab (nav, 100, 100);

    teardown ();
}

/**
 * The objective of this test is to verify that the navigator window
 * is not resizable.
 **/
static void
test_not_resizable ()
{
    printf ("test_not_resizable\n");
    setup ();
    assert (!gtk_window_get_resizable (GTK_WINDOW (nav)));
    teardown ();
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
	test_size_request_of_preview ();
    test_null_values ();
	test_nav_showing_empty_view ();
	test_size_change_when_pixbuf_change ();
    test_zoom_change_signal ();
    test_pos_change_when_pixbuf_change ();
    test_rmb_release ();
    test_lmb_release ();
    test_delayed_scaling ();
    test_not_resizable ();
    printf ("10 tests passed.\n");
}
