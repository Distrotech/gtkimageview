/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * This file contains tests that verify that the #GtkAnimView class
 * behaves correctly.
 **/
#include <assert.h>
#include <src/gtkanimview.h>

typedef struct
{
    GdkPixbufAnimation *anim;
    GdkPixbuf          *frames[3];
} AnimWrapper;

static AnimWrapper *
anim_wrapper_new (gdouble fps)
{
    AnimWrapper *aw = g_new0 (AnimWrapper, 1);
    GdkPixbufSimpleAnim *sa = gdk_pixbuf_simple_anim_new (9, 9, fps);
    for (int n = 0; n < 3; n++)
    {
        aw->frames[n] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 9, 9);
        gdk_pixbuf_simple_anim_add_frame (sa, aw->frames[n]);
    }
    aw->anim = GDK_PIXBUF_ANIMATION (sa);
    return aw;
}

static void
anim_wrapper_free (AnimWrapper *aw)
{
    g_object_unref (aw->anim);
    for (int n = 0; n < 3; n++)
        g_object_unref (aw->frames[n]);
    g_free (aw);
}

/**
 * test_finalize_stops_timer:
 *
 * The objective of this test is to verify that GtkAnimView removes
 * all its sources that it is using when finalized. The test case
 * could be a little simpler if #477653 is fixed, but for now, we rely
 * on brute force and trying to provoke a segfault.
 **/
static void
test_finalize_stops_timer ()
{
    printf ("test_finalize_stops_timer\n");
    GtkAnimView *aviews[20];
    for (int n = 0; n < G_N_ELEMENTS (aviews); n++)
    {
        aviews[n] = (GtkAnimView *) gtk_anim_view_new ();
        g_object_ref_sink (aviews[n]);

        AnimWrapper *aw = anim_wrapper_new (2.0);
        // Timer starts here
        gtk_anim_view_set_anim (aviews[n], aw->anim);
    }
    for (int n = 0; n < G_N_ELEMENTS (aviews); n++)
    {
        gtk_widget_destroy (GTK_WIDGET (aviews[n]));
        g_object_unref (aviews[n]);
    }
    // Timers might run, but object is gnome, leads to segfault.
    GMainContext *main_ctx = g_main_context_default ();
    g_usleep (1100 * 1000);
    g_main_context_iteration (main_ctx, TRUE);
}

/**
 * test_inherited_default_values:
 *
 * The objective of this test is to verify that the default values
 * specified in #GtkImageView are inherited correctly.
 **/
static void
test_inherited_default_values ()
{
    printf ("test_inherited_default_values\n");
    GtkImageView *view = (GtkImageView *) gtk_anim_view_new ();

    assert (view);
    assert (gtk_image_view_get_zoom (view) == (gdouble) 1.0);
    assert (gtk_image_view_get_pixbuf (view) == NULL);
    assert (gtk_image_view_get_fitting (view));

    GdkRectangle foo;
    assert (!gtk_image_view_get_viewport (view, &foo));

    assert (gtk_image_view_get_black_bg (view) == FALSE);

    int interp = gtk_image_view_get_interpolation (view);
    assert (interp == GDK_INTERP_BILINEAR);
}

/**
 * test_get_set_anim:
 *
 * The objective of this test is to verify that getting and setting
 * the anim attribute behaves like expected.
 **/
static void
test_get_set_anim ()
{
    printf ("test_get_set_anim\n");
    GtkAnimView *aview = (GtkAnimView *) gtk_anim_view_new ();

    assert (!gtk_anim_view_get_anim (aview));

    AnimWrapper *aw = anim_wrapper_new (5.0);

    gtk_anim_view_set_anim (aview, aw->anim);
    assert (gtk_anim_view_get_anim (aview) == aw->anim);

    gtk_anim_view_set_anim (aview, NULL);
    assert (!gtk_anim_view_get_anim (aview));

    anim_wrapper_free (aw);
}

