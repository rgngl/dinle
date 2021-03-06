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
/* dinle-session.h */

#ifndef _DINLE_SESSION_H
#define _DINLE_SESSION_H

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define DINLE_TYPE_SESSION dinle_session_get_type()

#define DINLE_SESSION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_SESSION, DinleSession))

#define DINLE_SESSION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_SESSION, DinleSessionClass))

#define DINLE_IS_SESSION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_SESSION))

#define DINLE_IS_SESSION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_SESSION))

#define DINLE_SESSION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_SESSION, DinleSessionClass))

typedef struct _DinleSession DinleSession;
typedef struct _DinleSessionClass DinleSessionClass;
typedef struct _DinleSessionPrivate DinleSessionPrivate;

struct _DinleSession
{
    GObject parent;

    DinleSessionPrivate *priv;
};

struct _DinleSessionClass
{
    GObjectClass parent_class;

    /* Signals */
    void (*done) (DinleSession *self, gpointer user_data);
    void (*activity) (DinleSession *self, gpointer user_data);
};

GType dinle_session_get_type (void) G_GNUC_CONST;

DinleSession *dinle_session_new (GSocketConnection *conn);
gboolean dinle_session_close (DinleSession *self);

G_END_DECLS

#endif /* _DINLE_SESSION_H */
