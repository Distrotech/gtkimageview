/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*-
 *
 * This file tests the GtkImageToolDragger class.
 **/
#include <src/gtkimagetooldragger.h>
#include <assert.h>

static GtkImageView *view = NULL;
// Use two global variables to avoid castings.
static GtkIImageTool *tool = NULL;
static GtkImageToolDragger *dragger = NULL;

static void
setup ()
{
    view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    g_object_ref_sink (view);

    tool = gtk_image_tool_dragger_new (view);
    dragger = GTK_IMAGE_TOOL_DRAGGER (tool);
    gtk_image_view_set_tool (view, tool);
}

static void
teardown ()
{
    g_object_unref (dragger);
    gtk_widget_destroy (GTK_WIDGET (view));
    g_object_unref (view);
}


/**
 * test_cursor_at_point_on_unrealized_view:
 *
 * Ensure that the cursor returned is %NULL if the view is not
 * realized.
 **/
static void
test_cursor_at_point_on_unrealized_view ()
{
    printf ("test_cursor_at_point_on_unrealized_view\n");
    setup ();
    GdkCursor *cursor = gtk_iimage_tool_cursor_at_point (tool, 10, 10);
    assert (!cursor);
    teardown ();
}

int
main (int   argc,
      char *argv[])
{
    gtk_init (&argc, &argv);
    test_cursor_at_point_on_unrealized_view ();
    printf ("1 test passed.\n");
}
