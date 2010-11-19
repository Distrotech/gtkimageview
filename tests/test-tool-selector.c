/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*-
 *
 * This file tests the GtkImageToolSelector class.
 **/
#include <src/gtkimagetoolselector.h>
#include <assert.h>
#include "testlib/testlib.h"

static GtkImageView *view = NULL;
// Use two global variables to avoid castings.
static GtkIImageTool *tool = NULL;
static GtkImageToolSelector *selector = NULL;

static void
setup ()
{
    view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);
    fake_realize (GTK_WIDGET (view));

    tool = gtk_image_tool_selector_new (view);
    selector = GTK_IMAGE_TOOL_SELECTOR (tool);
    gtk_image_view_set_tool (view, tool);
}

static void
teardown ()
{
    g_object_unref (selector);
    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
}

/**
 * test_null_image_view:
 *
 * Ensure that an error is printed if the view argument to the
 * constructor is NULL.
 **/
static void
test_null_image_view ()
{
    printf ("test_null_image_view (one error message expected)\n");
    GtkIImageTool *tool = gtk_image_tool_selector_new (NULL);
    assert (!tool);
}

/**
 * test_default_selection:
 *
 * Ensure that the default selection rectangle is (0, 0), (0, 0).
 **/
static void
test_default_selection ()
{
    printf ("test_default_selection\n");
    setup ();
    GdkRectangle sel;
    gtk_image_tool_selector_get_selection (selector, &sel);
    assert (gdk_rectangle_eq (sel, (GdkRectangle){0, 0, 0, 0}));
    teardown ();
}

/**
 * test_tool_ref_count:
 *
 * Ensure that the tool has the correct number of references.
 **/
static void
test_tool_ref_count ()
{
    printf ("test_tool_ref_count\n");
    setup ();
    
    // The selector has an implied reference and the view
    assert (G_OBJECT (selector)->ref_count == 2);
    
    teardown ();
}

/**
 * test_get_set_selection:
 *
 * Ensure that getting and setting the selection rectangle works when
 * the view shows a pixbuf.
 **/
static void
test_get_set_selection ()
{
    printf ("test_get_set_selection\n");
    setup ();
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 50, 50);
    gtk_image_view_set_pixbuf (view, pixbuf, FALSE);
    GdkRectangle sel = {10, 11, 12, 13};
    gtk_image_tool_selector_set_selection (selector, &sel);
    GdkRectangle ret;
    gtk_image_tool_selector_get_selection (selector, &ret);
    assert (gdk_rectangle_eq (ret, (GdkRectangle){10, 11, 12, 13}));
    g_object_unref (pixbuf);
    teardown();
}

/**
 * test_set_selection_no_pixbuf:
 *
 * Ensure that setting the selection when there is no pixbuf in the
 * view works. The selection should not be modified.
 **/
static void
test_set_selection_no_pixbuf ()
{
    printf ("test_set_selection_no_pixbuf\n");
    setup ();
    GdkRectangle sel = {10, 11, 12, 13};
    gtk_image_view_set_pixbuf (view, NULL, FALSE);
    gtk_image_tool_selector_set_selection (selector, &sel);

    gtk_image_tool_selector_get_selection (selector, &sel);
    assert (sel.x == 0 && sel.y == 0 && sel.width == 0 && sel.height == 0);
    teardown ();
}

/**
 * test_set_selection_to_big:
 *
 * Ensure that setting a to big selection rectangle leaves the
 * selection rectangle untouched.
 **/
static void
test_set_selection_to_big ()
{
    printf ("test_selection_to_big\n");
    setup ();
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 20, 20);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    GdkRectangle good_sel = {0, 0, 10, 10};
    gtk_image_tool_selector_set_selection (selector, &good_sel);

    GdkRectangle bad_sel = {0, 0, 50, 50};
    gtk_image_tool_selector_set_selection (selector, &bad_sel);

    GdkRectangle curr_sel;
    gtk_image_tool_selector_get_selection (selector, &curr_sel);
    assert (gdk_rectangle_eq (curr_sel, (GdkRectangle){0, 0, 10, 10}));

    g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_set_selection_outside_pixbuf:
 *
 * Ensure that a selection rectangle outside the pixbuf is moved back
 * in.
 **/
static void
test_set_selection_outside_pixbuf ()
{
    printf ("test_set_selection_outside_pixbuf\n");
    setup ();
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 20, 20);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    GdkRectangle good_sel = {0, 0, 10, 10};
    gtk_image_tool_selector_set_selection (selector, &good_sel);
    
    GdkRectangle bad_sel = {15, 15, 10, 10};
    gtk_image_tool_selector_set_selection (selector, &bad_sel);

    GdkRectangle curr_sel;
    gtk_image_tool_selector_get_selection (selector, &curr_sel);
    assert (gdk_rectangle_eq (curr_sel, (GdkRectangle){10, 10, 10, 10}));
    
    g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_motion_notify:
 *
 * Ensure that the selector handles motion notifies.
 **/
