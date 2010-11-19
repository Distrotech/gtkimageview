/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * Copyright © 2009 Björn Lindqvist <bjourne@gmail.com>
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
#include "gtkimagetoolcairo.h"

/*************************************************************/
/***** Copy functions ****************************************/
/*************************************************************/
/**
 * gdk_pixbuf_copy_to_surface:
 *
 * Copies a rectanguar area in a pixbuf to a cairo surface. This
 * function should be as fast as possible which I don't know if it is.
 **/
static void
gdk_pixbuf_copy_to_surface (GdkPixbuf       *pixbuf,
                            GdkRectangle    *rect,
                            cairo_surface_t *surface)
{
    cairo_t *cr = cairo_create (surface);
    gdk_cairo_set_source_pixbuf (cr, pixbuf, 0, 0);
    cairo_rectangle (cr, rect->x, rect->y, rect->width, rect->height);
    cairo_clip (cr);
    cairo_paint (cr);
    cairo_destroy (cr);
}

/**
 * cairo_image_surface_copy:
 *
 * Copies a rectangular area in a cairo surface on another surface.
 **/
static void
cairo_image_surface_copy (cairo_surface_t *src,
                          GdkRectangle    *rect,
                          cairo_surface_t *dst)
{
    int src_stride = cairo_image_surface_get_stride (src) / 4;
    int *dst_pixels = (int *) cairo_image_surface_get_data (dst);
    int *src_pixels = (int *) cairo_image_surface_get_data (src);
    int ybase = rect->y * src_stride + rect->x;
    for (int y = 0; y < rect->height; y++)
    {
        int idx = ybase;
        for (int x = 0; x < rect->width; x++)
        {
            dst_pixels[idx] = src_pixels[idx];
            idx++;
        }
        ybase += src_stride;
    }
}

static void
cairo_image_surface_copy_to_pixbuf (cairo_surface_t *surface,
                                    GdkRectangle    *rect,
                                    GdkPixbuf       *pixbuf)
{
    int src_stride = cairo_image_surface_get_stride (surface);
    int dst_stride = gdk_pixbuf_get_rowstride (pixbuf);
    guchar *dst_pixels = gdk_pixbuf_get_pixels (pixbuf);
    guchar *src_pixels = cairo_image_surface_get_data (surface);

    if (gdk_pixbuf_get_has_alpha (pixbuf))
    {
        for (int y = 0; y < rect->height; y++)
        {
            int src_idx = (rect->y + y) * src_stride + rect->x * 4;
            int dst_idx = (rect->y + y) * dst_stride + rect->x * 4;
            for (int x = 0; x < rect->width; x++)
            {
                dst_pixels[dst_idx + 0] = src_pixels[src_idx + 2];
                dst_pixels[dst_idx + 1] = src_pixels[src_idx + 1];
                dst_pixels[dst_idx + 2] = src_pixels[src_idx + 0];
                dst_pixels[dst_idx + 3] = src_pixels[src_idx + 3];
                src_idx += 4;
                dst_idx += 4;
            }
        }
    }
    else
    {
        for (int y = 0; y < rect->height; y++)
        {
            int src_idx = (rect->y + y) * src_stride + rect->x * 4;
            int dst_idx = (rect->y + y) * dst_stride + rect->x * 3;
            for (int x = 0; x < rect->width; x++)
            {
                dst_pixels[dst_idx + 0] = src_pixels[src_idx + 2];
                dst_pixels[dst_idx + 1] = src_pixels[src_idx + 1];
                dst_pixels[dst_idx + 2] = src_pixels[src_idx + 0];
                src_idx += 4;
                dst_idx += 3;
            }
        }
    }
}


/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/
/**
 * gtk_image_tool_cairo_set_redraw_area:
 *
 * Clips the input rectangle against the pixbuf size and sets it as
 * the new redraw area.
 **/
static void
gtk_image_tool_cairo_set_redraw_area (GtkImageToolCairo *cairo,
                                      GdkRectangle      *rect)
{
    GdkPixbuf *pixbuf = cairo->pixbuf;
    GdkRectangle pb_rect = {0, 0, 
                            gdk_pixbuf_get_width (pixbuf), 
                            gdk_pixbuf_get_height (pixbuf)};
    gdk_rectangle_intersect (&pb_rect, rect, rect);
    cairo->redraw_area = *rect;
}

/**
 * gtk_image_tool_cairo_surface_changed:
 *
 * Invalidate the cache and force a redraw of the damaged area. I'm
 * not sure whether the queued redraw area should be padded or not.
 **/
