/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*-
 *
 * This file tests getting and setting attributes in GtkImageView.
 **/
#include <src/gtkimagetoolselector.h>
#include <src/gtkimageview.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void
test_get_type ()
{
    printf ("test_get_type\n");
    assert (gtk_image_view_get_type ());
}

/**
 * test_default_state:
 *
 * Test that all default attributes have their correct default values.
 **/
static void
test_default_state ()
{
    printf ("test_default_state\n");
    GtkImageView *view = (GtkImageView *) gtk_image_view_new ();
    g_object_ref_sink (view);

    assert (!gtk_image_view_get_black_bg (view));
    assert (gtk_image_view_get_fitting (view));
    int interp = gtk_image_view_get_interpolation (view);
    assert (interp == GDK_INTERP_BILINEAR);

    /* Since no image is loaded, viewport won't be changed. */
    GdkRectangle viewport = {1, 2, 3, 4};
    gboolean is_active = gtk_image_view_get_viewport (view, &viewport);
    assert (viewport.x == 1);
    assert (viewport.y == 2);
    assert (!is_active);

    assert (!gtk_image_view_get_pixbuf (view));
    assert (gtk_image_view_get_show_cursor (view));
    assert (gtk_image_view_get_show_frame (view));
    assert (gtk_image_view_get_zoom (view) == (gdouble) 1.0);
    
    g_object_unref (view);
}

static void
test_change_zoom ()
{
    printf ("test_change_zoom\n");
    GtkImageView *view = (GtkImageView *) gtk_image_view_new ();
    gdouble legal_zooms[] = {0.1, 0.5, 1.0, 1.1, 1.33, 2.0, 20.0};
    for (int n = 0; n < G_N_ELEMENTS(legal_zooms); n++)
    {
        gdouble zoom = legal_zooms[n];
        gtk_image_view_set_zoom (view, zoom);
        assert (gtk_image_view_get_zoom (view) == zoom);
    }
}

static void
test_get_set_non_null_pixbuf ()
{
    printf ("test_get_set_non_null_pixbuf\n");
    GtkImageView *view = (GtkImageView *) gtk_image_view_new ();
    g_object_ref_sink (view);

    int dimensions[] = {10, 200, 300, 1000, 9999};
    for (int n = 0; n < G_N_ELEMENTS(dimensions); n++)
    {
        int dim = dimensions[n];
        GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                            dim, dim);
        gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
        assert (pixbuf == gtk_image_view_get_pixbuf (view));
        g_object_unref (pixbuf);
    }

	gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
}

/**
 * test_get_set_null_pixbuf
 *
 * The objective of this test is to verify that GtkImageView handles a
 * NULL pixbuf correctly.
 **/
static void
test_get_set_null_pixbuf ()
{
	printf ("test_get_set_null_pixbuf\n");
	GtkImageView *view = (GtkImageView *) gtk_image_view_new ();
    g_object_ref_sink (view);
	
    gtk_image_view_set_pixbuf (view, NULL, TRUE);
    assert (!gtk_image_view_get_pixbuf (view));

    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
}

static void
test_get_set_fitting ()
{
    printf ("test_get_set_fitting\n");
    GtkImageView *view = (GtkImageView *) gtk_image_view_new ();
    gboolean modes[] = {FALSE, TRUE};
    for (int n = 0; n < G_N_ELEMENTS(modes); n++)
    {
        gtk_image_view_set_fitting (view, modes[n]);
        assert (gtk_image_view_get_fitting (view) == modes[n]);
    }
}

/* /\** */
/*  * test_that_set_pixbuf_sets_fit_mode: */
/*  * */
/*  * Ensure that when a new pixbuf is loaded, fit_mode is set to */
/*  * GTK_FIT_SIZE_IF_LARGER. */
/*  * */
/*  * When viewing an image, the user might zoom in on something. But */
/*  * when a new image is loaded, the fit_mode is reset to */
/*  * GTK_FIT_SIZE_IF_LARGER so that the whole image is displayed in the */
/*  * widget. */
/*  **\/ */
/* static void */
/* test_that_set_pixbuf_sets_fit_mode () */
/* { */
/* 	printf ("test_that_set_pixbuf_sets_fit_mode\n"); */
/* 	GtkImageView *view = (GtkImageView *) gtk_image_view_new (); */
/* 	GtkFitMode modes[] = { */
/*         GTK_FIT_NONE, */
/*         GTK_FIT_SIZE_IF_LARGER */
/*     }; */
/* 	for (int n = 0; n < G_N_ELEMENTS(modes); n++) */
/* 	{ */
/* 		gtk_image_view_set_fit_mode (view, modes[n]); */
/* 		gtk_image_view_set_pixbuf (view, NULL); */
/* 		assert (gtk_image_view_get_fit_mode (view) == GTK_FIT_SIZE_IF_LARGER); */
/* 	} */
/* } */

