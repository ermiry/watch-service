#include <stdlib.h>
#include <string.h>

#include <cerver/version.h>

#include <cerver/http/http.h>
#include <cerver/http/response.h>
#include <cerver/http/json/json.h>

#include <cerver/utils/log.h>
#include <cerver/utils/utils.h>

#include "version.h"

#include "controllers/service.h"

HttpResponse *missing_values = NULL;

HttpResponse *watch_works = NULL;
HttpResponse *current_version = NULL;

static unsigned int watch_service_init_responses (void) {

	unsigned int retval = 1;

	missing_values = http_response_json_key_value (
		HTTP_STATUS_BAD_REQUEST, "error", "Missing values!"
	);

	watch_works = http_response_json_key_value (
		HTTP_STATUS_OK, "msg", "Watch service works!"
	);

	char version[VERSION_BUFFER_SIZE] = { 0 };
	(void) snprintf (
		version, VERSION_BUFFER_SIZE - 1,
		"%s - %s",
		SERVICE_VERSION_NAME, SERVICE_VERSION_DATE
	);

	current_version = http_response_json_key_value (
		HTTP_STATUS_OK, "version", version
	);

	if (
		missing_values
		&& watch_works && current_version
	) retval = 0;

	return retval;

}

unsigned int watch_service_init (void) {

	unsigned int errors = 0;

	errors |= watch_service_init_responses ();

	return errors;

}

void watch_service_end (void) {

	http_response_delete (missing_values);

	http_response_delete (watch_works);
	http_response_delete (current_version);

}