static void
gtk_image_tool_cairo_surface_changed (GtkImageToolCairo *cairo,
                                      GdkRectangle      *rect)
{
    gdk_pixbuf_draw_cache_invalidate (cairo->cache);
    GdkRectangle wid_rect;
    gtk_image_view_image_to_widget_rect (cairo->view, rect, &wid_rect);
    gtk_widget_queue_draw_area (GTK_WIDGET (cairo->view),
                                wid_rect.x, wid_rect.y,
                                wid_rect.width, wid_rect.height);
}

/**
 * gtk_image_tool_cairo_null_surfaces:
 *
 * Ensure that the cairo surfaces are destroyed and set to %NULL.
 **/
static void
gtk_image_tool_cairo_null_surfaces (GtkImageToolCairo *cairo)
{
    if (!cairo->image_surface)
        return;
    cairo_surface_destroy (cairo->image_surface);
    cairo_surface_destroy (cairo->draw_surface);
    cairo->image_surface = NULL;
    cairo->draw_surface = NULL;
}

/**
 * gtk_image_tool_cairo_null_surfaces:
 *
 * Update the cairo surfaces and put the result on the pixbuf.
 **/
static void
gtk_image_tool_cairo_update_pixbuf (GtkImageToolCairo *cairo)
{
    GdkRectangle redraw = cairo->redraw_area;
    // Guard against broken update areas.
    if (!redraw.width || !redraw.height)
        return;

    // Overwrite what the shaper wrote last.
    cairo_image_surface_copy (cairo->image_surface,
                              &redraw,
                              cairo->draw_surface);
    cairo_t *cr = cairo_create (cairo->draw_surface);
    cairo_rectangle (cr, redraw.x, redraw.y, redraw.width, redraw.height);
    cairo_clip (cr);
    gtk_iimage_cairo_shaper_draw (cairo->shaper, cr);

    cairo_image_surface_copy_to_pixbuf (cairo->draw_surface,
                                        &redraw,
                                        cairo->pixbuf);
    cairo_destroy (cr);
}

/*************************************************************/
/***** Implementation of the GtkIImageTool interface *********/
/*************************************************************/
static GdkCursor*
cursor_at_point (GtkIImageTool *tool,
                 int            x,
                 int            y)
{
    GtkImageToolCairo *cairo = GTK_IMAGE_TOOL_CAIRO (tool);
    return cairo->crosshair;
}

static gboolean
motion_notify (GtkIImageTool  *tool,
               GdkEventMotion *ev)
{
    GtkImageToolCairo *cairo = GTK_IMAGE_TOOL_CAIRO (tool);
    mouse_handler_motion_notify (cairo->mouse_handler, ev);
    if (!cairo->mouse_handler->dragging)
        return FALSE;

    GdkRectangle widget_rect = {ev->x, ev->y, 0, 0};
    GdkRectangle image_rect;
    if (!gtk_image_view_widget_to_image_rect (cairo->view,
                                              &widget_rect,
                                              &image_rect))
        return FALSE;
    
    GdkRectangle old_damage_area, new_damage_area;
    gtk_iimage_cairo_shaper_get_damage_area (cairo->shaper, &old_damage_area);
    gtk_iimage_cairo_shaper_motion (cairo->shaper, image_rect.x, image_rect.y);
    gtk_iimage_cairo_shaper_get_damage_area (cairo->shaper, &new_damage_area);

    // Union the areas to get the complete redraw area.
    if (cairo->redraw_area.x == -1)
        cairo->redraw_area = old_damage_area;

    GdkRectangle total_damage_area;
    gdk_rectangle_union (&cairo->redraw_area,
                         &new_damage_area,
                         &total_damage_area);
    gtk_image_tool_cairo_set_redraw_area (cairo, &total_damage_area);
    gtk_image_tool_cairo_surface_changed (cairo, &cairo->redraw_area);
    return TRUE;
}

static gboolean
button_press (GtkIImageTool  *tool,
              GdkEventButton *ev)
{
    GtkImageToolCairo *cairo = GTK_IMAGE_TOOL_CAIRO (tool);
    GdkRectangle widget_rect = {ev->x, ev->y, 0, 0};
    GdkRectangle image_rect;
    if (!gtk_image_view_widget_to_image_rect (cairo->view,
                                              &widget_rect,
                                              &image_rect))
        return FALSE;

    if (!mouse_handler_button_press (cairo->mouse_handler, ev))
        return FALSE;

    gtk_iimage_cairo_shaper_start (cairo->shaper, image_rect.x, image_rect.y);
    return TRUE;
}

