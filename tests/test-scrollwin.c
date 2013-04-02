/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*-
 *
 * This file tests the GtkImageScrollWin class.
 **/
#include <src/gtkimagescrollwin.h>
#include <assert.h>
#include "testlib/testlib.h"

static GtkImageView *view = NULL;
static GtkImageScrollWin *scroll_win = NULL;
static GdkPixbuf *pixbuf = NULL;

static void
setup ()
{
    view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref (view);
    gtk_object_sink (GTK_OBJECT (view));

    scroll_win = GTK_IMAGE_SCROLL_WIN (gtk_image_scroll_win_new (view));
    g_object_ref (scroll_win);
    gtk_object_sink (GTK_OBJECT (scroll_win));

    pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 100, 100);
}

static void
teardown ()
{
    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);

    gtk_widget_destroy (GTK_WIDGET (scroll_win));
    g_object_unref (scroll_win);

    g_object_unref (pixbuf);
}

/**
 * test_scrollbars_visibility:
 *
 * The objective of this test is to verify that the scrollbars aren't
 * shown when the image clearly fits inside the allocation and that
 * they are shown when the image clearly does not fit.
 **/
static void
test_scrollbars_visibility ()
{
    printf ("test_scrollbars_visibility\n");
    setup ();

    GTK_WIDGET (scroll_win)->allocation = (GdkRectangle){0, 0, 200, 200};
    fake_realize (GTK_WIDGET (scroll_win));

    GTK_WIDGET (view)->allocation = (GdkRectangle){0, 0, 200, 200};
    fake_realize (GTK_WIDGET (view));

    // The 100x100 pixbuf is smaller than the 200x200 allocation, so
    // don't show scrollbars.
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    assert (!GTK_WIDGET_VISIBLE (scroll_win->hscroll));
    assert (!GTK_WIDGET_VISIBLE (scroll_win->vscroll));

    // Zoomed width and height is 3 * 100 == 300 which is greater than
    // 200, so the scrollbars should be shown.
    gtk_image_view_set_zoom (view, 3.0);

    assert (GTK_WIDGET_VISIBLE (scroll_win->hscroll));
    assert (GTK_WIDGET_VISIBLE (scroll_win->vscroll));
    
    teardown ();
}

/**
 * test_scrollbars_hide_when_zooming_out:
 *
 * The objective of this test is to verify that the scrollbars aren't
 * shown if the view is zoomed out so that it exactly fits inside its
 * allocation.
 *
 * For example if the image width is 100, zoom 1.0 and allocation
 * width 55, then the scrollbars are shown, 100 * 1.0 > 55. If the
 * image is then zoomed out to 0.5 zoom, the scrollbars shouldn't be
 * shown, 100 * 0.5 < 55.
 **/
static void
test_scrollbars_hide_when_zooming_out ()
{
    printf ("test_scrollbars_hide_when_zooming_out\n");
    setup ();

    GTK_WIDGET (scroll_win)->allocation = (GdkRectangle){0, 0, 105, 105};
    fake_realize (GTK_WIDGET (scroll_win));

    GTK_WIDGET (view)->allocation = (GdkRectangle){0, 0, 105, 105};
    fake_realize (GTK_WIDGET (view));

    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
    // 105 > 100, so no scrollbars
    assert (!GTK_WIDGET_VISIBLE (scroll_win->hscroll));
    assert (!GTK_WIDGET_VISIBLE (scroll_win->vscroll));

    gtk_image_view_zoom_in (view);
    // 105 < 100 * 1.5, so show scrollbars. Due to the scrollbars, the
    // views allocation changes! The scrollbars are roughly 15 pixels
    // in width and height. In real code, the zoom in action would
    // automatically cause the allocation to be recalculated, but I
    // don't know how to simulate that.
    GTK_WIDGET (view)->allocation.width -= 15;
    GTK_WIDGET (view)->allocation.height -= 15;
    assert (GTK_WIDGET_VISIBLE (scroll_win->hscroll));
    assert (GTK_WIDGET_VISIBLE (scroll_win->vscroll));

    gtk_image_view_zoom_out (view);
    // 105 > 100, so no scrollbars. Same here as above, the zoom out
    // should cause the view to be resized.
    GTK_WIDGET (view)->allocation.width += 15;
    GTK_WIDGET (view)->allocation.height += 15;
    assert (!GTK_WIDGET_VISIBLE (scroll_win->hscroll));
    assert (!GTK_WIDGET_VISIBLE (scroll_win->vscroll));

    teardown();
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_scrollbars_visibility ();
    test_scrollbars_hide_when_zooming_out ();
    printf ("2 test passed.\n");
}
