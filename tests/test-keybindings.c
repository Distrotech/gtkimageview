/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; coding: utf-8 -*- */
/**
 * This file contains tests that validates that the correct response
 * is executed for each keybinding in GtkImageView.
 **/
#include <src/gtkimageview.h>
#include <src/gtkzooms.h>

#include <assert.h>

/**
 * test_set_zoom_1_2_3
 *
 * Tests that the keybinding for setting the zoom to 100%, 200% and
 * 300% works as expected.
 **/
static void
test_set_zoom_1_2_3 ()
{
    printf ("test_set_zoom_1_2_3\n");
    GtkImageView *view = (GtkImageView *) gtk_image_view_new ();

    char *key_names[] = {"3", "2", "1"};
    gdouble zooms[] = {3.0, 2.0, 1.0};
    for (int n = 0; n < G_N_ELEMENTS (key_names); n++)
    {
        guint keyval = gdk_keyval_from_name (key_names[n]);
        assert (gtk_bindings_activate ((GtkObject *) view, keyval, 0));
        assert (gtk_image_view_get_zoom (view) == zooms[n]);
                
    }
}

/**
 * test_zoom_in
 *
 * Tests that the keybindings for zooming in works as expected.
 **/
static void
test_zoom_in ()
{
    printf ("test_zoom_in\n");
    GtkImageView *view = (GtkImageView *) gtk_image_view_new ();

    char *zoom_in_keys[] = {"plus", "equal", "KP_Add"};
    for (int i = 0; i < G_N_ELEMENTS (zoom_in_keys); i++)
    {
        gtk_image_view_set_zoom (view, 1.0);
        guint keyval = gdk_keyval_from_name (zoom_in_keys[i]);
        for (int j = 0; j < 20; j++)
        {
            gdouble old_zoom = gtk_image_view_get_zoom (view);
            
            assert (gtk_bindings_activate ((GtkObject *) view, keyval, 0));
            gdouble exp_zoom = gtk_zooms_get_zoom_in (old_zoom);
            assert (gtk_image_view_get_zoom (view) == exp_zoom);
        }
    }
}

static void
test_zoom_out ()
{
    printf ("test_zoom_out\n");
    GtkImageView *view = (GtkImageView *) gtk_image_view_new ();
    char *zoom_out_keys[] = {"minus", "KP_Subtract"};
    for (int i = 0; i < G_N_ELEMENTS (zoom_out_keys); i++)
    {
        gtk_image_view_set_zoom (view, 1.0);
        guint keyval = gdk_keyval_from_name (zoom_out_keys[i]);
        for (int j = 0; j < 100; j++)
        {
            gdouble old_zoom = gtk_image_view_get_zoom (view);

            assert (gtk_bindings_activate ((GtkObject *) view, keyval, 0));
            gdouble exp_zoom = gtk_zooms_get_zoom_out (old_zoom);
            assert (gtk_image_view_get_zoom (view) == exp_zoom);
        }
    }
}

int
main (int argc, char *argv[])
{
    gtk_init (&argc, &argv);
    test_set_zoom_1_2_3 ();
    test_zoom_in ();
    test_zoom_out ();
    printf ("3 tests passed.\n");
}


