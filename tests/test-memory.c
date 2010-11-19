/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 * This file contains tests that stress the memory aspects of using
 * the GtkImageView.
 **/
#include <assert.h>
#include <src/gtkimageview.h>
#include <src/gtkimagescrollwin.h>
#include <src/gtkimagetoolselector.h>
#include "testlib/testlib.h"

/* Adjust this constant. It should be a high value, but not so high
  that the tests take an unreasonably large time to run. */
#define NUM_MEM_LOOPS    1000

/**
 * test_setting_the_pixbuf_multiple_times
 *
 * The objective of this test is to verify that
 * gtk_image_view_set_pixbuf correctly removes the reference to the
 * last #GdkPixbuf it had.
 *
 * The test is carried out by feeding the function the same pixbuf
 * multiple times. The refcount to the pixbuf should stay constant
 * between invocations.
 **/
static void
test_setting_the_pixbuf_multiple_times ()
{
    printf ("test_setting_the_pixbuf_multiple_times\n");
    GtkWidget *view = gtk_image_view_new ();
    g_object_ref_sink (view);

    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 9, 9);
    for (int n = 0; n < NUM_MEM_LOOPS; n++)
    {
        gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, TRUE);
        int ref_count = G_OBJECT (pixbuf)->ref_count;
        
        /* There should be two references to the object, view and
           pixbuf. */
        assert (ref_count == 2);
    }
    gtk_widget_destroy (view);
    g_object_unref (view);
    g_object_unref (pixbuf);
}

/**
 * test_unrefing_the_pixbuf:
 *
 * The objective of this test is to verify that
 * gtk_image_view_set_pixbuf correctly increases the reference count
 * to the #GdkPixbuf.
 *
 * Note that the GObject.ref_count attribute is "private." Well, that
 * is severly retarded because it makes testing refleaks that much
 * harder.
 **/
static void
test_unrefing_the_pixbuf ()
{
    printf ("test_unrefing_the_pixbuf\n");
    GtkWidget *view = gtk_image_view_new ();
    g_object_ref_sink (view);

    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 9, 9);

    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, TRUE);

    /* Unrefing the pixbuf should still keep the refcount to 1 because
       GtkImageView should hold a reference to it. */
    g_object_unref (pixbuf);
    int ref_count = G_OBJECT (pixbuf)->ref_count;
    assert (ref_count == 1);

    gtk_widget_destroy (view);
    g_object_unref (view);
}

/**
 * test_unreffing:
 *
 * The objective of this test is to verify allocating and deallocating
 * #GdkImageView does not leak any memory.
 *
 * Note that the process for referencing and unreferencing a
 * unrealized widget is quite contrived.
 **/
static void
test_unreffing ()
{
    printf ("test_unreffing\n");
    for (int n = 0; n < NUM_MEM_LOOPS; n++)
    {
        /* Allocate five instances simultaneously. This should make
           the test slightly harder because only the first allocation
           will cause the class to be allocated. */
        GtkWidget *views[5];
        for (int i = 0; i < G_N_ELEMENTS(views); i++)
        {
            views[i] = gtk_image_view_new ();
            g_object_ref_sink (views[i]);
        }
        /* Here, it works in the opposite way. Only the last
           deallocation frees the class itself. */
        for (int i = 0; i < G_N_ELEMENTS(views); i++)
        {
            gtk_widget_destroy (views[i]);
            g_object_unref (views[i]);
        }
    }
}

/**
 * test_unrefing_gtk_image_scroll_win:
 *
 * The objective of this test is to verify that allocating and
 * deallocating a GtkImageScrollWin does not produce any error
 * messages.
 **/
static void
test_unrefing_gtk_image_scroll_win ()
{
	printf ("test_unrefing_gtk_image_scroll_win\n");
	GtkWidget *view = gtk_image_view_new ();
    g_object_ref_sink (view);

	GtkWidget *window = gtk_image_scroll_win_new (GTK_IMAGE_VIEW (view));
    g_object_ref_sink (window);

	gtk_widget_destroy (window);
	g_object_unref (window);
	
	gtk_widget_destroy (view);
	g_object_unref (view);
}

/**
 * test_finalize_for_unrealized_attributes:
 *
 * The objective of this test is to verify that "finalize" does not
 * touch attributes that are allocated and deallocated in "realize"
 * and "unrealize".
 **/
static void
test_finalize_for_unrealized_attributes ()
{
	printf ("test_finalize_for_unrealized_attributes\n");
/* 	GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ()); */
/* 	g_object_ref (view); */
/* 	gtk_object_sink (GTK_OBJECT (view)); */

/* 	assert (!view->cursor); */
/* 	assert (!view->void_cursor); */
/* 	assert (!view->mouse_handler); */

/* 	/\* Set the attributes to bogus values, GTK will scream it if tries */
/* 	   to deallocate them. *\/ */
/* 	view->cursor = (void *)0x1234; */
/* 	view->void_cursor = (void *)0x1234; */
/* 	view->mouse_handler = (void *)0x1234; */

/* 	gtk_widget_destroy (GTK_WIDGET (view)); */
/* 	g_object_unref (view); */
}

/**
 * test_finalize_for_unrealized_attributes2:
 *
 * The objective of this test is to verify that "finalize" does not
 * touch attributes in #GtkImageNav that are allocated and deallocated
 * in "realize" and "unrealize".
 **/
static void
test_finalize_for_unrealized_attributes2 ()
{
	printf ("test_finalize_for_unrealized_attributes2\n");
	GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
	g_object_ref_sink (view);
	GtkImageNav *nav = GTK_IMAGE_NAV (gtk_image_nav_new (view));
	g_object_ref_sink (nav);

	/* See test_finalize_for_unrealized_attributes */
	nav->gc = (void *)0x1234;

	gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
    gtk_widget_destroy (GTK_WIDGET (nav));
    g_object_unref (nav);
}

/**
 * test_finalize_tool_selector:
 *
 * Ensure that finalizing the tool selector does not leak memory.
 **/
static void
test_finalize_tool_selector ()
{
    printf ("test_finalize_tool_selector\n");
    GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);

    for (int n = 0; n < NUM_MEM_LOOPS; n++)
    {
        GtkIImageTool *selector = gtk_image_tool_selector_new (view);
        g_object_unref (selector);
    }

    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
}

/**
 * test_offset_setting:
 *
 * Ensure that setting the offset multiple times does not leak memory.
 *
 * You have to run the iterations for a very long time before the leak
 * becomes noticable, but they were there!
 **/ 
static void
test_offset_setting ()
{
    printf ("test_offset_setting\n");
    GtkImageView *view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);
    fake_realize (GTK_WIDGET (view));

    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8,
                                        100, 100);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    for (int n = 0; n < NUM_MEM_LOOPS * 100; n++)
        gtk_image_view_set_offset (view, 40 + n % 2, 40, FALSE);

    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
    g_object_unref (pixbuf);
}

int
main (int argc, char *argv[])
{
    g_mem_set_vtable (glib_mem_profiler_table);
    gtk_init (&argc, &argv);
    test_unreffing ();
    test_unrefing_the_pixbuf ();
    test_setting_the_pixbuf_multiple_times ();
	test_unrefing_gtk_image_scroll_win ();
	test_finalize_for_unrealized_attributes ();
	test_finalize_for_unrealized_attributes2 ();
    test_finalize_tool_selector ();
    test_offset_setting ();
    printf ("8 test passed.\n");
}
