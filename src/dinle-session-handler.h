/*
    This file is part of Dinle.
    Copyright 2010  Üstün Ergenoglu

    Dinle is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dinle is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dinle.  If not, see <http://www.gnu.org/licenses/>.
*/
/* dinle-session-handler.h */

#ifndef _DINLE_SESSION_HANDLER_H
#define _DINLE_SESSION_HANDLER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define DINLE_TYPE_SESSION_HANDLER dinle_session_handler_get_type()

#define DINLE_SESSION_HANDLER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_SESSION_HANDLER, DinleSessionHandler))

#define DINLE_SESSION_HANDLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_SESSION_HANDLER, DinleSessionHandlerClass))

#define DINLE_IS_SESSION_HANDLER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_SESSION_HANDLER))

#define DINLE_IS_SESSION_HANDLER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_SESSION_HANDLER))

#define DINLE_SESSION_HANDLER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_SESSION_HANDLER, DinleSessionHandlerClass))

typedef struct _DinleSessionHandler DinleSessionHandler;
typedef struct _DinleSessionHandlerClass DinleSessionHandlerClass;
typedef struct _DinleSessionHandlerPrivate DinleSessionHandlerPrivate;

struct _DinleSessionHandler
{
    GObject parent;

    DinleSessionHandlerPrivate *priv;
};

struct _DinleSessionHandlerClass
{
    GObjectClass parent_class;

    gboolean (*process) (DinleSessionHandler *self, gchar *data, gsize len);

    /* Signals */
    void (*done) (DinleSessionHandler *self, gboolean success, gpointer user_data);
};

GType dinle_session_handler_get_type (void) G_GNUC_CONST;

gboolean dinle_session_handler_process (DinleSessionHandler *self, gchar *data, gsize len);

G_END_DECLS

#endif /* _DINLE_SESSION_HANDLER_H */
