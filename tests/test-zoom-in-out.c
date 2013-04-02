/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*-
 *
 * This file contains tests that validates the correctness of various
 * aspects of zooming operations.
 ***/
#include <src/gtkimageview.h>
#include <src/gtkzooms.h>
#include <assert.h>
#include "testlib/testlib.h"

static GtkImageView *view = NULL;

static void
setup ()
{
    view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref (view);
    gtk_object_sink (GTK_OBJECT (view));
}

static void
teardown ()
{
    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
}

/**
 * test_zooming_unsets_fitting:
 *
 * Test that when a zooming operation (other than a fitting) is
 * performed, the fitting attribute is reset to %FALSE.
 **/
static void
test_zooming_unsets_fitting ()
{
    printf ("test_zooming_unsets_fitting\n");
    setup ();
    gtk_image_view_set_fitting (view, TRUE);
    gtk_image_view_zoom_in (view);
    assert (!gtk_image_view_get_fitting (view));

    gtk_image_view_set_fitting (view, TRUE);
    gtk_image_view_zoom_out (view);
    assert (!gtk_image_view_get_fitting (view));

    gtk_image_view_set_fitting (view, TRUE);
    gtk_image_view_set_zoom (view, 33.0);
    assert (!gtk_image_view_get_fitting (view));
    teardown();
}

/**
 * test_equal_zoom_unsets_fitting:
 *
 * The objective of this test is to verify that setting a zoom
 * identical to the last zoom unsets the fit mode.
 **/
static void
test_equal_zoom_unsets_fitting ()
{
	printf ("test_equal_zoom_unsets_fitting\n");
    setup ();
	gtk_image_view_set_zoom (view, 5.0);
	gtk_image_view_set_fitting (view, TRUE);
	gtk_image_view_set_zoom (view, 5.0);
	assert (!gtk_image_view_get_fitting (view));
    teardown ();
}

/**
 * test_boundary_zoom_values
 *
 * Test that to small and to big zoom factors are clamped.
 **/
static void
test_boundary_zoom_values ()
{
    printf ("test_boundary_zoom_values\n");
    setup ();
    gdouble small_zooms[] = {-99.0, -12324.0, 0.0, 0.001, 0.019, 0.009};
    for (int n = 0; n < G_N_ELEMENTS(small_zooms); n++)
    {
        gtk_image_view_set_zoom (view, small_zooms[n]);
        assert (gtk_image_view_get_zoom (view) == gtk_zooms_get_min_zoom ());
    }

    gdouble big_zooms[] = {333.0, 1231231.0, 100.01, 99999999.0};
    for (int n = 0; n < G_N_ELEMENTS(big_zooms); n++)
    {
        gtk_image_view_set_zoom (view, big_zooms[n]);
        assert (gtk_image_view_get_zoom (view) == gtk_zooms_get_max_zoom ());
    }
    teardown();
}

/**
 * test_zooming_in_and_out:
 *
 * Test that zooming in and out sets the zoom to the expected
 * value. Test that the zoom is clamped at the minimum and maximum
 * zoom values.
 **/
static void
test_zooming_in_and_out ()
{
    printf ("test_zooming_in_and_out\n");
    setup ();
    gdouble zoom_ins[] = {1.5, 2.0, 3.0, 5.0, 7.5, 10.0, 15.0, 20.0, 30.0,
                          50.0, 75.0, 100.0};
    for (int n = 0; n < G_N_ELEMENTS(zoom_ins); n++)
    {
        gtk_image_view_zoom_in (view);
        assert (gtk_image_view_get_zoom (view) == zoom_ins[n]);
    }

    // Zooming in more should not change the zoom.
    gdouble max_zoom = gtk_image_view_get_zoom (view);
    for (int n = 0; n < 10; n++)
    {
        gtk_image_view_zoom_in (view);
        assert (gtk_image_view_get_zoom (view) == max_zoom);
    }

    gtk_image_view_set_zoom (view, 1.0);
    gdouble zoom_outs[] = {
        0.75, 0.5, 0.3, 0.2, 0.15, 0.10, 0.07, 0.05, 0.02
    };
    for (int n = 0; n < G_N_ELEMENTS(zoom_outs); n++)
    {
        gtk_image_view_zoom_out (view);
        assert (gtk_image_view_get_zoom (view) == zoom_outs[n]);
    }

    // Zooming out more should not change the zoom.
    gdouble min_zoom = gtk_image_view_get_zoom (view);
    for (int n = 0; n < 10; n++)
    {
        gtk_image_view_zoom_out (view);
        assert (gtk_image_view_get_zoom (view) == min_zoom);
    }
    teardown ();
}

/**
 * test_same_zoom_keeps_offset:
 *
 * The objective of this test is to verify that setting the same zoom
 * as the current zoom does not change the viewport.
 *
 * This bug was caused by an integer division problem in
 * gtkimageview.c. That is why the allocation must have an odd width
 * and height.
 **/
static void
test_same_zoom_keeps_offset ()
{
    printf ("test_same_zoom_keeps_offset\n");
    setup ();
    fake_realize (GTK_WIDGET (view));

    GtkAllocation alloc = {0, 0, 585, 293};
    GTK_WIDGET (view)->allocation = alloc;
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                        400, 301);
    
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    gdouble the_zoom = gtk_zooms_get_max_zoom ();
    
    gtk_image_view_set_zoom (view, the_zoom);
    GdkRectangle viewport1;
    gtk_image_view_get_viewport (view, &viewport1);

    for (int n = 0; n < 10; n++)
    {
        gtk_image_view_set_zoom (view, the_zoom);
        GdkRectangle viewport2;
        gtk_image_view_get_viewport (view, &viewport2);

        assert (gdk_rectangle_eq (viewport1, viewport2));
    }

    g_object_unref (pixbuf);
    teardown ();
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_zooming_in_and_out ();
	test_equal_zoom_unsets_fitting ();
    test_boundary_zoom_values ();
    test_zooming_unsets_fitting ();
    test_same_zoom_keeps_offset ();
    printf ("5 test passed.\n");
}