static void
test_motion_notify ()
{
    printf ("test_lookup_cursors\n");
    setup ();
    GdkEventMotion ev = {.x = 10, .y = 10};
    assert (!gtk_iimage_tool_motion_notify (tool, &ev));
    teardown();
}

/**
 * test_motion_notify_in_selection:
 *
 * Ensure that the selector handles a motion notify occuring inside
 * the selection.
 **/
static void
test_motion_notify_in_selection ()
{
    printf ("test_motion_notify_in_selection\n");
    setup ();

    // Fake an allocation
    GtkAllocation alloc = {0, 0, 40, 40};
    gtk_widget_set_allocation (GTK_WIDGET (view), &alloc);

    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 20, 20);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    GdkRectangle sel_rect = {0, 0, 20, 20};
    gtk_image_tool_selector_set_selection (selector, &sel_rect);

    // Put the pointer inside the selection area.
    GdkEventMotion ev = {.x = 20, .y = 20};
    assert (!gtk_iimage_tool_motion_notify (tool, &ev));
    g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_set_selection_on_unrealized_view:
 *
 * The objective of this test is to verify that setting the selection
 * on a selector tool with an unrealized view works as expected.
 **/
static void
test_set_selection_on_unrealized_view ()
{
    printf ("test_set_selection_on_unrealized_view\n");
    GtkImageView *view2 = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view2);
    tool = gtk_image_tool_selector_new (view2);
    selector = GTK_IMAGE_TOOL_SELECTOR (tool);
    
    gtk_image_view_set_tool (view2, tool);
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 20, 20);
    gtk_image_view_set_pixbuf (view2, pixbuf, TRUE);

    GdkRectangle sel = {10, 10, 10, 10};

    gtk_image_tool_selector_set_selection (selector, &sel);
    
    g_object_unref (pixbuf);
    g_object_unref (selector);
    gtk_widget_destroy (GTK_WIDGET (view2));
    g_object_unref (view2);
}

/**
 * test_motion_notify_keeps_accuracy:
 *
 * Motion notify of #GtkImageToolSelector must remember the initial
 * zoom space position of the mouse pointer. Because the geometry of
 * the selection rectangle is stored in image space coordinates, it
 * means that multiple motion notify events that defines drags that
 * are shorter must be recorded somehow.
 *
 * The easiest way to accomplish that is to remember the position
 * where the drag began.
 **/
static void
test_motion_notify_keeps_accuracy ()
{
    printf ("test_motion_notify_keeps_accuracy\n");
    setup ();

    GtkAllocation alloc = {0, 0, 500, 500};
    gtk_widget_set_allocation (GTK_WIDGET (view), &alloc);

    GdkPixbuf *pixbuf =
        gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 100, 100);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
    gtk_image_view_set_zoom (view, 5.0);

    // Set the initial selection
    GdkRectangle sel;
    sel = (GdkRectangle){10, 10, 50, 50};
    gtk_image_tool_selector_set_selection (selector, &sel);

    // Fake drag the selection some
    GtkWidget *widget = GTK_WIDGET (view);
    GtkWidgetClass *wid_class = GTK_WIDGET_GET_CLASS (widget);
    GdkEventMotion motion_ev;

    // First, fake a button click in the selection at widget
    // coordinates (100, 100) which is inside the selection.
    GdkWindow *window = gtk_widget_get_window (GTK_WIDGET (view));
    GdkEventButton button_ev = {.button = 1,
                                .window = window,
                                .x = 100, .y = 100};
    wid_class->button_press_event (widget, &button_ev);

    // Then a motion event at (103, 105)
    motion_ev = (GdkEventMotion){.x = 103, .y = 105};
    wid_class->motion_notify_event (widget, &motion_ev);

    // Since 100 // 5 == 103 // 5, the selection should not move in
    // the x direction. But should move one pixel downwards in the y
    // direction.
    gtk_image_tool_selector_get_selection (selector, &sel);
    assert (sel.x == 10 && sel.y == 11);

    // The another mtoin event at (106, 110)
    motion_ev = (GdkEventMotion){.x = 106, .y = 110};
    wid_class->motion_notify_event (widget, &motion_ev);

    // 6 // 5 = 1 and 10 // 5 = 2, so the selection should now have
    // moved one pixel to the right and two pixels downwards from its
    // original position.
    gtk_image_tool_selector_get_selection (selector, &sel);
    assert (sel.x == 11 && sel.y == 12);
    
    g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_button_press_before_realize:
 *
 * Ensure that the selector can handle button press events even before
 * its view has been realized.
 **/
