#ifndef __TESTLIB_H__
#define __TESTLIB_H__

#include <gtk/gtk.h>

void          fake_realize                   (GtkWidget *widget);
gboolean      g_main_context_wait_for_event  (GMainContext *context,
                                              int           timeout);

#endif
