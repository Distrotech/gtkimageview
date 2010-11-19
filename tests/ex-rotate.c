/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
#include <gdk/gdkkeysyms.h>
#include <src/gtkimagescrollwin.h>
#include <src/gtkimageview.h>

/*
  This is the sad state of rotations.

  GtkImageView itself refuses to perform orthogonal rotations. The
  reason why, is that an attempt to support it would either be
  half-assed or extremely complex.

  The half-assed way is the one shown below. :) You can only use it to
  rotate pixbufs. Rotating animations the same way, would require
  gdk_pixbuf_rotate_simple to be called on each new frame which would
  be prohibitively expensive.

  If you use GtkImageToolSelector and rotate the pixbuf, then you
  would naturally want the selection to rotate with the pixbuf. But
  that is pretty tricky to implement and would require you to know
  what the rotation state of the view is.

  Ideally, the view should remember the rotation state and not call
  gdk_pixbuf_rotate_simple() at all, but that is really complex as
  each coordinate in every calculation would have to be translated.
 */
static void
rotate_cb (GtkImageView      *view,
           GdkPixbufRotation  rotation)
{
    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (view);
    if (!pixbuf)
        return;
    
    GdkPixbuf *dest = gdk_pixbuf_rotate_simple (pixbuf, rotation);
    gtk_image_view_set_pixbuf (view, dest, gtk_image_view_get_fitting (view));
    g_object_unref (dest);
}

int
main (int   argc,
      char *argv[])
{
    printf ("This program demonstrates how to perform orthogonal\n"
            "rotations on the pixbuf that GtkImageView displays.\n"
            "Press *E* and *R* to rotate the view counter-clockwise\n"
            "and clockwise.\n");

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
    GtkWidget *view = gtk_image_view_new ();

    // Add rotation keybindings to the view
    GtkImageViewClass *klass = GTK_IMAGE_VIEW_GET_CLASS (view);
    g_signal_new ("rotate",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__ENUM,
                  G_TYPE_NONE,
                  1, G_TYPE_INT);
    g_signal_connect (G_OBJECT (view), "rotate", G_CALLBACK (rotate_cb), NULL);
    
    GtkBindingSet *binding_set = gtk_binding_set_by_class (klass);
    gtk_binding_entry_add_signal (binding_set, GDK_r, 0,
                                  "rotate", 1, G_TYPE_INT,
                                  GDK_PIXBUF_ROTATE_CLOCKWISE);
    gtk_binding_entry_add_signal (binding_set, GDK_e, 0,
                                  "rotate", 1, G_TYPE_INT,
                                  GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
    
    GtkWidget *scroll_win = gtk_image_scroll_win_new (GTK_IMAGE_VIEW (view));
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (filenames[0], NULL);
    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, TRUE);
    g_object_unref (pixbuf);
    gtk_container_add (GTK_CONTAINER (window), scroll_win);
    gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);
    gtk_widget_show_all (window);
    gtk_main ();
}
