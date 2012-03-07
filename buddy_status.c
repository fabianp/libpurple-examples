/*
 * Print status of buddies when they log in/out.
 *
 * Author: Fabian Pedregosa <fabian@fseoane.net>
 */


#include "common.h"

#include <glib.h>
#include <purple.h>
#include <unistd.h>
#include <string.h>


static void buddy_presence(PurpleBuddy *buddy)
{
	PurplePresence *presence = purple_buddy_get_presence(buddy);
	PurpleStatus *status = purple_presence_get_active_status(presence);
	printf("buddy %s is now %s\n", buddy->name, purple_status_get_id(status));
}

static void
connect_to_signals_for_demonstration_purposes_only(void)
{
	static int handle;
	purple_signal_connect(purple_blist_get_handle(), "buddy-signed-on", &handle,
				PURPLE_CALLBACK(buddy_presence), NULL);
	purple_signal_connect(purple_blist_get_handle(), "buddy-signed-off", &handle,
				PURPLE_CALLBACK(buddy_presence), NULL);
}

int main(int argc, char *argv[])
{
	GList *iter;
	int i, num;
	GList *names = NULL;
	const char *prpl;
	char name[128];
	char *password;
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	PurpleAccount *account;
	PurpleSavedStatus *status;
	char *res;

#ifndef _WIN32
	/* libpurple's built-in DNS resolution forks processes to perform
	 * blocking lookups without blocking the main process.  It does not
	 * handle SIGCHLD itself, so if the UI does not you quickly get an army
	 * of zombie subprocesses marching around.
	 */
	signal(SIGCHLD, SIG_IGN);
#endif

	init_libpurple();

	printf("libpurple initialized.\n");

	iter = purple_plugins_get_protocols();
	for (i = 0; iter; iter = iter->next) {
		PurplePlugin *plugin = iter->data;
		PurplePluginInfo *info = plugin->info;
		if (info && info->name) {
			printf("\t%d: %s\n", i++, info->name);
			names = g_list_append(names, info->id);
		}
	}
	printf("Select the protocol [0-%d]: ", i-1);
	res = fgets(name, sizeof(name), stdin);
	if (!res) {
		fprintf(stderr, "Failed to gets protocol selection.");
		abort();
	}
	sscanf(name, "%d", &num);
	prpl = g_list_nth_data(names, num);

	printf("Username: ");
	res = fgets(name, sizeof(name), stdin);
	if (!res) {
		fprintf(stderr, "Failed to read user name.");
		abort();
	}
	name[strlen(name) - 1] = 0;  /* strip the \n at the end */


	/* Create the account */
	account = purple_account_new(name, prpl);

	/* Get the password for the account */
	password = getpass("Password: ");
	purple_account_set_password(account, password);

	/* It's necessary to enable the account first. */
	purple_account_set_enabled(account, UI_ID, TRUE);

	/* Now, to connect the account(s), create a status and activate it. */
	status = purple_savedstatus_new(NULL, PURPLE_STATUS_AVAILABLE);
	purple_savedstatus_activate(status);

	connect_to_signals_for_demonstration_purposes_only();

	g_main_loop_run(loop);

	return 0;
}

