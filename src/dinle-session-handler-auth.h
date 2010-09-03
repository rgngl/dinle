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
/* dinle-session-handler-auth.h */

#ifndef _DINLE_SESSION_HANDLER_AUTH_H
#define _DINLE_SESSION_HANDLER_AUTH_H

#include "dinle-session-handler.h"

G_BEGIN_DECLS

#define DINLE_TYPE_SESSION_HANDLER_AUTH dinle_session_handler_auth_get_type()

#define DINLE_SESSION_HANDLER_AUTH(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_SESSION_HANDLER_AUTH, DinleSessionHandlerAuth))

#define DINLE_SESSION_HANDLER_AUTH_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_SESSION_HANDLER_AUTH, DinleSessionHandlerAuthClass))

#define DINLE_IS_SESSION_HANDLER_AUTH(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_SESSION_HANDLER_AUTH))

#define DINLE_IS_SESSION_HANDLER_AUTH_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_SESSION_HANDLER_AUTH))

#define DINLE_SESSION_HANDLER_AUTH_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_SESSION_HANDLER_AUTH, DinleSessionHandlerAuthClass))

typedef struct _DinleSessionHandlerAuth DinleSessionHandlerAuth;
typedef struct _DinleSessionHandlerAuthClass DinleSessionHandlerAuthClass;
typedef struct _DinleSessionHandlerAuthPrivate DinleSessionHandlerAuthPrivate;

struct _DinleSessionHandlerAuth
{
  DinleSessionHandler parent;

  DinleSessionHandlerAuthPrivate *priv;
};

struct _DinleSessionHandlerAuthClass
{
  DinleSessionHandlerClass parent_class;
};

GType dinle_session_handler_auth_get_type (void) G_GNUC_CONST;

DinleSessionHandlerAuth *dinle_session_handler_auth_new (void);

G_END_DECLS

#endif /* _DINLE_SESSION_HANDLER_AUTH_H */
