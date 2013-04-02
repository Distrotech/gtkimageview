/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
/**
 * This file contains tests that validates that the fitting attribute
 * has the desired effect on the GtkImageView.
 **/
#include <src/gtkimageview.h>
#include <src/gtkzooms.h>

#include <assert.h>

static void
set_pixbuf_and_size_allocate (GtkImageView *view,
							  GdkPixbuf    *pixbuf,
							  int           width,
							  int           height)
{
	gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

	/* Zoom is not changed automatically because for these test cases,
	   the widget is not realized. */
	GtkAllocation alloc = {0, 0, width, height};
	gtk_widget_size_allocate (GTK_WIDGET (view), &alloc);
}

/**
 * test_fitting_on_smaller_images:
 *
 * Test fitting pixbufs that are smaller than the allocated size
 * inside a #GtkImageView.
 *
 * The objective of this test is to verify that the fitted pixbufs are
 * not scaled up. That is, the zoom should always be 1.
 **/
static void
test_fitting_on_smaller_images ()
{
	printf ("test_fitting_on_smaller_images\n");

	int allocated_sizes[] = {
		500, 500,
		100, 200,
		300, 100
	};
	int pixbuf_sizes[] = {
		100, 100,
		50, 50,
		10, 10,
		1, 1
	};
	
	GtkImageView *view = (GtkImageView *) gtk_image_view_new ();
	g_object_ref (view);
    gtk_object_sink (GTK_OBJECT (view));

	for (int i = 0; i < G_N_ELEMENTS (pixbuf_sizes); i += 2)
	{
		int pb_width = pixbuf_sizes[i];
		int pb_height = pixbuf_sizes[i + 1];
		GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
											pb_width, pb_height);
		for (int j = 0; j < G_N_ELEMENTS (allocated_sizes); j += 2)
		{
			GtkWidget *widget = GTK_WIDGET (view);
			widget->allocation.width = allocated_sizes[j];
			widget->allocation.height = allocated_sizes[j + 1];

			/* To make sure the set_pixbuf() call actually updates the
			   zoom. */
			gtk_image_view_set_zoom (view, 22.0);

			set_pixbuf_and_size_allocate (view, pixbuf,
										  allocated_sizes[j],
										  allocated_sizes[j + 1]);

			assert (gtk_image_view_get_fitting (view));

			gdouble zoom = gtk_image_view_get_zoom (view);

			assert (zoom == 1.0);
		}
		g_object_unref (pixbuf);
	}
	gtk_widget_destroy (GTK_WIDGET (view));
	g_object_unref (view);
}

/**
 * test_fitting_fit_size_if_larger:
 *
 * Test fitting pixbufs with varying sizes into a GtkImageView with
 * varying sizes.
 *
 * The objective of this test is to ensure that the zoom is set so
 * that the output area exactly fits the whole image. Note that by
 * default, fitting == %TRUE and that is why no explicit
 * initialization of that property is made.
 *
 * Also ensure that fitting is %TRUE after
 * gtk_image_view_set_pixbuf().
 **/
static void
test_fitting_fit_size_if_larger ()
{
    printf ("test_fitting_fit_size_if_larger\n");
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
                                            
        for (int j = 0; j < G_N_ELEMENTS (allocated_sizes); j += 2)
        {
            GtkWidget *widget = (GtkWidget *) view;
            int alloc_width = allocated_sizes[j];
            int alloc_height = allocated_sizes[j + 1];
            widget->allocation.width = alloc_width;
            widget->allocation.height = alloc_height;

			set_pixbuf_and_size_allocate (view, pixbuf,
										  alloc_width, alloc_height);

			assert (gtk_image_view_get_fitting (view));

            gdouble width_ratio = (gdouble) alloc_width / img_width;
            gdouble height_ratio = (gdouble) alloc_height / img_height;

            gdouble expected_zoom = MIN (width_ratio, height_ratio);
            expected_zoom = CLAMP (expected_zoom,
                                   gtk_zooms_get_min_zoom (),
                                   1.0);

			gdouble real_zoom = gtk_image_view_get_zoom (view);
            assert (real_zoom == expected_zoom);

        }
        g_object_unref (pixbuf);
    }
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_fitting_on_smaller_images ();
	test_fitting_fit_size_if_larger ();
    printf ("2 test passed.\n");
}
