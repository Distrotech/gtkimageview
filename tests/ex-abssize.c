/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
#include <src/gtkimagescrollwin.h>
#include <src/gtkanimview.h>

int
main (int   argc,
      char *argv[])
{
    printf ("This program demonstrates how to resize a window according\n"
            "to the size of the image.\n");
    
    char **filenames = NULL;
    GOptionEntry options[] = {
        {
            G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY,
            &filenames, NULL, "[FILE...]"
        },
        {NULL}
    };
    GOptionContext *ctx = g_option_context_new ("ex-abssize");
    g_option_context_add_main_entries (ctx, options, "moo");
    g_option_context_parse (ctx, &argc, &argv, NULL);
    g_option_context_free (ctx);
    
    gtk_init (&argc, &argv);
    if (!filenames)
    {
        printf ("Supply name of image.\n");
        return 1;
    }

    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    GtkWidget *view = gtk_anim_view_new ();
    GdkPixbufAnimation *anim = gdk_pixbuf_animation_new_from_file (filenames[0],
                                                                   NULL);
    gtk_anim_view_set_anim (GTK_ANIM_VIEW (view), anim);
    GtkWidget *scroll_win = gtk_image_scroll_win_new (GTK_IMAGE_VIEW (view));

    // Request that the widget map with the same size as the image.
    int width = gdk_pixbuf_animation_get_width (anim);
    int height = gdk_pixbuf_animation_get_height (anim);
    gtk_widget_set_size_request (scroll_win, width, height);

    gtk_container_add (GTK_CONTAINER (window), scroll_win);
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (gtk_main_quit), NULL);
    gtk_widget_show_all (window);

    // Allow the window to be shrinked after it has been mapped.
    gtk_widget_set_size_request (scroll_win, -1, -1);
    gtk_main ();
}
