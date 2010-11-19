/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
#include <src/gtkimagescrollwin.h>
#include <src/gtkimageview.h>

int
main (int   argc,
      char *argv[])
{
    printf ("This program demonstrates how to put a GtkImageView\n"
            "inside an alignment widget.\n");

    gtk_init (&argc, &argv);
    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* Create the GtkImageView and set it to show an image. */
    GtkWidget *view = gtk_image_view_new ();
    char *fname = "tests/gnome_logo.jpg";
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (fname, NULL);
    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, TRUE);
    /* Make the background black to easier see the views borders. */
    gtk_image_view_set_black_bg (GTK_IMAGE_VIEW (view), TRUE);

    /* Create the GtkImageScrollWin to contain the view. */
    GtkWidget *scroll_win = gtk_image_scroll_win_new (GTK_IMAGE_VIEW (view));

    /* Create an alignment that contains the scroll win. */
    GtkWidget *alignment = gtk_alignment_new (0.5, 0.5, 0.5, 0.5);
    gtk_container_add (GTK_CONTAINER (alignment), scroll_win);

    gtk_container_add (GTK_CONTAINER (window), alignment);
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (gtk_main_quit), NULL);
    
    gtk_widget_show_all (window);
    gtk_main ();
}
