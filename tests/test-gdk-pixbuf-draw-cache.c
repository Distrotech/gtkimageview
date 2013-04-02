/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
/**
 * This file contains tests that validates that the GdkPixbufDrawCache
 * class works correctly.
 **/
#include <assert.h>
#include <src/gtkimageview.h>

/**
 * test_only_scale_op_on_new_identical_pixbuf:
 *
 * The objective of this test is to verify that the cached pixbuf that
 * the GdkPixbufDrawCache holds is not discarded when it receives a new
 * pixbuf with the exact same bit depth and colorspace.
 **/
static void
test_only_scale_op_on_new_identical_pixbuf ()
{
    printf ("test_only_scale_op_on_new_identical_pixbuf\n");

    GdkPixbuf *pixbuf1 = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 9, 9);
    GdkPixbuf *pixbuf2 = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 9, 9);
    
    int interp = GDK_INTERP_BILINEAR;
    GdkRectangle area = {0, 0, 10, 10};
    
    GdkPixbufDrawOpts o1 = {1, area, 0, 0, interp, pixbuf1, 0, 0};
    GdkPixbufDrawOpts o2 = {1, area, 0, 0, interp, pixbuf2, 0, 0};

    GdkPixbufDrawMethod meth = gdk_pixbuf_draw_cache_get_method (&o1, &o2);
    assert (meth == GDK_PIXBUF_DRAW_METHOD_SCALE);

    g_object_unref (pixbuf1);
    g_object_unref (pixbuf2);
}

/**
 * test_cache_is_used_on_equal_opts:
 *
 * The objective of this test is to verify that the cache is used when
 * the new draw options are identical to the old ones if the area is
 * smaller than the cached pixbuf. If so, the #GdkPixbufDrawMethod
 * should be %GDK_PIXBUF_DRAW_METHOD_CONTAINS.
 **/
static void
test_cache_is_used_on_equal_opts ()
{
    printf ("test_cache_is_used_on_equal_opts\n");
    int interp = GDK_INTERP_BILINEAR;
    GdkRectangle area = {0, 0, 10, 10};
    GdkPixbuf *pb = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 30, 30);
    GdkPixbufDrawOpts o1 = {1, area, 0, 0, interp, pb, 0, 0};
    GdkPixbufDrawOpts o2 = {1, area, 0, 0, interp, pb, 0, 0};

    GdkPixbufDrawMethod meth = gdk_pixbuf_draw_cache_get_method (&o1, &o2);
    assert (meth == GDK_PIXBUF_DRAW_METHOD_CONTAINS);

    g_object_unref (pb);
}

/**
 * test_contains_even_if_cache_to_small:
 *
 * The objective of this test is to verfy that the method is
 * %GDK_PIXBUF_DRAW_METHOD_CONTAINS even if the cache is smaller than the
 * requsted draw area.
 **/
static void
test_contains_even_if_cache_to_small ()
{
    printf ("test_contains_even_if_cache_to_small\n");
    int interp = GDK_INTERP_BILINEAR;
    GdkRectangle area = {0, 0, 20, 20};
    
    GdkPixbuf *draw_pb = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE,
                                         8, 30, 30);
    GdkPixbufDrawOpts o1 = {1, area, 0, 0, interp, draw_pb, 0, 0};
    GdkPixbufDrawOpts o2 = {1, area, 0, 0, interp, draw_pb, 0, 0};

    GdkPixbufDrawMethod meth = gdk_pixbuf_draw_cache_get_method (&o1, &o2);
    assert (meth == GDK_PIXBUF_DRAW_METHOD_CONTAINS);
    g_object_unref (draw_pb);
}


/**
 * test_scroll_needed_if_rect_size_not_equal:
 *
 * The objective of this test is to verify that a scroll operation is
 * needed if the size of the rectangle changes.
 **/
static void
test_scroll_needed_if_rect_size_not_equal ()
{
    printf ("test_scroll_needed_if_rect_size_not_equal\n");
    int interp = GDK_INTERP_BILINEAR;
    GdkPixbuf *pb = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 9, 9);
    GdkPixbufDrawOpts o1 = {1, (GdkRectangle){0, 0, 20, 20},
                           0, 0, interp, pb, 0, 0};
    GdkPixbufDrawOpts o2 = {1, (GdkRectangle){20, 0, 10, 10},
                           0, 0, interp, pb, 0, 0};

    GdkPixbufDrawMethod meth = gdk_pixbuf_draw_cache_get_method (&o1, &o2);
    assert (meth == GDK_PIXBUF_DRAW_METHOD_SCROLL);

    g_object_unref (pb);
}

/**
 * test_default_draw_options:
 *
 * The objective of this test is to verify that the default draw
 * options in the pixbuf draw cache results in the correct draw
 * method.
 **/
static void
test_default_draw_options ()
{
    printf ("test_default_draw_options\n");
    GdkPixbufDrawCache *cache = gdk_pixbuf_draw_cache_new ();
    int interp = GDK_INTERP_BILINEAR;
    GdkPixbuf *pb = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 9, 9);
    GdkPixbufDrawOpts o = {1, (GdkRectangle){0, 0, 20, 20},
                           0, 0, interp, pb, 0, 0};
    GdkPixbufDrawMethod meth =
        gdk_pixbuf_draw_cache_get_method (&cache->old, &o);
    assert (meth == GDK_PIXBUF_DRAW_METHOD_SCALE);
    
    gdk_pixbuf_draw_cache_free (cache);
    g_object_unref (pb);
}

/**
 * test_invalidate:
 *
 * The objective of this test is to verify that forcing the
 * GdkPixbufDrawCache to apply a scale operation by invalidating works
 * as expected.
 **/
static void
test_invalidate ()
{
    printf ("test_invalidate\n");
    GdkPixbufDrawCache *cache = gdk_pixbuf_draw_cache_new ();
    
    GdkInterpType interp = GDK_INTERP_BILINEAR;
    GdkPixbuf *pb = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 9, 9);
    GdkPixbufDrawOpts opts = {1, (GdkRectangle){0, 0, 5, 5},
                              0, 0, interp, pb, 0, 0};
    cache->old = opts;


    /* GdkPixbufDrawOpts are identical, so no allocation or scale
       necessary. */
    assert (gdk_pixbuf_draw_cache_get_method (&cache->old, &opts)
            == GDK_PIXBUF_DRAW_METHOD_CONTAINS);

    /* Force a scale operation in the next draw. */
    gdk_pixbuf_draw_cache_invalidate (cache);

    assert (gdk_pixbuf_draw_cache_get_method (&cache->old, &opts) ==
            GDK_PIXBUF_DRAW_METHOD_SCALE);
    
    gdk_pixbuf_draw_cache_free (cache);
    g_object_unref (pb);
}

int
main(int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_only_scale_op_on_new_identical_pixbuf ();
    test_cache_is_used_on_equal_opts ();
    test_contains_even_if_cache_to_small ();
    test_scroll_needed_if_rect_size_not_equal ();
    test_default_draw_options ();
    test_invalidate ();
    printf ("6 tests passed.\n");
}
