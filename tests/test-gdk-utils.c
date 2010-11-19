/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; coding: utf-8 -*- */
/**
 * This file contains tests for the extra GDK functions defined in
 * utils.h.
 **/
#include <src/utils.h>
#include <gtk/gtk.h>

#include <assert.h>

static void
rects_around_rect_checker (GdkRectangle outer,
						   GdkRectangle inner)
{
    GdkRectangle borders[4];
	gdk_rectangle_get_rects_around (&outer, &inner, borders);

	GdkRectangle top = borders[0];
	assert (top.x == outer.x);
	assert (top.y == outer.y);
	assert (top.width == outer.width);
	assert (top.height == inner.y - outer.y);

	GdkRectangle left = borders[1];
    assert (left.x == 0);
    assert (left.y == inner.y);
    assert (left.width == inner.x - outer.x);
    assert (left.height == inner.height);

	GdkRectangle right = borders[2];
    assert (right.x == inner.x + inner.width);
    assert (right.y == inner.y);
    assert (right.width ==
			(outer.x + outer.width) - (inner.x + inner.width));
    assert (right.height == inner.height);

    GdkRectangle bottom = borders[3];
    assert (bottom.x == outer.x);
    assert (bottom.y == inner.y + inner.height);
    assert (bottom.width == outer.width);
    assert (bottom.height ==
			(outer.y + outer.height) - (inner.y + inner.height));
}

/**
 * test_get_rects_around_rect
 *
 * Test that getting the four bordering #GdkRectangle:s around a
 * rectangle works as expected.
 **/
static void
test_get_rects_around_rect ()
{
    printf ("test_get_rects_around_rect\n");
	GdkRectangle outsides[] = {
		{0, 0, 600, 400},
		{0, 0, 10, 10}
	};
	GdkRectangle insides[] = {
		{40, 40, 100, 100}
	};

    for (int i = 0; i < G_N_ELEMENTS(outsides); i++)
        for (int j = 0; j < G_N_ELEMENTS(insides); j++)
			rects_around_rect_checker (outsides[i], insides[j]);
			
    // Do manual simple sanity checks
	GdkRectangle out = {0, 0, 100, 100};
    GdkRectangle in = {25, 25, 50, 50};
    GdkRectangle arounds[4];
    gdk_rectangle_get_rects_around (&out, &in, arounds);
	assert (gdk_rectangle_eq2 (arounds[0], 0, 0, 100, 25));
	assert (gdk_rectangle_eq2 (arounds[1], 0, 25, 25, 50));
	assert (gdk_rectangle_eq2 (arounds[2], 75, 25, 25, 50));
	assert (gdk_rectangle_eq2 (arounds[3], 0, 75, 100, 25));
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_get_rects_around_rect ();
    printf ("1 test passed.\n");
}


