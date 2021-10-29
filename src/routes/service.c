#include <stdlib.h>

#include <cerver/http/http.h>
#include <cerver/http/request.h>
#include <cerver/http/response.h>

#include "controllers/service.h"

// GET /api/watch
void watch_main_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	(void) http_response_send (watch_works, http_receive);

}

// GET /api/watch/version
void watch_version_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	(void) http_response_send (current_version, http_receive);

}

// GET *
void watch_catch_all_handler (
	const HttpReceive *http_receive,
	const HttpRequest *request
) {

	(void) http_response_send (not_found_error, http_receive);

}
