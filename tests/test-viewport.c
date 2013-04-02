/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * This file contains tests that validates that the viewport
 * #GtkImageView returns is always correct.
 **/
#include <src/gtkimageview.h>
#include <assert.h>

static GtkImageView *view = NULL;

/**
 * test_viewport_when_image_is_completely_visible:
 *
 * The objective of this test is to verify that the correct viewport
 * is returned when the image shown is completely visible inside the
 * widget.
 **/
static void
test_viewport_when_image_is_completely_visible ()
{
    printf ("test_viewport_when_image_is_completely_visible\n");

    GtkAllocation alloc = {0, 0, 100, 100};
    gtk_widget_size_allocate (GTK_WIDGET (view), &alloc);

    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                        10, 10);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    gdouble zooms[] = {0.1, 0.5, 1.0, 1.5, 2.0, 3.0, 5.0, 9.0};
    for (int n = 0; n < G_N_ELEMENTS (zooms); n++)
    {
        gtk_image_view_set_zoom (view, zooms[n]);
        gdouble the_zoom = gtk_image_view_get_zoom (view);

        GdkRectangle viewport;
        gtk_image_view_get_viewport (view, &viewport);
        GdkRectangle exp = {0, 0, 10 * the_zoom, 10 * the_zoom};
        assert (gdk_rectangle_eq (viewport, exp));
    }
    
    g_object_unref (pixbuf);
}

/**
 * test_return_value:
 *
 * The objective of this test is to verify that
 * gtk_image_view_get_viewport returns %TRUE when it shows an image
 * and %FALSE if it does not.
 **/
static void
test_return_value ()
{
    printf ("test_return_value\n");

    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                        500, 500);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    GdkRectangle dummy;
    assert (gtk_image_view_get_viewport (view, &dummy));
    gtk_image_view_set_pixbuf (view, NULL, TRUE);
    assert (!gtk_image_view_get_viewport (view, &dummy));
    
    g_object_unref (pixbuf);
}

/**
 * test_null_rect_argument:
 *
 * The objective of this test is to verify that feeding the
 * gtk_image_view_get_viewport function a %NULL rect argument works as
 * expected.
 **/
static void
test_null_rect_argument ()
{
    printf ("test_null_rect_argument\n");
    gtk_image_view_get_viewport (view, NULL);
}

/**
 * test_set_offset_unrealized:
 *
 * The objective of this test is to verify that setting the offset of
 * an unrealized GtkImageView with a non-NULL pixbuf works as
 * expected.
 **/
static void
test_set_offset_unrealized ()
{
    printf ("test_set_offset_unrealized\n");
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                        500, 500);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
    gtk_image_view_set_offset (view, 50, 50, FALSE);
    g_object_unref (pixbuf);
}

/**
 * test_set_offset_invaliding_unrealized:
 *
 * The objective of this test is to verify that setting the offset of
 * an allocated but unrealized GtkImageView with a non-NULL pixbuf
 * with invalidate = TRUE works as expected.
 **/
static void
test_set_offset_invaliding_allocated ()
{
    printf ("test_set_offset_invaliding_allocated\n");
    GTK_WIDGET (view)->window = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                        500, 500);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
    gtk_image_view_set_offset (view, 100, 100, TRUE);
    g_object_unref (pixbuf);
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref (view);
    gtk_object_sink (GTK_OBJECT (view));
    
    test_viewport_when_image_is_completely_visible ();
    test_return_value ();
    test_null_rect_argument ();
    test_set_offset_unrealized ();
    test_set_offset_invaliding_allocated ();
    printf ("5 tests passed.\n");

    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
}
