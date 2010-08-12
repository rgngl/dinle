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
/* dinle-config-manager.c */

#include "dinle-config-manager.h"

#include <glib/gprintf.h>

G_DEFINE_TYPE (DinleConfigManager, dinle_config_manager, G_TYPE_OBJECT)

#define CONFIG_MANAGER_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), DINLE_TYPE_CONFIG_MANAGER, DinleConfigManagerPrivate))

#define BASIC_SETTINGS_GROUP "basic"
#define MEDIA_ROOT_KEY "mediaroot"

static DinleConfigManager *instance = NULL;

struct _DinleConfigManagerPrivate
{
    gchar *media_root;
};

enum {
    PROP_0,

    PROP_MEDIA_ROOT,

    PROP_NUMBER
};


/*prototypes for private functions.*/
static DinleConfigManager *dinle_config_manager_new(void);
static void update_config(GKeyFile *kf);

DinleConfigManager *
dinle_config_manager_get (void)
{
    if (!instance || !DINLE_IS_CONFIG_MANAGER(instance)) {
        instance = dinle_config_manager_new ();
    }

    return instance;
}

gboolean
dinle_config_manager_load_file (const gchar *file)
{
    if (!DINLE_IS_CONFIG_MANAGER(instance))
        instance = dinle_config_manager_new ();

    GKeyFile *kf = g_key_file_new ();
    GKeyFileFlags flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
    GError *error = NULL;
    gboolean success = FALSE;

    g_print ("Trying to load configuration from: %s\n", file);
    if (g_file_test (file, G_FILE_TEST_EXISTS) ) {
        if (g_key_file_load_from_file(kf, file, flags, &error)) {
            g_print ("Loaded %s\n", file);
            update_config(kf);
            success = TRUE;
        } else {
            g_print ("Failed loading configuration from %s\n", file);
        }
    }
    g_key_file_free (kf);

    return success;
}

static void
dinle_config_manager_get_property (GObject    *object,
                                   guint       property_id,
                                   GValue     *value,
                                   GParamSpec *pspec)
{
    DinleConfigManagerPrivate *priv = CONFIG_MANAGER_PRIVATE(object);

    switch (property_id)
    {
        case PROP_MEDIA_ROOT:
            g_value_set_string (value, priv->media_root);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

/*static void*/
/*dinle_config_manager_set_property (GObject      *object,*/
                                   /*guint         property_id,*/
                                   /*const GValue *value,*/
                                   /*GParamSpec   *pspec)*/
/*{*/
    /*switch (property_id)*/
    /*{*/
        /*default:*/
            /*G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);*/
    /*}*/
/*}*/

static void
dinle_config_manager_dispose (GObject *object)
{
    G_OBJECT_CLASS (dinle_config_manager_parent_class)->dispose (object);
}

static void
dinle_config_manager_finalize (GObject *object)
{
    G_OBJECT_CLASS (dinle_config_manager_parent_class)->finalize (object);
}

static void
dinle_config_manager_class_init (DinleConfigManagerClass *klass)
{
    GParamSpec *pspec = NULL;
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (DinleConfigManagerPrivate));

    object_class->get_property = dinle_config_manager_get_property;
    /*object_class->set_property = dinle_config_manager_set_property;*/
    object_class->dispose = dinle_config_manager_dispose;
    object_class->finalize = dinle_config_manager_finalize;

    pspec = g_param_spec_string ("media-root",
            "Media Root",
            "Where the daemon is looking for files",
            NULL /* default value */,
            G_PARAM_READABLE);
    g_object_class_install_property (object_class,
            PROP_MEDIA_ROOT,
            pspec);

}

static void
dinle_config_manager_init (DinleConfigManager *self)
{
    self->priv = CONFIG_MANAGER_PRIVATE (self);
    
    self->priv->media_root = NULL;
}

static DinleConfigManager *
dinle_config_manager_new (void)
{
    return g_object_new (DINLE_TYPE_CONFIG_MANAGER, NULL);
}

static void
update_config(GKeyFile *kf)
{
    DinleConfigManagerPrivate *priv = CONFIG_MANAGER_PRIVATE (instance);

    gchar *mr = g_key_file_get_string (kf, BASIC_SETTINGS_GROUP, MEDIA_ROOT_KEY, NULL);
    if (mr) {
        priv->media_root = mr;
        g_print("Media root is: %s\n", priv->media_root);
    }
}
