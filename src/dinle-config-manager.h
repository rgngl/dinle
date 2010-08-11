/* dinle-config-manager.h */

#ifndef _DINLE_CONFIG_MANAGER_H
#define _DINLE_CONFIG_MANAGER_H

#include <glib-object.h>

G_BEGIN_DECLS

#define DINLE_TYPE_CONFIG_MANAGER dinle_config_manager_get_type()

#define DINLE_CONFIG_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  DINLE_TYPE_CONFIG_MANAGER, DinleConfigManager))

#define DINLE_CONFIG_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  DINLE_TYPE_CONFIG_MANAGER, DinleConfigManagerClass))

#define DINLE_IS_CONFIG_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  DINLE_TYPE_CONFIG_MANAGER))

#define DINLE_IS_CONFIG_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  DINLE_TYPE_CONFIG_MANAGER))

#define DINLE_CONFIG_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  DINLE_TYPE_CONFIG_MANAGER, DinleConfigManagerClass))

typedef struct _DinleConfigManager DinleConfigManager;
typedef struct _DinleConfigManagerClass DinleConfigManagerClass;
typedef struct _DinleConfigManagerPrivate DinleConfigManagerPrivate;

struct _DinleConfigManager
{
    GObject parent;

    DinleConfigManagerPrivate *priv;
};

struct _DinleConfigManagerClass
{
    GObjectClass parent_class;
};

GType dinle_config_manager_get_type (void) G_GNUC_CONST;

DinleConfigManager *dinle_config_manager_get (void);
gboolean dinle_config_manager_load_file (gchar *file);

G_END_DECLS

#endif /* _DINLE_CONFIG_MANAGER_H */
