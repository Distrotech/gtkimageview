/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- */
#ifndef __GDK_PIXBUF_DRAW_CACHE_H__
#define __GDK_PIXBUF_DRAW_CACHE_H__

#include <gdk/gdk.h>

#include "utils.h"

typedef struct _GdkPixbufDrawOpts GdkPixbufDrawOpts;
typedef struct _GdkPixbufDrawCache GdkPixbufDrawCache;

/**
 * GdkPixbufDrawMethod:
 *
 * Enumeration constants that determine the proper way to draw the
 * next time.
 *
 * <itemizedlist>
 *   <listitem>GD_PIXBUF_DRAW_METHOD_SCALE : Scale the area of the
 *   pixbuf to draw and put the result in cache. This is the slowest
 *   draw method as the whole area to be drawn must be rescaled. It is
 *   mostly used when no part of #GdkPixbufDrawCache:s cache is
 *   valid.</listitem>
 *   <listitem>GDK_PIXBUF_DRAW_METHOD_CONTAINS : Get the area of the
 *   pixbuf to draw from the cache without updating the cache
 *   afterwards. Only blitting is needed.</listitem>
 *   <listitem>GDK_PIXBUF_DRAW_METHOD_SCROLL : Partially use the cache
 *   and scale the region not cached. The cache is updated with the
 *   result.</listitem>
 * </itemizedlist>
 **/
typedef enum
{
    GDK_PIXBUF_DRAW_METHOD_SCALE = 0,
    GDK_PIXBUF_DRAW_METHOD_CONTAINS = 1,
    GDK_PIXBUF_DRAW_METHOD_SCROLL = 2
} GdkPixbufDrawMethod;

/**
 * GdkPixbufDrawOpts:
 *
 * Struct which holds options for how the pixbuf should be
 * drawn. Options include such things like the source rectangle in the
 * pixbuf to draw, where to draw it, which zoom to use and so on.
 **/
struct _GdkPixbufDrawOpts
{
    gdouble        zoom;

    /* Rectangle in zoom-space coordinates of the area to draw. */
    GdkRectangle   zoom_rect;

    /* Position in widget-space coordinates where to draw. */
    int            widget_x;
    int            widget_y;
       
    GdkInterpType  interp;
    GdkPixbuf     *pixbuf;

    /* The two colors to use to draw the checker board. */
    int            check_color1;
    int            check_color2;
};

/**
 * GdkPixbufDrawCache:
 *
 * Cache that ensures fast redraws by storing the last draw
 * operation. For example, when resizing a #GtkImageView, the view
 * receives an expose event and must redraw the damaged region. Unless
 * fitting is %TRUE, most of the pixels it should draw are indentical
 * to the ones drawn the previous time. Redrawing them is wasteful
 * because scaling and especially bilinear scaling is very
 * slow. Therefore, PixbufDrawCache objectifies the drawing process
 * and adds a cache with the last draw from which pixels can be
 * fetched.
 *
 * This object is present purely to ensure optimal speed. A
 * #GtkIImageTool that is asked to redraw a part of the image view
 * widget could either do it by itself using gdk_pixbuf_scale() and
 * gdk_draw_pixbuf().
 **/
struct _GdkPixbufDrawCache
{
    GdkPixbuf         *last_pixbuf;
    GdkPixbufDrawOpts  old;
    int                check_size;
};

GdkPixbufDrawCache *gdk_pixbuf_draw_cache_new (void);
void          gdk_pixbuf_draw_cache_free (GdkPixbufDrawCache *cache);
void          gdk_pixbuf_draw_cache_invalidate (GdkPixbufDrawCache *cache);
void          gdk_pixbuf_draw_cache_draw (GdkPixbufDrawCache *cache,
                                          GdkPixbufDrawOpts  *opts,
                                          GdkDrawable        *drawable);
GdkPixbufDrawMethod gdk_pixbuf_draw_cache_get_method (GdkPixbufDrawOpts *old,
                                                      GdkPixbufDrawOpts *new_);

#endif
