#include "testlib.h"

/**
 * fake_realize:
 *
 * A fake realization function for GtkImageView. The normal can not be
 * used because gdk does not like it.
 **/
void
fake_realize (GtkWidget *widget)
{
    GdkWindowAttr attrs;
    attrs.window_type = GDK_WINDOW_CHILD;
    attrs.x = widget->allocation.x;
    attrs.y = widget->allocation.y;
    attrs.width = widget->allocation.width;
    attrs.height = widget->allocation.height;
    attrs.wclass = GDK_INPUT_OUTPUT;
    attrs.visual = gtk_widget_get_visual (widget);
    attrs.colormap = gtk_widget_get_colormap (widget);
    attrs.event_mask = (gtk_widget_get_events (widget)
                        | GDK_EXPOSURE_MASK
                        | GDK_BUTTON_MOTION_MASK
                        | GDK_BUTTON_PRESS_MASK
                        | GDK_BUTTON_RELEASE_MASK
                        | GDK_POINTER_MOTION_MASK);
                        
    int attr_mask = (GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP);
    widget->window = gdk_window_new (NULL, &attrs, attr_mask);

    widget->style = gtk_style_attach (widget->style, widget->window);
    gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
}

/**
 * g_main_context_wait_for_event:
 * @context: a #GMainContext or %NULL to use the default context
 * @timeout: Maximum number of microseconds to wait
 * @returns: %TRUE if any source have pending events for the given
 *   context
 *
 * Waits up to timeout microseconds until any sources have pending
 * events for the given context. 
 * 
 **/
gboolean
g_main_context_wait_for_event (GMainContext *context,
                               int           timeout)
{
    int timeout_msecs = timeout / 1000;
    for (int n = 0; n < timeout_msecs; n++)
    {
        if (g_main_context_pending (context))
            return TRUE;
        g_usleep (1000);
    }
    return FALSE;
}