static gboolean
button_release (GtkIImageTool  *tool,
                GdkEventButton *ev)
{
    GtkImageToolCairo *cairo = GTK_IMAGE_TOOL_CAIRO (tool);

    if (!cairo->pixbuf)
        return mouse_handler_button_release (cairo->mouse_handler, ev);

    GdkRectangle damage_area;
    gtk_iimage_cairo_shaper_get_damage_area (cairo->shaper, &damage_area);
    gtk_image_tool_cairo_set_redraw_area (cairo, &damage_area);

    // Let the shaper draw on the image surface.
    cairo_t *cr = cairo_create (cairo->image_surface);
    cairo_rectangle (cr,
                     cairo->redraw_area.x, cairo->redraw_area.y,
                     cairo->redraw_area.width, cairo->redraw_area.height);
    cairo_clip (cr);
    gtk_iimage_cairo_shaper_draw (cairo->shaper, cr);
    cairo_destroy (cr);

    gtk_iimage_cairo_shaper_reset (cairo->shaper);
    // These two calls means we will do a double-draw but it cannot be
    // helped.
    gtk_image_tool_cairo_surface_changed (cairo, &cairo->redraw_area);
    gtk_image_view_damage_pixels (cairo->view, &cairo->redraw_area);
    return mouse_handler_button_release (cairo->mouse_handler, ev);
}

static void
paint_image (GtkIImageTool     *tool,
             GdkPixbufDrawOpts *opts,
             GdkDrawable       *drawable)
{
    GtkImageToolCairo *cairo = GTK_IMAGE_TOOL_CAIRO (tool);
    if (cairo->redraw_area.x != -1)
    {
        gtk_image_tool_cairo_update_pixbuf (cairo);
        cairo->redraw_area = (GdkRectangle){-1, -1, -1, -1};
    }
    gdk_pixbuf_draw_cache_draw (cairo->cache, opts, drawable);
}

static void
pixbuf_changed (GtkIImageTool *tool,
                gboolean       reset_fit,
                GdkRectangle  *rect)
{
    GtkImageToolCairo *cairo = GTK_IMAGE_TOOL_CAIRO (tool);
    GdkPixbuf *pixbuf = gtk_image_view_get_pixbuf (cairo->view);
    if (!pixbuf)
    {
        gtk_image_tool_cairo_null_surfaces (cairo);
        return;
    }

    // The pixbuf instance is the same but data in it has changed, we
    // can ignore that.
    if (pixbuf == cairo->pixbuf && cairo->overwritable_pixbuf)
        return;

    cairo->pixbuf = pixbuf;

    int width = gdk_pixbuf_get_width (pixbuf);
    int height = gdk_pixbuf_get_height (pixbuf);
    cairo_format_t format = CAIRO_FORMAT_RGB24;
    if (gdk_pixbuf_get_has_alpha (pixbuf))
        format = CAIRO_FORMAT_ARGB32;
    
    if (!cairo->image_surface
        || cairo_image_surface_get_width (cairo->image_surface) != width
        || cairo_image_surface_get_height (cairo->image_surface) != height
        || cairo_image_surface_get_format (cairo->image_surface) != format)
    {
        gtk_image_tool_cairo_null_surfaces (cairo);
        cairo->image_surface = cairo_image_surface_create (format,
                                                           width, height);
        cairo->draw_surface = cairo_image_surface_create (format,
                                                          width, height);
    }

    // The damaged area.
    GdkRectangle damage = {0, 0, width, height};
    if (rect)
        damage = *rect;

    // Copy contents to cairo surfaces and schedule a redraw.
    gdk_pixbuf_copy_to_surface (pixbuf, &damage, cairo->image_surface);
    cairo_image_surface_copy (cairo->image_surface,
                              &damage,
                              cairo->draw_surface);
    gtk_image_tool_cairo_surface_changed (cairo, &damage);
}

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
gtk_iimage_tool_interface_init (gpointer g_iface,
                                gpointer iface_data)
{
    GtkIImageToolClass *klass = (GtkIImageToolClass *) g_iface;
    klass->button_press = button_press;
    klass->button_release = button_release;
    klass->cursor_at_point = cursor_at_point;
    klass->motion_notify = motion_notify;
    klass->paint_image = paint_image;
    klass->pixbuf_changed = pixbuf_changed;
}

G_DEFINE_TYPE_EXTENDED (GtkImageToolCairo,
                        gtk_image_tool_cairo,
                        G_TYPE_OBJECT,
                        0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_IIMAGE_TOOL,
                                               gtk_iimage_tool_interface_init));

static void
gtk_image_tool_cairo_finalize (GObject *object)
{
    GtkImageToolCairo *cairo = GTK_IMAGE_TOOL_CAIRO (object);
    gdk_cursor_unref (cairo->crosshair);
    gdk_pixbuf_draw_cache_free (cairo->cache);
    g_free (cairo->mouse_handler);
    gtk_image_tool_cairo_null_surfaces (cairo);

    /* Chain up */
    G_OBJECT_CLASS (gtk_image_tool_cairo_parent_class)->finalize (object);
}

