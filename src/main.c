#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <time.h>
#include <signal.h>

#include <cerver/cerver.h>
#include <cerver/version.h>

#include <cerver/http/http.h>
#include <cerver/http/route.h>

#include <cerver/utils/log.h>
#include <cerver/utils/utils.h>

#include "service.h"
#include "version.h"

#include "routes/service.h"

static Cerver *watch_service = NULL;

void end (int dummy) {

	if (watch_service) {
		cerver_stats_print (watch_service, false, false);
		(void) printf ("\nHTTP Cerver stats:\n");
		http_cerver_all_stats_print ((HttpCerver *) watch_service->cerver_data);
		(void) printf ("\n");
		cerver_teardown (watch_service);
	}

	service_end ();

	cerver_end ();

	#ifdef SERVICE_DEBUG
	(void) printf ("\n\nDone!\n\n");
	#endif

	exit (0);

}

static void watch_set_service_routes (HttpCerver *http_cerver) {

	/*** top level route ***/

	// GET /api/watch
	HttpRoute *main_route = http_route_create (REQUEST_METHOD_GET, "api/", watch_main_handler);
	http_cerver_route_register (http_cerver, main_route);

	// HEAD /api/watch
	http_route_set_handler (main_route, REQUEST_METHOD_HEAD, watch_main_handler);

	/*** service routes ***/

	// GET /api/watch/version
	HttpRoute *version_route = http_route_create (REQUEST_METHOD_GET, "version", watch_version_handler);
	http_route_child_add (main_route, version_route);

	// HEAD /api/watch/version
	http_route_set_handler (version_route, REQUEST_METHOD_HEAD, watch_version_handler);

}

static void start (void) {

	watch_service = cerver_create (
		CERVER_TYPE_WEB,
		"watch-service",
		(u16) PORT,
		PROTOCOL_TCP,
		false,
		CERVER_CONNECTION_QUEUE
	);

	if (watch_service) {
		/*** cerver configuration ***/
		cerver_set_alias (watch_service, "watch");

		cerver_set_receive_buffer_size (watch_service, CERVER_RECEIVE_BUFFER_SIZE);
		cerver_set_thpool_n_threads (watch_service, CERVER_TH_THREADS);
		cerver_set_handler_type (watch_service, CERVER_HANDLER_TYPE_THREADS);

		cerver_set_reusable_address_flags (watch_service, true);

		/*** web cerver configuration ***/
		HttpCerver *http_cerver = (HttpCerver *) watch_service->cerver_data;

		watch_set_service_routes (http_cerver);

		// add a catch all route
		http_cerver_set_catch_all_route (http_cerver, watch_catch_all_handler);

		// admin configuration
		http_cerver_enable_admin_routes (http_cerver, true);

		if (cerver_start (watch_service)) {
			cerver_log_error (
				"Failed to start %s!",
				watch_service->info->name
			);

			cerver_delete (watch_service);
		}
	}

	else {
		cerver_log_error ("Failed to create cerver!");

		cerver_delete (watch_service);
	}

}

int main (int argc, char const **argv) {

	srand ((unsigned int) time (NULL));

	// register to the quit signal
	(void) signal (SIGINT, end);
	(void) signal (SIGTERM, end);

	// to prevent SIGPIPE when writting to socket
	(void) signal (SIGPIPE, SIG_IGN);

	cerver_init ();

	cerver_version_print_full ();

	service_version_print_full ();

	if (!service_init ()) {
		start ();
	}

	else {
		cerver_log_error ("Failed to init watch!");
	}

	service_end ();

	cerver_end ();

	return 0;

}