/**
 * test_get_initial_showing_frame:
 *
 * The objective of this test is to verify that the animation view
 * shows the first frame of the animation when the animation is
 * loaded.
 **/
static void
test_get_initial_showing_frame ()
{
    printf ("test_get_initial_showing_frame\n");
    GtkAnimView *aview = (GtkAnimView *) gtk_anim_view_new ();
    g_object_ref_sink (aview);

    AnimWrapper *aw = anim_wrapper_new (1.0);

    gtk_anim_view_set_anim (aview, aw->anim);

    GdkPixbuf *showing = gtk_image_view_get_pixbuf (GTK_IMAGE_VIEW (aview));
    assert (showing == aw->frames[0]);

    gtk_widget_destroy (GTK_WIDGET (aview));
    g_object_unref (aview);

    anim_wrapper_free (aw);
}

/**
 * test_get_second_showing_frame:
 *
 * The objective of this test is to verify that the animation view is
 * updated correctly. It plays an animation with 1 frame per second,
 * so after 1.5 seconds it should show the second frame of the
 * animation. 
 **/
static void
test_get_second_showing_frame ()
{
    printf ("test_get_second_showing_frame\n");
    GtkAnimView *aview = (GtkAnimView *) gtk_anim_view_new ();
    g_object_ref_sink (aview);

    AnimWrapper *aw = anim_wrapper_new (2.0);

    gtk_anim_view_set_anim (aview, aw->anim);

    /* Wait until 1.5 seconds have elapsed. The delay does not make
       sense, see #431997. */
    g_usleep (1100 * 1000);
    GMainContext *main_ctx = g_main_context_default ();
    g_main_context_iteration (main_ctx, TRUE);

    GdkPixbuf *showing = gtk_image_view_get_pixbuf (GTK_IMAGE_VIEW (aview));

    assert (showing == aw->frames[1]);

    gtk_widget_destroy (GTK_WIDGET (aview));
    g_object_unref (aview);
    anim_wrapper_free (aw);
}


/**
 * test_stopping_animation:
 *
 * The objective of this test is to verify that when an animation is
 * stopped, no "animation time" elapses. That is, if the animation is
 * stopped at frame 0 it should continue to play at frame 0 when it is
 * started again.
 **/
static void
test_stopping_animation ()
{
    printf ("test_stopping_animation\n");
/*     GtkAnimView *aview = (GtkAnimView *) gtk_anim_view_new (); */
/*     g_object_ref_sink (aview); */
    
/*     AnimWrapper *aw = anim_wrapper_new (2.0); */

/*     gtk_anim_view_set_anim (aview, aw->anim); */
    
/*     g_usleep (1100 * 1000); */
/*     GMainContext *main_ctx = g_main_context_default (); */
/*     g_main_context_iteration (main_ctx, TRUE); */

/*     gtk_widget_destroy (GTK_WIDGET (aview)); */
/*     g_object_unref (aview); */
/*     anim_wrapper_free (aw); */
}

/**
 * test_playing_null_anim:
 *
 * The objective of this test is to verify that the animation view
 * never reports that the animation is running when the animation is
 * NULL.
 **/
static void
test_playing_null_anim ()
{
    printf ("test_playing_null_anim\n");
    GtkAnimView *aview = (GtkAnimView *) gtk_anim_view_new ();
    g_object_ref_sink (aview);

    assert (!gtk_anim_view_get_is_playing (aview));
    gtk_anim_view_set_is_playing (aview, TRUE);
    assert (!gtk_anim_view_get_is_playing (aview));

    gtk_widget_destroy (GTK_WIDGET (aview));
    g_object_unref (aview);
}

int
main (int   argc,
      char *argv[])
{
    gtk_init (&argc, &argv);
    test_finalize_stops_timer ();
    test_inherited_default_values ();
    test_get_set_anim ();
    test_get_initial_showing_frame ();
    test_get_second_showing_frame ();
    test_stopping_animation ();
    test_playing_null_anim ();
    printf ("7 tests passed.\n");
}

