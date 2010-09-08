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
/* dinle-session-handler-ready.h */

#ifndef _DINLE_SESSION_HANDLER_READY_H
#define _DINLE_SESSION_HANDLER_READY_H

#include "dinle-session-handler.h"

G_BEGIN_DECLS

#define DINLE_TYPE_SESSION_HANDLER_READY dinle_session_handler_ready_get_type()

#define DINLE_SESSION_HANDLER_READY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_SESSION_HANDLER_READY, DinleSessionHandlerReady))

#define DINLE_SESSION_HANDLER_READY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_SESSION_HANDLER_READY, DinleSessionHandlerReadyClass))

#define DINLE_IS_SESSION_HANDLER_READY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_SESSION_HANDLER_READY))

#define DINLE_IS_SESSION_HANDLER_READY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_SESSION_HANDLER_READY))

#define DINLE_SESSION_HANDLER_READY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_SESSION_HANDLER_READY, DinleSessionHandlerReadyClass))

typedef struct _DinleSessionHandlerReady DinleSessionHandlerReady;
typedef struct _DinleSessionHandlerReadyClass DinleSessionHandlerReadyClass;
typedef struct _DinleSessionHandlerReadyPrivate DinleSessionHandlerReadyPrivate;

struct _DinleSessionHandlerReady
{
    DinleSessionHandler parent;

    DinleSessionHandlerReadyPrivate *priv;
};

struct _DinleSessionHandlerReadyClass
{
    DinleSessionHandlerClass parent_class;
};

GType dinle_session_handler_ready_get_type (void) G_GNUC_CONST;

DinleSessionHandler *dinle_session_handler_ready_new (void);

G_END_DECLS

#endif /* _DINLE_SESSION_HANDLER_READY_H */