/**
 * test_set_pixbuf_with_new_sets_fitting:
 *
 * The objective of this test is to verify that when
 * gtk_image_view_set_pixbuf() is called, with a #GdkPixbuf reference
 * that is not equal to the current pixbuf, fitting is set to %TRUE.
 *
 * When viewing an image, the user might zoom in on something. But
 * when a new image is loaded, the fitting is (normally) reset to
 * %TRUE so that the whole image is displayed in the widget.
 **/
static void
test_set_pixbuf_with_new_sets_fitting ()
{
	printf ("test_set_pixbuf_with_new_sets_fitting\n");
	GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);
	GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 5, 5);

	gboolean modes[] = {FALSE, TRUE};
	for (int n = 0; n < G_N_ELEMENTS (modes); n++)
	{
		gtk_image_view_set_pixbuf (view, NULL, TRUE);
		gtk_image_view_set_fitting (view, modes[n]);
		gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
		assert (gtk_image_view_get_fitting (view)); 
	}
	
	g_object_unref (pixbuf);
	gtk_widget_destroy (GTK_WIDGET (view));
	g_object_unref (view);
}

/**
 * test_set_old_pixbuf_sets_fitting
 *
 * The objective of this test is to verify that when
 * gtk_image_view_set_pixbuf() is called, with a #GdkPixbuf reference
 * that is equal to the current pixbuf, fitting is set to %TRUE.
 **/
static void
test_set_old_pixbuf_sets_fitting ()
{
	printf ("test_set_old_pixbuf_sets_fitting\n");

	GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);

	gtk_image_view_set_fitting (view, FALSE);

	/* Do change fitting. */
	gtk_image_view_set_pixbuf (view, NULL, TRUE);
    assert (gtk_image_view_get_fitting (view));

	gtk_widget_destroy (GTK_WIDGET (view));
	g_object_unref (view);
}

/**
 * test_gtk_flags
 *
 * Ensure that the correct GTK flags is set on the widget.
 **/
static void
test_gtk_flags ()
{
	printf ("test_gtk_flags\n");
	GtkImageView *view = (GtkImageView *) gtk_image_view_new ();

    assert (gtk_widget_get_can_focus (GTK_WIDGET (view)));
    assert (gtk_widget_get_double_buffered (GTK_WIDGET (view)));
}

static void
test_get_set_black_bg ()
{
	printf ("test_get_set_bg\n");
	GtkImageView *view = (GtkImageView *) gtk_image_view_new ();
	gboolean values[] = {TRUE, FALSE};
	for (int n = 0; n < G_N_ELEMENTS(values); n++)
	{
		gtk_image_view_set_black_bg (view, values[n]);
		assert (gtk_image_view_get_black_bg (view) == values[n]);
	}
}

/**
 * test_set_same_pixbuf:
 *
 * The objective of this test is to verify that setting the same
 * pixbuf as the one currently used works correctly.
 **/
static void
test_set_same_pixbuf ()
{
    printf ("test_set_same_pixbuf\n");
    
    GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);

    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 9, 9);

    /* Ownership of the pixbuf is transferred to the view. */
    gtk_image_view_set_pixbuf (view, pixbuf, FALSE);
    g_object_unref (pixbuf);

    /* Only the views reference remains. */
    int ref_count = G_OBJECT (pixbuf)->ref_count;
    assert (ref_count == 1);
    
    for (int n = 0; n < 10; n++)
    {
        GdkPixbuf *old = gtk_image_view_get_pixbuf (view);
        gtk_image_view_set_pixbuf (view, old, FALSE);
    }

    gtk_widget_destroy (GTK_WIDGET (view));
	g_object_unref (view);
}

/**
 * test_set_tool_before_realize:
 *
 * The objective of this test is to verify that setting the tool of
 * the view before realizing the view works correctly.
 **/
static void
test_set_tool_before_realize ()
{
    printf ("test_set_tool_before_realize\n");
    GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
	g_object_ref (view);
	gtk_object_sink (GTK_OBJECT (view));

    GtkIImageTool *tool = gtk_image_tool_selector_new (view);
    gtk_image_view_set_tool (view, tool);

    g_object_unref (tool);
    gtk_widget_destroy (GTK_WIDGET (view));
	g_object_unref (view);
}

/**
 * test_library_version:
 *
 * The objective of this test is to verify that
 * gtk_image_view_library_version () returns the correct thing.
 **/
static void
test_library_version ()
{
    printf ("test_library_version\n");
    const char *version = gtk_image_view_library_version ();
    assert (version);

    char *p = (char *)version;
    for (int n = 0; n < 3; n++)
    {
        int part = atoi(p);
        assert (part >= 0);
        p = strchr(p, '.');
        p++;
    }
    assert (!--p);
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_get_type ();
    test_default_state ();
    test_change_zoom ();
    test_get_set_non_null_pixbuf ();
	test_get_set_null_pixbuf ();
    test_get_set_fitting ();
    test_set_pixbuf_with_new_sets_fitting ();
	test_set_old_pixbuf_sets_fitting ();
	test_gtk_flags ();
	test_get_set_black_bg ();
    test_set_same_pixbuf ();
    test_set_tool_before_realize ();
    test_library_version ();
    printf ("13 tests passed.\n");
}
