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
#include "gtkiimagecairoshaper.h"

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
GType
gtk_iimage_cairo_shaper_get_type (void)
{
    static GType type = 0;
    if (type)
        return type;
    static const GTypeInfo info = {
        sizeof (GtkIImageCairoShaperClass),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        0,
        0,
        NULL
    };
    type = g_type_register_static (G_TYPE_INTERFACE,
                                   "GtkIImageCairoShaper",
                                   &info,
                                   0);
    g_type_interface_add_prerequisite (type, G_TYPE_OBJECT);
    return type;
}

void
gtk_iimage_cairo_shaper_start (GtkIImageCairoShaper *shaper,
                               int                   x,
                               int                   y)
{
    GTK_IIMAGE_CAIRO_SHAPER_GET_CLASS (shaper)->start (shaper, x, y);
}

void
gtk_iimage_cairo_shaper_motion (GtkIImageCairoShaper *shaper,
                                int                   x,
                                int                   y)
{
    GTK_IIMAGE_CAIRO_SHAPER_GET_CLASS (shaper)->motion (shaper, x, y);
}

void
gtk_iimage_cairo_shaper_reset (GtkIImageCairoShaper *shaper)
{
    GTK_IIMAGE_CAIRO_SHAPER_GET_CLASS (shaper)->reset (shaper);
}

void
gtk_iimage_cairo_shaper_get_damage_area (GtkIImageCairoShaper *shaper,
                                         GdkRectangle         *damage_area)
{
    GTK_IIMAGE_CAIRO_SHAPER_GET_CLASS (shaper)->
        get_damage_area (shaper, damage_area);
}

void          
gtk_iimage_cairo_shaper_draw (GtkIImageCairoShaper *shaper,
                              cairo_t              *cr)
{
    GTK_IIMAGE_CAIRO_SHAPER_GET_CLASS (shaper)->draw (shaper, cr);
}


