
/* Generated data (by glib-mkenums) */

#include "gtkimageview.h"
#include "gtkimagetoolselector.h"
#include "gtkimageview-typebuiltins.h"
/* enumerations from "gtkimagetoolselector.h" */
GType
hotspot_type_get_type (void)
{
  static GType etype = 0;
  if (etype == 0)  {
    static const GEnumValue values[] = {
      { HOTSPOT_INSIDE, "HOTSPOT_INSIDE", "inside" },
      { HOTSPOT_RESIZE_NORTH_WEST, "HOTSPOT_RESIZE_NORTH_WEST", "resize-north-west" },
      { HOTSPOT_RESIZE_NORTH_EAST, "HOTSPOT_RESIZE_NORTH_EAST", "resize-north-east" },
      { HOTSPOT_RESIZE_SOUTH_WEST, "HOTSPOT_RESIZE_SOUTH_WEST", "resize-south-west" },
      { HOTSPOT_RESIZE_SOUTH_EAST, "HOTSPOT_RESIZE_SOUTH_EAST", "resize-south-east" },
      { HOTSPOT_RESIZE_NORTH, "HOTSPOT_RESIZE_NORTH", "resize-north" },
      { HOTSPOT_RESIZE_EAST, "HOTSPOT_RESIZE_EAST", "resize-east" },
      { HOTSPOT_RESIZE_SOUTH, "HOTSPOT_RESIZE_SOUTH", "resize-south" },
      { HOTSPOT_RESIZE_WEST, "HOTSPOT_RESIZE_WEST", "resize-west" },
      { HOTSPOT_OUTSIDE, "HOTSPOT_OUTSIDE", "outside" },
      { HOTSPOT_LAST, "HOTSPOT_LAST", "last" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static (g_intern_static_string ("HotspotType"), values);
  }
  return etype;
}
/* enumerations from "gtkimageview.h" */
GType
gtk_image_transp_get_type (void)
{
  static GType etype = 0;
  if (etype == 0)  {
    static const GEnumValue values[] = {
      { GTK_IMAGE_TRANSP_COLOR, "GTK_IMAGE_TRANSP_COLOR", "color" },
      { GTK_IMAGE_TRANSP_BACKGROUND, "GTK_IMAGE_TRANSP_BACKGROUND", "background" },
      { GTK_IMAGE_TRANSP_GRID, "GTK_IMAGE_TRANSP_GRID", "grid" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static (g_intern_static_string ("GtkImageTransp"), values);
  }
  return etype;
}
/* enumerations from "gdkpixbufdrawcache.h" */
GType
gdk_pixbuf_draw_method_get_type (void)
{
  static GType etype = 0;
  if (etype == 0)  {
    static const GEnumValue values[] = {
      { GDK_PIXBUF_DRAW_METHOD_SCALE, "GDK_PIXBUF_DRAW_METHOD_SCALE", "scale" },
      { GDK_PIXBUF_DRAW_METHOD_CONTAINS, "GDK_PIXBUF_DRAW_METHOD_CONTAINS", "contains" },
      { GDK_PIXBUF_DRAW_METHOD_SCROLL, "GDK_PIXBUF_DRAW_METHOD_SCROLL", "scroll" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static (g_intern_static_string ("GdkPixbufDrawMethod"), values);
  }
  return etype;
}

/* Generated data ends here */

