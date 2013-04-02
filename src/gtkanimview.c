/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * Copyright © 2007-2008 Björn Lindqvist <bjourne@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

/**
 * SECTION:gtkanimview
 * @see_also: #GtkImageView, The
 * <filename>./tests/ex-anim.c</filename> program for an example of
 * how this widget is used
 * @short_description: Subclass of #GtkImageView capable of playing
 * GIF animations.
 *
 * <para>
 *   GtkAnimView subclasses GtkImageView. It has the same look and
 *   feel as its parent but is also capable of displaying GIF
 *   animations.
 * </para>
 * <refsect2>
 *   <title>Keybindings</title>
 *   <para>
 *     GtkAnimView uses a few more keybindings in addition to those used
 *     by GtkImageView:
 *     <table width = "100%" border = "0">
 *       <thead>
 *         <tr>
 *           <th>Keys</th>
 *           <th>Corresponding function</th>
 *           <th>Description</th>
 *         </tr>
 *       </thead>
 *       <tbody>
 *         <tr>
 *           <td>%GDK_p</td>
 *           <td>gtk_anim_view_set_is_playing()</td>
 *           <td>Stops or resumes the running animation.</td>
 *         </tr>
 *         <tr>
 *           <td>%GDK_j</td>
 *           <td>gtk_anim_view_step()</td>
 *           <td>Steps the animation one frame forward.</td>
 *         </tr>
 *       </tbody>  
 *     </table>  
 *   </para>  
 * </refsect2>
 **/
#include <gdk/gdkkeysyms.h>
#include "gtkanimview.h"

/*************************************************************/
/***** Private data ******************************************/
/*************************************************************/
enum
{
    TOGGLE_RUNNING,
    STEP,
    LAST_SIGNAL
};

static guint gtk_anim_view_signals[LAST_SIGNAL] = {0};

G_DEFINE_TYPE(GtkAnimView, gtk_anim_view, GTK_TYPE_IMAGE_VIEW);

/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/

static gboolean
gtk_anim_view_updator (gpointer data)
{
    GtkAnimView *aview = (GtkAnimView *) data;

    // Workaround for #437791.
    glong delay_us = aview->delay * 1000;
    if (aview->delay == 20)
    {
        // If the delay time is 20 ms, the GIF is a "fast player." and
        // we increase it to a more reasonable 100 ms so that the
        // frame is only updated 1/5 of the times
        // gtk_anim_view_updator() is run.
        delay_us = 200;
    }
    g_time_val_add (&aview->time, delay_us);

    gboolean next = gdk_pixbuf_animation_iter_advance (aview->iter,
                                                       &aview->time);
    gtk_anim_view_set_is_playing (aview, FALSE);
    
    aview->delay = gdk_pixbuf_animation_iter_get_delay_time (aview->iter);
    aview->timer_id = g_timeout_add (aview->delay,
                                     gtk_anim_view_updator, aview);

    if (!next)
        return FALSE;

    GdkPixbuf *pixbuf = gdk_pixbuf_animation_iter_get_pixbuf (aview->iter);
    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (aview), pixbuf, FALSE);
    
    return FALSE;
}

/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/
static void
gtk_anim_view_toggle_running (GtkAnimView *aview)
{
    gtk_anim_view_set_is_playing (aview, !aview->timer_id);
}


/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
gtk_anim_view_init (GtkAnimView *aview)
{
    aview->anim = NULL;
    aview->iter = NULL;
    aview->timer_id = 0;
}

static void
gtk_anim_view_finalize (GObject *object)
{
    gtk_anim_view_set_is_playing (GTK_ANIM_VIEW (object), FALSE);
    
    /* Chain up. */
    G_OBJECT_CLASS (gtk_anim_view_parent_class)->finalize (object);
}

static void
gtk_anim_view_init_signals (GtkAnimViewClass *klass)
{
    /**
     * GtkAnimView::toggle-running:
     * @aview: a #GtkAnimView
     *
     * Stops the animation if it was playing or resumes it, if it was
     * playing. ::toggle-running is a keybinding signal emitted when
     * %GDK_p is pressed on the widget and should not be used by
     * clients of this library.
     **/
    gtk_anim_view_signals[TOGGLE_RUNNING] =
        g_signal_new ("toggle_running",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (GtkAnimViewClass, toggle_running),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);
    /**
     * GtkAnimView::step:
     * @aview: a #GtkAnimView
     *
     * Steps the animation one frame forward. If the animation is
     * playing it will first be stopped. ::step is a keybinding signal
     * emitted when %GDK_j is pressed on the widget and should not be
     * used by clients of this library.
     **/
    gtk_anim_view_signals[STEP] =
        g_signal_new ("step",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (GtkAnimViewClass, step),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE,
                      0);
}

static void
gtk_anim_view_class_init (GtkAnimViewClass *klass)
{
    gtk_anim_view_init_signals (klass);
    
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = gtk_anim_view_finalize;

    klass->toggle_running = gtk_anim_view_toggle_running;
    klass->step = gtk_anim_view_step;

    /* Add keybindings. */
    GtkBindingSet *binding_set = gtk_binding_set_by_class (klass);

    /* Stop */
    gtk_binding_entry_add_signal (binding_set, GDK_p,
                                  0, "toggle_running", 0);

    /* Step */
    gtk_binding_entry_add_signal (binding_set, GDK_j,
                                  0, "step", 0);
}

