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
 * SECTION:gtkzooms
 * @short_description: Functions for dealing with zoom factors
 *
 * <para>
 *   #GtkImageView uses a discrete amount of zoom factors for determining
 *   which zoom to set. Using these functions, it is possible to retrieve
 *   information and manipulate a zoom factor.
 * </para>
 **/
#include "gtkzooms.h"

static const gdouble ZOOMS[] = {
    0.02, 0.05, 0.07, 0.10,
    0.15, 0.20, 0.30, 0.50, 0.75, 1.0,
    1.5, 2.0, 3.0, 5.0, 7.5, 10.0,
    15.0, 20.0, 30.0, 50.0, 75.0, 100.0
};

/**
 * gtk_zooms_get_zoom_in:
 * @zoom: A zoom factor.
 * @returns: a zoom factor that is one step larger than the supplied
 *      one
 *
 * Returns the zoom factor that is one step larger than the supplied
 * zoom factor.
 **/
gdouble
gtk_zooms_get_zoom_in (gdouble zoom)
{
    int length = G_N_ELEMENTS(ZOOMS);
    for (int n = 0; n < length; n++)
    {
        gdouble this_zoom = ZOOMS[n];
        if (this_zoom > zoom)
            return this_zoom;
    }
    return ZOOMS[length - 1];
}

/**
 * gtk_zooms_get_zoom_out:
 * @zoom: a zoom factor
 * @returns: a zoom factor that is one step smaller than the supplied
 *      one.
 *
 * Returns the zoom factor that is one step smaller than the supplied
 * zoom factor.
 **/
gdouble
gtk_zooms_get_zoom_out (gdouble zoom)
{
    int length = G_N_ELEMENTS(ZOOMS);
    for (int n = length - 1; n >= 0; n--)
    {
        gdouble this_zoom = ZOOMS[n];
        if (this_zoom < zoom)
            return this_zoom;
    }
    return ZOOMS[0];
}

/**
 * gtk_zooms_get_min_zoom:
 * @returns: The minimal zoom factor.
 *
 * Returns the minimum allowed zoom factor.
 **/
gdouble
gtk_zooms_get_min_zoom (void)
{
    return ZOOMS[0];
}

/**
 * gtk_zooms_get_max_zoom:
 * @returns: The maximal zoom factor.
 *
 * Returns the maximum allowed zoom factor.
 **/
gdouble
gtk_zooms_get_max_zoom (void)
{
    return ZOOMS[G_N_ELEMENTS(ZOOMS) - 1];
}

/**
 * gtk_zooms_clamp_zoom:
 * @zoom: A zoom factor
 * @returns: The zoom factor clamped to the interval [min, max].
 *
 * Returns the zoom factor clamped to the minumum and maximum allowed
 * value.
 **/
gdouble
gtk_zooms_clamp_zoom (gdouble zoom)
{
    return CLAMP (zoom, gtk_zooms_get_min_zoom (), gtk_zooms_get_max_zoom ());
}
