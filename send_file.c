/*
 * Send a file to each connected user.
 *
 * TODO: detect buddy and don't send it to ourselves.
 */


#include "common.h"

#include <glib.h>
#include <purple.h>
#include <string.h>
#include <unistd.h>


static void
send_file(PurpleBuddy *buddy)
{

	PurpleAccount *account = purple_buddy_get_account(buddy);
	PurpleConnection *gc = purple_account_get_connection(account);
	PurplePlugin *prpl = purple_connection_get_prpl(gc);
	PurplePluginProtocolInfo *prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(prpl);
	char file_name[256];
	char *res;

	printf("\nSending file to %s ... \n", buddy->name);
	printf("Type path of file to send: ");
	res = fgets(file_name, sizeof(file_name), stdin);
	file_name[strlen(file_name) - 1] = 0;  /* strip the \n at the end */
	if (!res) {
		fprintf(stderr, "Failed to read file path.");
		abort();
	}
	prpl_info->send_file(gc, buddy->name, file_name);
}

static void
signed_on(PurpleConnection *gc, gpointer null)
{
	PurpleAccount *account = purple_connection_get_account(gc);
	printf("Account connected: %s\n", account->username);
}

static void
connect_to_signals_for_demonstration_purposes_only(void)
{
	static int handle;
	purple_signal_connect(purple_connections_get_handle(), "signed-on", &handle,
				PURPLE_CALLBACK(signed_on), NULL);
	purple_signal_connect(purple_blist_get_handle(), "buddy-signed-on", &handle,
				PURPLE_CALLBACK(send_file), NULL);

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



