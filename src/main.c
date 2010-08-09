#include "config.h"

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

GThread *main_task;
GMainLoop *main_loop;

static gboolean foreground = FALSE;

static GOptionEntry entries[] =
{
    /*{ "repeats", 'r', 0, G_OPTION_ARG_INT, &repeats, "Average over N repetitions", "N" },*/
    /*{ "max-size", 'm', 0, G_OPTION_ARG_INT, &max_size, "Test up to 2^M items", "M" },*/
    /*{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL },*/
    /*{ "beep", 'b', 0, G_OPTION_ARG_NONE, &beep, "Beep when done", NULL },*/
    /*{ "rand", 0, 0, G_OPTION_ARG_NONE, &rand, "Randomize the data", NULL },*/
    { "foreground", 'f', 0, G_OPTION_ARG_NONE, &foreground, "Stay in the foreground, do not daemonize", NULL},
    { NULL }
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

    main_task = g_thread_self();
    main_loop = g_main_loop_new(NULL, FALSE);

    if (!foreground) {
        int res = daemon(1, 1);
        if (res) {
            fprintf(stderr, "Cannot daemonize, what's wrong?\n");
            abort();
        }
    }

    g_main_loop_run(main_loop);

    return 0;
}
