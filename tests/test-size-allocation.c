/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * This file contains tests that validates that the size allocation
 * signal is handled properly and that the attributes of the
 * GtkImageView is updated like they should.
 **/
#include <src/gtkimageview.h>
#include <src/gtkzooms.h>
#include <assert.h>
#include "testlib/testlib.h"

/**
 * test_that_image_is_fitted_after_size_changes:
 *
 * Test that a pixbuf fitted before a size allocation remains fitted
 * after the size allocation has taken place.
 **/
static void
test_that_image_is_fitted_after_size_changes ()
{
    printf ("test_that_image_is_fitted_after_size_changes\n");
    int allocated_sizes[] = {
        500, 500,
        50, 50,
        12, 33,
        99, 70,
        249, 120,
        1517, 124,
        1, 1
    };
    int image_sizes[] = {
        50, 50,
        45, 99,
        33, 22,
        115, 317,
        1, 1
    };
    GtkImageView *view = (GtkImageView *) gtk_image_view_new ();
    for (int i = 0; i < G_N_ELEMENTS (image_sizes); i += 2)
    {
        int img_width = image_sizes[i];
        int img_height = image_sizes[i + 1];
        GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                            img_width, img_height);
        gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
        for (int j = 0; j < G_N_ELEMENTS (allocated_sizes); j += 2)
        {
            int alloc_width = allocated_sizes[j];
            int alloc_height = allocated_sizes[j + 1];
            GtkAllocation alloc = {0, 0, alloc_width, alloc_height};
            gtk_widget_size_allocate (GTK_WIDGET (view), &alloc);

            gdouble width_ratio = (gdouble) alloc_width / img_width;
            gdouble height_ratio = (gdouble) alloc_height / img_height;

            gdouble expected_zoom = MIN (width_ratio, height_ratio);
			expected_zoom = CLAMP (expected_zoom,
                                   gtk_zooms_get_min_zoom (),
                                   1.0);
	    
            assert (gtk_image_view_get_zoom (view) == expected_zoom);
            
        }
        g_object_unref (pixbuf);
    }
}

/**
 * test_size_allocate_at_image_boundary:
 *
 * The objective of this test is to verify that a size allocate event
 * that occurs when the image is scrolled to an edge, causes the image
 * offset to change.
 **/
static void
test_size_allocate_at_image_boundary ()
{
	printf ("test_size_allocate_at_image_boundary\n");
	
	GtkWidget *view = GTK_WIDGET (gtk_image_view_new ());
    g_object_ref_sink (view);

	fake_realize (view);
	
	GtkAllocation alloc = {0, 0, 200, 200};
	gtk_widget_size_allocate (view, &alloc);
	
	GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
										500, 500);

	gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, TRUE);
	gtk_image_view_set_zoom (GTK_IMAGE_VIEW (view), 1.0);

	/* Create the scroll adjustments, each has the range [0-500]. */
	GtkObject *hadj = gtk_adjustment_new (0.0, 0.0, 500.0,
										  20.0, 100.0, 200.0);
	GtkObject *vadj = gtk_adjustment_new (0.0, 0.0, 500.0,
										  20.0, 100.0, 200.0);
	gtk_widget_set_scroll_adjustments (view,
									   GTK_ADJUSTMENT (hadj),
									   GTK_ADJUSTMENT (vadj));

	/* Scroll the adjustment to the right edge of the image. */
	g_assert (hadj);
	gtk_adjustment_set_value (GTK_ADJUSTMENT (hadj), 300.0);

	
    GMainContext *main_ctx = g_main_context_default ();
    while (g_main_context_pending (main_ctx))
    {
        g_main_context_iteration (main_ctx, TRUE);
        g_usleep (1000 * 100);
    }

    /* The viewport should now match the rectangle (300, 0)-(200, 200) */
	GdkRectangle viewport;
	gtk_image_view_get_viewport (GTK_IMAGE_VIEW (view), &viewport);
    
    assert (gdk_rectangle_eq (viewport, (GdkRectangle){300, 0, 200, 200}));

	/* Now expand the widget by making its width 100 pixels
	   wider. This decreased the x-coordinate of the viewport by 100
	   and increased the width by the same amount. */
	alloc = (GtkAllocation){0, 0, 300, 200};
	gtk_widget_size_allocate (view, &alloc);

	gtk_image_view_get_viewport (GTK_IMAGE_VIEW (view), &viewport);

    assert (gdk_rectangle_eq (viewport, (GdkRectangle){200, 0, 300, 200}));
	
	g_object_unref (pixbuf);
	gtk_widget_destroy (view);
    g_object_unref (view);
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_that_image_is_fitted_after_size_changes ();
	test_size_allocate_at_image_boundary ();
    printf ("2 tests passed.\n");
}
