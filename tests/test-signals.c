/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * This file tests the various user-defined signals that GtkImageView
 * emits.
 *
 * Each of the following actions may trigger a zoom-changed signal:
 *
 * 1. Calling gtk_image_view_set_zoom ()
 * 2. Calling gtk_image_view_set_fitting ()
 * 3. Calling gtk_image_view_zoom_in/out ()
 * 4. Calling gtk_image_view_set_pixbuf ()
 * 5. Sending a size-allocate event.
 **/
#include <src/gtkimageview.h>
#include <src/gtkzooms.h>
#include <assert.h>
#include "testlib/testlib.h"

static int num_calls = 0;

static gdouble gotten_zoom = -1;
static void
zoom_changed_cb (GtkImageView *view)
{
    gotten_zoom = gtk_image_view_get_zoom (view);
    num_calls++;
}

static GdkScrollDirection gotten_dir = -1;
static void
mouse_wheel_scroll_cb (GtkImageView       *view,
					   GdkScrollDirection  dir)
{
	gotten_dir = dir;
	num_calls++;
}

static void
pixbuf_changed_cb (GtkImageView *view)
{
    num_calls++;
}

/**
 * test_zoom_changed_emitted_by_setting_zoom:
 *
 * The objective of this test is to verify that the zoom-changed
 * signal is emitted whenever gtk_image_view_set_zoom() is called with
 * a zoom not equal to the current zoom.
 *
 **/
static void
test_zoom_changed_emitted_by_setting_zoom ()
{
    printf ("test_zoom_changed_emitted_by_setting_zoom\n");
    GtkWidget *view = GTK_WIDGET (gtk_image_view_new ());
    g_object_ref_sink (view);

    g_signal_connect (G_OBJECT (view), "zoom_changed",
                      G_CALLBACK (zoom_changed_cb), NULL);

    gdouble zooms[] = {3.0, 0.55, 1.2, 0.01};
    for (int n = 0; n < G_N_ELEMENTS (zooms); n++)
    {
        num_calls = 0;
        gtk_image_view_set_zoom (GTK_IMAGE_VIEW (view), zooms[n]);

        gdouble expected_zoom = gtk_zooms_clamp_zoom (zooms[n]);
        assert (num_calls == 1);
        assert (gotten_zoom == expected_zoom);
    }

    gtk_widget_destroy (view);
    g_object_unref (view);
}

/**
 * test_not_zoom_changed_emitted_by_setting_equal_zoom
 *
 * The objective of this test is to verify that the zoom-changed
 * signal is NOT emitted if gtk_image_view_set_zoom () is called with
 * a zoom EQUAL to the current zoom.
 **/
static void
test_not_zoom_changed_emitted_by_setting_equal_zoom ()
{
    printf ("test_not_zoom_changed_emitted_by_setting_equal_zoom\n");
    GtkWidget *view = GTK_WIDGET (gtk_image_view_new ());
    g_object_ref_sink (view);

    g_signal_connect (G_OBJECT (view), "zoom_changed",
                      G_CALLBACK (zoom_changed_cb), NULL);

    gdouble zooms[] = {3.0, 0.55, 1.2, 0.01};
    num_calls = 0;
    for (int n = 0; n < G_N_ELEMENTS (zooms); n++)
    {
        gtk_image_view_set_zoom (GTK_IMAGE_VIEW (view), zooms[n]);
        gtk_image_view_set_zoom (GTK_IMAGE_VIEW (view), zooms[n]);
        gtk_image_view_set_zoom (GTK_IMAGE_VIEW (view), zooms[n]);
        /* Signal is only emitted once per loop. */
        assert (num_calls == n + 1);
    }

    gtk_widget_destroy (view);
    g_object_unref (view);
}

