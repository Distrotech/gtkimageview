/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
#include <src/gtkimagescrollwin.h>
#include <src/gtkimageview.h>

GdkPixbuf *pixbuf = NULL;
GtkWidget *view = NULL;

/* Use a simple XOR pattern to distort the pixbuf. */
static void
xor_pixbuf ()
{
    guchar *pixels = gdk_pixbuf_get_pixels (pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride (pixbuf);
    int n_channels = gdk_pixbuf_get_n_channels (pixbuf);
    for (int y = 0; y < gdk_pixbuf_get_height (pixbuf); y++)
        for (int x = 0; x < gdk_pixbuf_get_width (pixbuf); x++)
        {
            guchar *p = pixels + y * rowstride + x * n_channels;
            for (int n  = 0; n < 3; n++)
                p[n] ^= 0xff;
        }
}

static gboolean
distort_pixbuf (gpointer data)
{
    printf ("* Manipulating the data of the pixbuf!\n");
    xor_pixbuf ();
    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, FALSE);
    g_timeout_add (2000, distort_pixbuf, NULL);
    return FALSE;
}

int
main (int argc, char *argv[])
{
    printf ("This program demonstrates how changes to the pixbuf that\n"
            "GtkImageView displays must be handled.\n");
    gtk_init (&argc, &argv);
    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    view = gtk_image_view_new ();
    
    char *fname = "tests/gnome_logo.jpg";
    pixbuf = gdk_pixbuf_new_from_file (fname, NULL);
    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, TRUE);

    g_timeout_add (2000, distort_pixbuf, NULL);

    GtkWidget *scroll_win = gtk_image_scroll_win_new (GTK_IMAGE_VIEW (view));
    gtk_container_add (GTK_CONTAINER (window), scroll_win);
    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (gtk_main_quit), NULL);
    
    gtk_widget_show_all (window);
    gtk_main ();
}
