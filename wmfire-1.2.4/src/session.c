/******************************************/
/* Session Management                     */
/******************************************/

/******************************************/
/* This program is free software; you can redistribute it and/or
/* modify it under the terms of the GNU General Public License
/* as published by the Free Software Foundation; either version 2
/* of the License, or (at your option) any later version.
/*
/* This program is distributed in the hope that it will be useful,
/* but WITHOUT ANY WARRANTY; without even the implied warranty of
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/* GNU General Public License for more details.
/*
/* You should have received a copy of the GNU General Public License
/* along with this program; if not, write to the Free Software
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
/******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <gdk/gdk.h>
#include <X11/SM/SMlib.h>

#include "session.h"

static gchar *client_id;

static void
cb_smc_save_yourself(SmcConn smc_conn, SmPointer client_data, gint save_type,
		     gboolean shutdown, gint interact_style, gboolean fast)
{
	/* program_function_save_state(); */
	SmcSaveYourselfDone(smc_conn, True);
}

static void
cb_smc_die(SmcConn smc_conn, SmPointer client_data)
{
	SmcCloseConnection(smc_conn, 0, NULL);
	exit(0);
}

static void
cb_smc_save_complete(SmcConn smc_conn, SmPointer client_data)
{
}

static void
cb_smc_shutdown_cancelled(SmcConn smc_conn, SmPointer client_data)
{
}

static void
cb_ice_connection_messages(IceConn ice_connection, gint source, GdkInputCondition condition)
{
	IceProcessMessages(ice_connection, NULL, NULL);
}

void
smc_connect(gint argc, gchar ** argv, gchar * session_id)
{
	SmProp userid, program, restart, clone, pid, *props[5];
	SmPropValue userid_val, pid_val;
	SmcCallbacks *callbacks;
	SmcConn smc_connection;
	IceConn ice_connection;
	struct passwd *pwd;
	gchar error_string[256], pid_str[8], *userid_str;
	gulong mask;
	gint i, j;

	/* Session manager callbacks */
	callbacks = g_new0(SmcCallbacks, 1);
	callbacks->save_yourself.callback = cb_smc_save_yourself;
	callbacks->die.callback = cb_smc_die;
	callbacks->save_complete.callback = cb_smc_save_complete;
	callbacks->shutdown_cancelled.callback = cb_smc_shutdown_cancelled;

	mask = SmcSaveYourselfProcMask | SmcDieProcMask | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask;

	smc_connection = SmcOpenConnection(NULL /* SESSION_MANAGER env variable */ ,
					   NULL /* share ICE connection */ ,
					   SmProtoMajor, SmProtoMinor, mask,
					   callbacks, session_id, &client_id,
					   sizeof (error_string), error_string);
	g_free(callbacks);
	if (!smc_connection)
		return;

	gdk_set_sm_client_id(client_id);

	/* Session manager properties */
	userid.name = SmUserID;
	userid.type = SmARRAY8;
	userid.num_vals = 1;
	userid.vals = &userid_val;
	pwd = getpwuid(getuid());
	userid_str = pwd->pw_name;
	userid_val.value = (SmPointer) userid_str;
	userid_val.length = strlen(userid_str);

	pid.name = SmProcessID;
	pid.type = SmARRAY8;
	pid.num_vals = 1;
	pid.vals = &pid_val;
	sprintf(pid_str, "%i", getpid());
	pid_val.value = (SmPointer) pid_str;
	pid_val.length = strlen(pid_str);

	restart.name = SmRestartCommand;
	restart.type = SmLISTofARRAY8;
	restart.vals = g_new0(SmPropValue, argc + 2);
	j = 0;
	for (i = 0; i < argc; ++i) {
		if ( strcmp(argv[i], "-S") ) {
			restart.vals[j].value = (SmPointer) argv[i];
			restart.vals[j++].length = strlen(argv[i]);
		} else
			i++;
	}
	restart.vals[j].value = (SmPointer) "-S";
	restart.vals[j++].length = 2;
	restart.vals[j].value = (SmPointer) client_id;
	restart.vals[j++].length = strlen(client_id);
	restart.num_vals = j;

	clone.name = SmCloneCommand;
	clone.type = SmLISTofARRAY8;
	clone.vals = restart.vals;
	clone.num_vals = restart.num_vals - 2;

	program.name = SmProgram;
	program.type = SmARRAY8;
	program.vals = restart.vals;
	program.num_vals = 1;

	props[0] = &program;
	props[1] = &userid;
	props[2] = &restart;
	props[3] = &clone;
	props[4] = &pid;

	SmcSetProperties(smc_connection, 5, props);
	g_free(restart.vals);

	ice_connection = SmcGetIceConnection(smc_connection);
	gdk_input_add(IceConnectionNumber(ice_connection), GDK_INPUT_READ,
		      (GdkInputFunction) cb_ice_connection_messages, ice_connection);
}