/**
 * test_zoom_changed_emitted_by_size_allocate:
 *
 * The objective of this test is to verify that the zoom-changed
 * signal is emitted when size allocate causes the zoom to change and
 * that the zoom-changed signal is NOT emitted when size allocate does
 * NOT cause the zoom.
 **/
static void
test_zoom_changed_emitted_by_size_allocate ()
{
    printf ("test_zoom_changed_emitted_by_size_allocate\n");
    GtkWidget *view = GTK_WIDGET (gtk_image_view_new ());
    g_object_ref_sink (view);

    GtkAllocation alloc = {0, 0, 200, 200};
    gtk_widget_size_allocate (view, &alloc);

    GdkPixbuf *pixbuf1 = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE,
                                         8, 500, 500);
    GdkPixbuf *pixbuf2 = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE,
                                         8, 50, 50);
    
    g_signal_connect (G_OBJECT (view), "zoom_changed",
                      G_CALLBACK (zoom_changed_cb), NULL);

    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf1, TRUE);
    
    num_calls = 0;
    gtk_widget_size_allocate (view, &alloc);
    assert (num_calls == 1);

    /* Recalcing the zoom again does not cause zoom-changed to be
       emitted. */
    gtk_widget_size_allocate (view, &alloc);
    assert (num_calls == 1);
    
    g_object_unref (pixbuf1);
    g_object_unref (pixbuf2);

    gtk_widget_destroy (view);
    g_object_unref (view);
}

/**
 * test_scroll_event_emits_mouse_wheel_scroll:
 *
 * The objective of this test is to verify that the mouse_wheel_scroll
 * signal is emitted when the widget receives a scroll event whose
 * direction is either GDK_SCROLL_UP or GDK_SCROLL_DOWN and whoes
 * state flags does not include GDK_CONTROL_MASK.
 **/
static void
test_scroll_event_emits_mouse_wheel_scroll ()
{
	printf ("test_scroll_event_emits_mouse_wheel_scroll\n");
	GtkWidget *view = gtk_image_view_new ();
    g_object_ref_sink (view);

	g_signal_connect (G_OBJECT (view), "mouse_wheel_scroll",
					  G_CALLBACK (mouse_wheel_scroll_cb), NULL);

	
	GdkEventScroll ev;
	gboolean retval;

	/* Should cause mouse_wheel_scroll -- state is 0. */
	ev.direction = GDK_SCROLL_DOWN;
	ev.state = 0;
	num_calls = 0;
	g_signal_emit_by_name (view, "scroll-event", &ev, &retval);
	assert (gotten_dir == GDK_SCROLL_DOWN);
	assert (num_calls == 1);

	/* Should not cause mouse_wheel_scroll. */
	ev.state = GDK_CONTROL_MASK;
	num_calls = 0;
	g_signal_emit_by_name (view, "scroll-event", &ev, &retval);
	assert (num_calls == 0);
	
	gtk_widget_destroy (view);
    g_object_unref (view);
}

/**
 * test_scroll_event_updates_zoom:
 *
 * The objective of this test is to verify that the zoom is updated
 * when the mouse wheel is scrolled while ctrl is pressed.
 **/
 
static void
test_scroll_event_updates_zoom ()
{
	printf ("test_scroll_event_updates_zoom\n");
	GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);

	assert (gtk_image_view_get_zoom (view) == 1.0);

	GdkEventScroll ev;
	gboolean retval;
	
	/* Test zoom in. */
	ev.direction = GDK_SCROLL_UP;
	ev.state = GDK_CONTROL_MASK;
	g_signal_emit_by_name (view, "scroll-event", &ev, &retval);
	assert (gtk_image_view_get_zoom (view) > 1.0);

	gtk_image_view_set_zoom (view, 1.0);

	/* Test zoom out. */
	ev.direction = GDK_SCROLL_DOWN;
	g_signal_emit_by_name (view, "scroll-event", &ev, &retval);
	assert (gtk_image_view_get_zoom (view) < 1.0);

	gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
}

