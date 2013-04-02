/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
#include <glib.h>
#include <gtk/gtk.h>
#include <src/gtkanimview.h>
#include <src/gtkimagescrollwin.h>

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);

    if (argc != 2)
    {
        printf ("usage: ex-anim <image>\n");
        return 1;
    }

    GdkPixbufAnimation *anim = gdk_pixbuf_animation_new_from_file (argv[1],
                                                                   NULL);

    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    GtkAnimView *aview = GTK_ANIM_VIEW (gtk_anim_view_new ());
    gtk_anim_view_set_anim (aview, anim);

    GtkWidget *scroll = gtk_image_scroll_win_new (GTK_IMAGE_VIEW (aview));
    
    gtk_container_add (GTK_CONTAINER (window), scroll);
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (gtk_main_quit), NULL);

    gtk_widget_show_all (window);

    gtk_main ();
}
