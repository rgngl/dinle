/* dinle-session-manager.h */

#ifndef _DINLE_SESSION_MANAGER_H
#define _DINLE_SESSION_MANAGER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define DINLE_TYPE_SESSION_MANAGER dinle_session_manager_get_type()

#define DINLE_SESSION_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_SESSION_MANAGER, DinleSessionManager))

#define DINLE_SESSION_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_SESSION_MANAGER, DinleSessionManagerClass))

#define DINLE_IS_SESSION_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_SESSION_MANAGER))

#define DINLE_IS_SESSION_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_SESSION_MANAGER))

#define DINLE_SESSION_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_SESSION_MANAGER, DinleSessionManagerClass))

typedef struct _DinleSessionManager DinleSessionManager;
typedef struct _DinleSessionManagerClass DinleSessionManagerClass;
typedef struct _DinleSessionManagerPrivate DinleSessionManagerPrivate;

struct _DinleSessionManager
{
    GObject parent;

    DinleSessionManagerPrivate *priv;
};

struct _DinleSessionManagerClass
{
    GObjectClass parent_class;
};

GType dinle_session_manager_get_type (void) G_GNUC_CONST;

DinleSessionManager *dinle_session_manager_get (void);

G_END_DECLS

#endif /* _DINLE_SESSION_MANAGER_H */