/**
 * test_pixbuf_changed_emitted_by_setting_pixbuf:
 *
 * The objective of this test is to verify that the pixbuf-changed
 * signal is emitted whenever gtk_image_view_set_pixbuf() is called.
 **/
static void
test_pixbuf_changed_emitted_by_setting_pixbuf ()
{
    printf ("test_pixbuf_changed_emitted_by_setting_pixbuf\n");
    GtkWidget *view = GTK_WIDGET (gtk_image_view_new ());
    g_object_ref_sink (view);

    g_signal_connect (G_OBJECT (view), "pixbuf-changed",
                      G_CALLBACK (pixbuf_changed_cb), NULL);

    /* We set the same pixbuf five times, five signals are emitted. */
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 5, 5);
    for (int n = 0; n < 5; n++)
        gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, TRUE);
    assert (num_calls == 5);

    g_object_unref (pixbuf);
    gtk_widget_destroy (view);
    g_object_unref (view);
}

/**
 * test_set_null_scroll_adjustments:
 *
 * The objective of this test is to verify that setting GtkImageView:s
 * scroll adjustments to NULL works as expected.
 *
 * A GtkScrolledWindow sets its childs scroll adjustments to NULL when
 * it removes it, that is why GtkImageView must handle it.
 **/
static void
test_set_null_scroll_adjustments ()
{
    printf ("test_set_null_scroll_adjustments\n");
    GtkWidget *view = GTK_WIDGET (gtk_image_view_new ());
    g_object_ref_sink (view);

    gtk_widget_set_scroll_adjustments (view, NULL, NULL);

    gtk_widget_destroy (view);
    g_object_unref (view);
}


/**
 * test_scroll_handle_dragging:
 *
 * The objective of this test is to verify that scrolling does not
 * jump. The test simulates a user dragging a scroll handle
 * downwards. It passes if the viewport always moves downwards. If it
 * moves upwards it means that there is a jumping effect.
 **/
static void
test_scroll_handle_dragging ()
{
    printf ("test_scroll_handle_dragging\n");
    GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);
    
    fake_realize (GTK_WIDGET (view));
    GtkAllocation alloc = {0, 0, 200, 200};
	gtk_widget_size_allocate (GTK_WIDGET (view), &alloc);

    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
										500, 500);
    GtkAdjustment *hadj = GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 0,
                                                              0, 0, 0));
    GtkAdjustment *vadj = GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 0,
                                                              0, 0, 0));
    g_object_ref_sink (hadj);
    g_object_ref_sink (vadj);

    gtk_widget_set_scroll_adjustments (GTK_WIDGET (view), hadj, vadj);

    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
	gtk_image_view_set_zoom (view, 1.0);
    gtk_image_view_set_offset (view, 0, 0, FALSE);

    int oldy = 0;
    for (int n = 0; n < 50; n++)
    {
        gtk_adjustment_set_value (vadj, 5.0 * n);
        gtk_adjustment_changed (vadj);

        if (!g_main_context_wait_for_event (NULL, 1000 * 1000))
            break;
        g_main_context_iteration (NULL, TRUE);
        GdkRectangle viewport;
        gtk_image_view_get_viewport (view, &viewport);

        assert (viewport.y >= oldy);
        oldy = viewport.y;
    }

    g_object_unref (hadj);
    g_object_unref (vadj);
    g_object_unref (pixbuf);
    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_zoom_changed_emitted_by_setting_zoom ();
    test_not_zoom_changed_emitted_by_setting_equal_zoom ();
    test_zoom_changed_emitted_by_size_allocate ();
    test_scroll_event_emits_mouse_wheel_scroll ();
    test_scroll_event_updates_zoom ();
    test_pixbuf_changed_emitted_by_setting_pixbuf ();
    test_set_null_scroll_adjustments ();
    test_scroll_handle_dragging ();
    printf ("8 tests passed.\n");
}
