/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * Copyright © 2007 Björn Lindqvist <bjourne@gmail.com>
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
#ifndef __ZOOMS_H__
#define __ZOOMS_H__

#include <glib.h>

G_BEGIN_DECLS

gdouble         gtk_zooms_get_zoom_in (gdouble zoom);
gdouble         gtk_zooms_get_zoom_out (gdouble zoom);
gdouble         gtk_zooms_get_min_zoom (void);
gdouble         gtk_zooms_get_max_zoom (void);
gdouble         gtk_zooms_clamp_zoom (gdouble zoom);

G_END_DECLS

#endif