static void
test_button_press_before_realize ()
{
    printf ("test_button_press_before_realize\n");
    setup ();

    GtkAllocation alloc = {0, 0, 10, 10};
    gtk_widget_set_allocation (GTK_WIDGET (view), &alloc);
    GdkPixbuf *pixbuf =
        gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 10, 10);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
    GdkEventButton ev = {.button = 1,
                         .window = gtk_widget_get_window (GTK_WIDGET (view)),
                         .x = 5, .y = 5};
    gtk_iimage_tool_button_press (tool, &ev);
    g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_cursor_at_point_inside_selection:
 *
 * Ensure that the correct cursor is returned when
 * gtk_iimage_tool_cursor_at_point() is called with a position inside
 * the selection.
 **/
static void
test_cursor_at_point_inside_selection ()
{
    printf ("test_cursor_at_point_inside_selection\n");
    setup ();

    GtkAllocation alloc = {0, 0, 50, 50};
    gtk_widget_set_allocation (GTK_WIDGET (view), &alloc);
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 90, 90);
    gtk_image_view_set_pixbuf (view, pixbuf, FALSE);
    
    gtk_image_view_set_zoom (view, 1.0);
    GdkRectangle sel = {0, 0, 90, 90};
    gtk_image_tool_selector_set_selection (selector, &sel);
    GdkCursor *cursor = gtk_iimage_tool_cursor_at_point (tool, 0, 0);
    assert (cursor);

    g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_cursor_at_point_on_null_pixbuf:
 *
 * Ensure that the cursor returned is %NULL if there is no pixbuf in
 * the view.
 **/
static void
test_cursor_at_point_on_null_pixbuf ()
{
    printf ("test_cursor_at_point_on_null_pixbuf\n");
    setup ();
    GdkCursor *cursor = gtk_iimage_tool_cursor_at_point (tool, 10, 10);
    assert (!cursor);
    teardown ();
}

/**
 * test_cursor_outside_widget:
 *
 * Ensure that the cursor returned is %NULL when the coordinate is
 * outside the widget.
 **/
static void
test_cursor_outside_widget ()
{
    printf ("test_cursor_outside_widget\n");
    setup ();
    GtkAllocation alloc = {0, 0, 50, 50};
    gtk_widget_set_allocation (GTK_WIDGET (view), &alloc);
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 90, 90);
    gtk_image_view_set_pixbuf (view, pixbuf, FALSE);

    int coords[] = {
        250, 250,
        250, 25,
        -10, -10
    };
    for (int n = 0; n < G_N_ELEMENTS(coords); n += 2)
    {
        int x = coords[n];
        int y = coords[n + 1];
        GdkCursor *cursor = gtk_iimage_tool_cursor_at_point (tool, x, y);
        assert (!cursor);
    }
    g_object_unref (pixbuf);
    teardown ();
}

/**
 * test_selection_after_pixbuf_change:
 *
 * Ensure that the selection becomes (0,0)-[0,0] when the pixbuf is
 * changed if reset_fit is %TRUE.
 **/
static void
test_selection_after_pixbuf_change ()
{
    printf ("test_selection_after_pixbuf_change\n");
    setup ();
    GdkRectangle rect;
    
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 90, 90);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);
    gtk_image_tool_selector_get_selection (selector, &rect);
    assert (gdk_rectangle_eq (rect, (GdkRectangle){0, 0, 0, 0}));

    rect = (GdkRectangle){1, 2, 3, 4};
    gtk_image_tool_selector_set_selection (selector, &rect);
    gtk_image_view_set_pixbuf (view, NULL, TRUE);
    gtk_image_tool_selector_get_selection (selector, &rect);
    assert (gdk_rectangle_eq (rect, (GdkRectangle){0, 0, 0, 0}));

    g_object_unref (pixbuf);
    teardown ();
}

int
main (int   argc,
      char *argv[])
{
    gtk_init (&argc, &argv);
    test_null_image_view ();
    test_default_selection ();
    test_tool_ref_count ();
    test_get_set_selection ();
    test_set_selection_no_pixbuf ();
    test_set_selection_to_big ();
    test_set_selection_outside_pixbuf ();
    test_motion_notify ();
    test_motion_notify_in_selection ();
    test_set_selection_on_unrealized_view ();
    test_motion_notify_keeps_accuracy ();
    test_button_press_before_realize ();
    test_cursor_at_point_inside_selection ();
    test_cursor_at_point_on_null_pixbuf ();
    test_cursor_outside_widget ();
    test_selection_after_pixbuf_change ();
    printf ("16 tests passed.\n");
}
