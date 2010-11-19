/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
#include <gdk/gdkkeysyms.h>
#include <src/gtkimagescrollwin.h>
#include <src/gtkimageview.h>
#include <src/gtkimagetoolselector.h>

GtkWidget *view = NULL;
GtkIImageTool *sel = NULL;

// This algorithm could be improved quite a bit.
inline static void
blur_pixel (guchar *pixels,
            int     stride,
            int     n_chans,
            int     width,
            int     height,
            int     x,
            int     y)
{
    
    int chan_tots[] = {0, 0, 0, 0};

    int xstart = (x == 0) ? 0 : -1;
    int ystart = (y == 0) ? 0 : -1;
    int xend = (x == width - 1) ? 1 : 2;
    int yend = (y == height - 1) ? 1 : 2;

    for (int yy = ystart; yy < yend; yy++)
    {
        int line_addr = (y + yy) * stride + (x + xstart) * n_chans;
        for (int xx = 0; xx < (xend - xstart); xx++)
            for (int n = 0; n < n_chans; n++)
                chan_tots[n] += pixels[line_addr++];
    }
    
    // blur_area is number of pixels that contributed to the blurring.
    int blur_area = (xend - xstart) * (yend - ystart);
    int addr = y * stride + x * n_chans;
    for (int n = 0; n < n_chans; n++)
        pixels[addr + n] = chan_tots[n] / blur_area;
}

static void
blur_cb (GtkImageView *view)
{
    GdkRectangle rect;
    gtk_image_tool_selector_get_selection (GTK_IMAGE_TOOL_SELECTOR (sel),
                                           &rect);
    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (view);
    
    guchar *pixels = gdk_pixbuf_get_pixels (pixbuf);
    int stride = gdk_pixbuf_get_rowstride (pixbuf);
    int n_chans = gdk_pixbuf_get_n_channels (pixbuf);
    int width = gdk_pixbuf_get_width (pixbuf);
    int height = gdk_pixbuf_get_height (pixbuf);
        
    for (int y = rect.y; y < rect.y + rect.height; y++)
        for (int x = rect.x; x < rect.x + rect.width; x++)
            blur_pixel (pixels, stride, n_chans, width, height, x, y);
    gtk_image_view_damage_pixels (view, &rect);
}

int
main (int   argc,
      char *argv[])
{
    printf ("This program demonstrates how a GtkImageToolSelector could\n"
            "be used to select a rectangle on an image which are then\n"
            "blurred.\n"
            "Select a rectangle on the image and press *B* to blur it.\n");

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
    
    view = gtk_image_view_new ();
    sel = gtk_image_tool_selector_new (GTK_IMAGE_VIEW (view));
    gtk_image_view_set_tool (GTK_IMAGE_VIEW (view), sel);

    // Add the blur keybinding to the view
    GtkImageViewClass *klass = GTK_IMAGE_VIEW_GET_CLASS (view);
    g_signal_new ("blur",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);
    g_signal_connect (G_OBJECT (view), "blur", G_CALLBACK (blur_cb), NULL);
    GtkBindingSet *binding_set = gtk_binding_set_by_class (klass);
    gtk_binding_entry_add_signal (binding_set, GDK_b, 0, "blur", 0);

    // Easier to see the blurring without linear interpolation.
    gtk_image_view_set_interpolation (GTK_IMAGE_VIEW (view),
                                      GDK_INTERP_NEAREST);

    GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    GtkWidget *scroll_win = gtk_image_scroll_win_new (GTK_IMAGE_VIEW (view));
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (filenames[0], NULL);
    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (view), pixbuf, TRUE);
    g_object_unref (pixbuf);
    gtk_container_add (GTK_CONTAINER (window), scroll_win);
    gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);
    gtk_widget_show_all (window);
    gtk_main ();
}
