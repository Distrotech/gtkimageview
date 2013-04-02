#ifndef __GTK_IMAGE_TOOL_SELECTOR_H__
#define __GTK_IMAGE_TOOL_SELECTOR_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include "gtkiimagetool.h"
#include "gtkimageview.h"
#include "mouse_handler.h"

G_BEGIN_DECLS

#define GTK_TYPE_IMAGE_TOOL_SELECTOR            (gtk_image_tool_selector_get_type ())
#define GTK_IMAGE_TOOL_SELECTOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IMAGE_TOOL_SELECTOR, GtkImageToolSelector))
#define GTK_IMAGE_TOOL_SELECTOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IMAGE_TOOL_SELECTOR, GtkImageToolSelectorClass))
#define GTK_IS_IMAGE_TOOL_SELECTOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IMAGE_TOOL_SELECTOR))
#define GTK_IS_IMAGE_TOOL_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IMAGE_TOOL_SELECTOR))
#define GTK_IMAGE_TOOL_SELECTOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IMAGE_TOOL_SELECTOR, GtkImageToolSelectorClass))

typedef struct _GtkImageToolSelector GtkImageToolSelector;
typedef struct _GtkImageToolSelectorClass GtkImageToolSelectorClass;

typedef enum
{
    HOTSPOT_INSIDE = 0,
    HOTSPOT_RESIZE_NORTH_WEST,
    HOTSPOT_RESIZE_NORTH_EAST,
    HOTSPOT_RESIZE_SOUTH_WEST,
    HOTSPOT_RESIZE_SOUTH_EAST,
    HOTSPOT_RESIZE_NORTH,
    HOTSPOT_RESIZE_EAST,
    HOTSPOT_RESIZE_SOUTH,
    HOTSPOT_RESIZE_WEST,
    HOTSPOT_OUTSIDE,
    HOTSPOT_LAST
} HotspotType;

typedef struct
{
    /* Area of the hotspot in image space coordinates. */
    GdkRectangle     rect;

    /* Cursor to use when the pointer is over the hotspot area. */
    GdkCursor       *cursor;
} Hotspot;

struct _GtkImageToolSelector
{
    GObject             parent;
    GtkImageView       *view;

    /* A darkened version of the views pixbuf. */
    GdkPixbuf          *background;

    /* Currently selected rectangle in image space coordinates. */
    GdkRectangle        sel_rect;

    /* Caches for the shaded area and the selection. */
    GdkPixbufDrawCache *bg_cache;
    GdkPixbufDrawCache *fg_cache;

    GdkCursor          *drag_cursor;

    /* For dragging the selection rectangle. */
    MouseHandler       *mouse_handler;

    /* ID of the currently running autoscroll timer. */
    int                 timer_id;

    /* Initial drag start zoom space coordinate. */
    GdkRectangle        sel_drag_start;

    /* How much outside the cursor is from the view. Stored for the
       benefit of the autoscroll_cb() function. */
    int                 outside_x;
    int                 outside_y;

    /* List of hotspots in the widget which can be pressed to drag or
       resize the selection. */
    Hotspot             hotspots[HOTSPOT_LAST];

    /* Type of the currently grabbed hotspot. */
    int                 hotspot_type;
};

struct _GtkImageToolSelectorClass
{
    GObjectClass parent;

    void (* selection_changed)               (GtkImageToolSelector *selector);
};

GType         gtk_image_tool_selector_get_type    (void);

/* Constructors */
GtkIImageTool *gtk_image_tool_selector_new        (GtkImageView *view);

/* Read-write properties */
void gtk_image_tool_selector_get_selection (GtkImageToolSelector *selector,
                                            GdkRectangle         *rect);
void gtk_image_tool_selector_set_selection (GtkImageToolSelector *selector,
                                            GdkRectangle         *rect);

G_END_DECLS

#endif
