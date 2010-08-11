#include "config.h"
#include "dinle-config-manager.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <glib.h>

#define CONFIG_FILE "dinled.conf"
#define USER_CONFIG_DIR ".dinled"
#define SYSTEM_CONFIG_DIR "/etc"

GThread *main_task;
GMainLoop *main_loop;

static gboolean foreground = FALSE;
static gchar *music_root = NULL;
static gchar *config_file = NULL;

static GOptionEntry entries[] =
{
    { "foreground", 'f', 0, G_OPTION_ARG_NONE, &foreground, "Stay in the foreground, do not daemonize", NULL },
    { "config-file", 'c', 0, G_OPTION_ARG_STRING, &config_file, "Configuration file to load.", "FILE" },
    { "music-root", 'm', 0, G_OPTION_ARG_STRING, &music_root, "Root directory to search music, by passes configuration", "DIR" },
    { NULL, }
};

int main(int argc, char** argv)
{
    GError *error = NULL;
    GOptionContext *context;

    context = g_option_context_new ("- music server daemon");
    g_option_context_add_main_entries (context, entries, NULL);
    if (!g_option_context_parse (context, &argc, &argv, &error)) {
        g_print ("option parsing failed: %s\n", error->message);
        exit (1);
    }


#ifdef HAVE_LOCALE_H
    /* initialize locale */
    setlocale(LC_CTYPE,"");
#endif

    g_set_application_name(PACKAGE);

    /* enable GLib's thread safety code */
    g_thread_init(NULL);
    g_type_init();

    main_task = g_thread_self();
    main_loop = g_main_loop_new(NULL, FALSE);

    if (!foreground) {
        int res = daemon(1, 1);
        if (res) {
            fprintf(stderr, "Cannot daemonize, what's wrong?\n");
            abort();
        }
    }

    g_object_ref (dinle_config_manager_get());

    if (config_file)
        dinle_config_manager_load_file (config_file);
    else {
        /* Try to load default config files here. */
        gboolean config_loaded = FALSE;

        config_loaded |= dinle_config_manager_load_file (SYSTEM_CONFIG_DIR "/" CONFIG_FILE);

        const gchar *homedir = g_get_home_dir ();
        gchar *cf = g_strconcat (homedir, "/", USER_CONFIG_DIR, "/", CONFIG_FILE, NULL);
        config_loaded |= dinle_config_manager_load_file (cf);

        if (!config_loaded) {
            g_print ("No configuration found in default directories.\nUse one with the -c parameter.\n");
            abort();
        }
    }

    if (music_root)
        fprintf(stderr, "%s\n", music_root);

    g_main_loop_run(main_loop);

    return 0;
}
