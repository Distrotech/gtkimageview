/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * This file tests GTK signals like expose_event on GtkImageView.
 **/
#include <src/gtkimageview.h>
#include <assert.h>
#include "testlib/testlib.h"

/**
 * test_expose_event_with_pixbuf:
 *
 * The objective of this test is simply to verify that GtkImageView
 * doesn't crash when it is repainting a pixbuf.
 **/
static void
test_expose_event_with_pixbuf ()
{
    printf ("test_expose_event_with_pixbuf\n");
    GtkWidget *view = gtk_image_view_new ();
    g_object_ref_sink (view);
    GdkPixbuf *pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 5, 5);

    fake_realize (view);

    GtkAllocation alloc;
    gtk_widget_get_allocation (view, &alloc);
    alloc.width = 100;
    alloc.height = 100;
    gtk_widget_set_allocation (view, &alloc);
    
    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, FALSE);

    GdkEventExpose ev = {.area = {0, 0, 999, 999}};
    GTK_WIDGET_GET_CLASS (view)->expose_event (view, &ev);

    g_object_unref (pixbuf);
    gtk_widget_destroy (view);
    g_object_unref (view);
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_expose_event_with_pixbuf ();
    printf ("1 test passed.\n");
}