/**
 * gtk_anim_view_new:
 * @returns: A new #GtkAnimView.
 *
 * Creates a new #GtkAnimView with default values. The default values
 * are:
 *
 * <itemizedlist>
 *   <listitem>anim : %NULL</listitem>
 *   <listitem>is_playing : %FALSE</listitem>
 * </itemizedlist>
 **/
GtkWidget *
gtk_anim_view_new (void)
{
    GtkWidget *aview = g_object_new (GTK_TYPE_ANIM_VIEW, NULL);
    return aview;
}


/*************************************************************/
/***** Read-write properties *********************************/
/*************************************************************/
/**
 * gtk_anim_view_get_anim:
 * @aview: a #GtkAnimView.
 * @returns: the current animation 
 *
 * Returns the current animation of the view.
 **/
GdkPixbufAnimation *
gtk_anim_view_get_anim (GtkAnimView *aview)
{
    return aview->anim;
}

/**
 * gtk_anim_view_set_anim:
 * @aview: A #GtkAnimView.
 * @anim: A pixbuf animation to play.
 *
 * Sets the pixbuf animation to play, or %NULL to not play any
 * animation.
 *
 * If the animation is a static image or only has one frame, then the
 * static image will be displayed instead. If more frames are loaded
 * into the animation, then #GtkAnimView will automatically animate to
 * those frames.
 *
 * The effect of this method is analoguous to
 * gtk_image_view_set_pixbuf(). Fit mode is reset to
 * %GTK_FIT_SIZE_IF_LARGER so that the whole area of the animation
 * fits in the view. Three signals are emitted, first the
 * #GtkImageView will emit ::zoom-changed and then ::pixbuf-changed,
 * second, #GtkAnimView itself will emit ::anim-changed.
 *
 * The default pixbuf animation is %NULL.
 **/
void
gtk_anim_view_set_anim (GtkAnimView        *aview,
                        GdkPixbufAnimation *anim)
{
    if (aview->anim)
        g_object_unref (aview->anim);
    aview->anim = anim;
    if (!aview->anim)
    {
        gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (aview), NULL, TRUE);
        return;
    }
    g_object_ref (aview->anim);
    if (aview->iter)
        g_object_unref (aview->iter);

    g_get_current_time (&aview->time);
    aview->iter = gdk_pixbuf_animation_get_iter (aview->anim, &aview->time);

    GdkPixbuf *pixbuf = gdk_pixbuf_animation_iter_get_pixbuf (aview->iter);
    if (gdk_pixbuf_animation_is_static_image (anim))
        pixbuf = gdk_pixbuf_animation_get_static_image (anim);

    gtk_image_view_set_pixbuf (GTK_IMAGE_VIEW (aview), pixbuf, TRUE);

    gtk_anim_view_set_is_playing (aview, FALSE);
    aview->delay = gdk_pixbuf_animation_iter_get_delay_time (aview->iter);
    aview->timer_id = g_timeout_add (aview->delay,
                                     gtk_anim_view_updator, aview);
}

/**
 * gtk_anim_view_set_is_playing:
 * @aview: a #GtkImageView
 * @playing: %TRUE to play the animation, %FALSE otherwise
 *
 * Sets whether the animation should play or not. If there is no
 * current animation this method does not have any effect.
 **/
void
gtk_anim_view_set_is_playing (GtkAnimView *aview,
                              gboolean     playing)
{
    if (!playing && aview->timer_id)
    {
        /* Commanded to stop AND the animation is playing. */
        g_source_remove (aview->timer_id);
        aview->timer_id = 0;
    }
    else if (playing && aview->anim)
        gtk_anim_view_updator (aview);
}

/**
 * gtk_anim_view_get_is_playing:
 * @aview: A #GtkImageView.
 * @returns: %TRUE if an animation is playing, %FALSE otherwise.
 *
 * Returns whether the animation is playing or not. If there is no
 * current animation, this method will always returns %FALSE.
 **/
gboolean
gtk_anim_view_get_is_playing (GtkAnimView *aview)
{
    return aview->timer_id && aview->anim;
}

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/
/**
 * gtk_anim_view_step:
 * @aview: A #GtkImageView.
 *
 * Steps the animation one frame forward. If the animation is playing
 * it will be stopped. Will it wrap around if the animation is at its
 * last frame?
 **/
void
gtk_anim_view_step (GtkAnimView *aview)
{
    if (aview->anim)
    {
        // Part of workaround for #437791. gtk_anim_view_updator()
        // might not always immidiately step to the next frame, so we
        // loop until the frame is changed.
        //
        // If we are on the last frame, it will not wrap around so the
        // frame will never change. So we risk an infinite loop.
        // Unfortunately but expectedly, GdkPixbufAnimationIter
        // doesn't provide a way to check if we
        // are on the last frame because the API is totally brain
        // damaged. The work-around is to give gtk_anim_view_updator
        // exactly 10 chances to advance the frame before bailing out.
        int n = 0;
        GdkPixbuf *old = gdk_pixbuf_animation_iter_get_pixbuf (aview->iter);
        while ((gdk_pixbuf_animation_iter_get_pixbuf (aview->iter) == old) &&
               (n < 10))            
        {
            gtk_anim_view_updator (aview);
            n++;
        }
    }
    gtk_anim_view_set_is_playing (aview, FALSE);
}