enum
{
    PROP_IMAGE_VIEW = 1,
    PROP_CAIRO_SHAPER = 2
};

static void
gtk_image_tool_cairo_get_property (GObject    *object,
                                   guint       prop_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
    GtkImageToolCairo *cairo = GTK_IMAGE_TOOL_CAIRO (object);
    if (prop_id == PROP_CAIRO_SHAPER)
        g_value_set_object (value, cairo->shaper);
    else
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gtk_image_tool_cairo_set_property (GObject      *object,
                                   guint         prop_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
    GtkImageToolCairo *cairo = GTK_IMAGE_TOOL_CAIRO (object);
    if (prop_id == PROP_IMAGE_VIEW)
        cairo->view = g_value_get_object (value);
    else if (prop_id == PROP_CAIRO_SHAPER)
        cairo->shaper = g_value_get_object (value);
    else
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
gtk_image_tool_cairo_class_init (GtkImageToolCairoClass *klass)
{
    GObjectClass *object_class = (GObjectClass *) klass;
    object_class->finalize = gtk_image_tool_cairo_finalize;
    object_class->set_property = gtk_image_tool_cairo_set_property;
    object_class->get_property = gtk_image_tool_cairo_get_property;

    GParamSpec *pspec;
    pspec = g_param_spec_object ("view",
                                 "Image View",
                                 "Image View to draw on",
                                 GTK_TYPE_IMAGE_VIEW,
                                 G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
    g_object_class_install_property (object_class, PROP_IMAGE_VIEW, pspec);

    pspec = g_param_spec_object ("shaper",
                                 "Cairo Shaper",
                                 "Cairo Shaper to draw with",
                                 GTK_TYPE_IIMAGE_CAIRO_SHAPER,
                                 G_PARAM_READWRITE);
    g_object_class_install_property (object_class, PROP_CAIRO_SHAPER, pspec);
}

static void
gtk_image_tool_cairo_init (GtkImageToolCairo *cairo)
{
    cairo->crosshair = gdk_cursor_new (GDK_CROSSHAIR);
    cairo->cache = gdk_pixbuf_draw_cache_new ();
    cairo->mouse_handler = mouse_handler_new (cairo->crosshair);
    // -1 is used to indicate that no pixbuf area is to be updated.
    cairo->redraw_area = (GdkRectangle){-1, -1, -1, -1};
    cairo->overwritable_pixbuf = TRUE;
}

GtkIImageTool*
gtk_image_tool_cairo_new (GtkImageView         *view,
                          GtkIImageCairoShaper *shaper)
{
    g_return_val_if_fail (view, NULL);
    g_return_val_if_fail (shaper, NULL);
    gpointer data = g_object_new (GTK_TYPE_IMAGE_TOOL_CAIRO,
                                  "view", view,
                                  "shaper", shaper,
                                  NULL);
    return GTK_IIMAGE_TOOL (data);
}

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/
void
gtk_image_tool_cairo_damage_surface (GtkImageToolCairo *cairo,
                                     GdkRectangle      *rect)
{
    if (!cairo->pixbuf)
        return;
    GdkPixbuf *pixbuf = cairo->pixbuf;
    GdkRectangle damage = {0, 0,
                           gdk_pixbuf_get_width (pixbuf),
                           gdk_pixbuf_get_height (pixbuf)};
    if (rect)
        damage = *rect;

    gtk_image_tool_cairo_set_redraw_area (cairo, &damage);
    gtk_image_tool_cairo_update_pixbuf (cairo);
    gtk_image_tool_cairo_surface_changed (cairo, &damage);
    gtk_image_view_damage_pixels (cairo->view, &damage);
}

/*************************************************************/
/***** Read-only properties **********************************/
/*************************************************************/
cairo_surface_t*
gtk_image_tool_cairo_get_image_surface (GtkImageToolCairo *cairo)
{
    return cairo->image_surface;
}

/*************************************************************/
/***** Read-write properties *********************************/
/*************************************************************/
gboolean
gtk_image_tool_cairo_get_overwritable_pixbuf (GtkImageToolCairo *cairo)
{
    return cairo->overwritable_pixbuf;
}

void
gtk_image_tool_cairo_set_overwritable_pixbuf (GtkImageToolCairo *cairo,
                                              gboolean           overwritable_pixbuf)
{
    cairo->overwritable_pixbuf = overwritable_pixbuf;
}

GtkIImageCairoShaper*
gtk_image_tool_cairo_get_cairo_shaper (GtkImageToolCairo *cairo)
{
    return cairo->shaper;
}

void
gtk_image_tool_cairo_set_cairo_shaper (GtkImageToolCairo *cairo,
                                       GtkIImageCairoShaper *shaper)
{
    cairo->shaper = shaper;
}

