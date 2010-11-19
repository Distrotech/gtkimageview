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
#ifndef __GTK_ANIM_VIEW_H__
#define __GTK_ANIM_VIEW_H__

#include "gtkimageview.h"

G_BEGIN_DECLS

#define GTK_TYPE_ANIM_VIEW              (gtk_anim_view_get_type ())
#define GTK_ANIM_VIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_ANIM_VIEW, GtkAnimView))
#define GTK_ANIM_VIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_ANIM_VIEW, GtkAnimViewClass))
#define GTK_IS_ANIM_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_ANIM_VIEW))
#define GTK_IS_ANIM_VIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_ANIM_VIEW))
#define GTK_ANIM_VIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_ANIM_VIEW, GtkAnimViewClass))

typedef struct _GtkAnimView GtkAnimView;
typedef struct _GtkAnimViewClass GtkAnimViewClass;

/**
 * GtkAnimView:
 *
 * GtkAnimView is a subclass of #GtkImageView that provies facilities
 * for displaying and controlling an animation.
 **/
struct _GtkAnimView
{
    GtkImageView     parent;

    /* The current animation. */
    GdkPixbufAnimation *anim;

    /* The iterator of the current animation. */
    GdkPixbufAnimationIter *iter;

    /* ID of the currently running animation timer. */
    int                 timer_id;

    /* Timer used to get the right frame. */
    GTimeVal            time;
    int                 delay;

};

struct _GtkAnimViewClass
{
    GtkImageViewClass parent_class;

    /* Keybinding signals. */
    void (* toggle_running)                  (GtkAnimView        *aview);
    void (* step)                            (GtkAnimView        *aview);
};

GType         gtk_anim_view_get_type         (void) G_GNUC_CONST;

/* Constructors */
GtkWidget    *gtk_anim_view_new              (void);

/* Read-write properties */
GdkPixbufAnimation *gtk_anim_view_get_anim   (GtkAnimView        *aview);
void          gtk_anim_view_set_anim         (GtkAnimView        *aview,
                                              GdkPixbufAnimation *anim);
void          gtk_anim_view_set_is_playing   (GtkAnimView        *aview,
                                              gboolean            playing);
gboolean      gtk_anim_view_get_is_playing   (GtkAnimView        *aview);

/* Actions */
void          gtk_anim_view_step             (GtkAnimView        *aview); 

G_END_DECLS

#endif
