/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */

#include <src/gtkimagescrollwin.h>
#include <src/gtkimagetoolselector.h>
#include <src/gtkimageview.h>

GtkImageView *view = NULL;
GtkIImageTool *selector = NULL;

static void
sel_changed_cb (GtkImageToolSelector *selector)
{
    GdkRectangle sel;
    gtk_image_tool_selector_get_selection (selector, &sel);
    printf ("* Selection is %s\n", gdk_rectangle_to_str (sel));
}

int
main (int argc, char *argv[])
{
    printf ("This program demonstrates how to monitor the selection\n"
            "rectangle of a image selector tool.\n");
    gtk_init (&argc, &argv);
    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size (GTK_WINDOW (window), 500, 300);

    view = GTK_IMAGE_VIEW (gtk_image_view_new ());
    gtk_image_view_set_interpolation (view, GDK_INTERP_NEAREST);
    char *fname = "tests/gnome_logo.jpg";
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (fname, NULL);
    gtk_image_view_set_pixbuf (view, pixbuf, TRUE);

    selector = gtk_image_tool_selector_new (view);
    gtk_image_view_set_tool (view, selector);
    
    GdkRectangle sel_rect = {10, 10, 30, 20};
    gtk_image_tool_selector_set_selection (GTK_IMAGE_TOOL_SELECTOR (selector),
                                           &sel_rect);

    g_signal_connect (G_OBJECT (selector), "selection_changed",
                      G_CALLBACK (sel_changed_cb), NULL);

    GtkWidget *scroll_win = gtk_image_scroll_win_new (view);
    gtk_container_add (GTK_CONTAINER (window), scroll_win);
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (gtk_main_quit), NULL);

    gtk_widget_show_all (window);
    gtk_main ();
}
