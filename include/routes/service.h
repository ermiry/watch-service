#ifndef _ROUTES_SERVICE_H_
#define _ROUTES_SERVICE_H_

struct _HttpReceive;
struct _HttpRequest;

// GET /api/watch
extern void watch_main_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET /api/watch/version
extern void watch_version_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

// GET *
extern void watch_catch_all_handler (
	const struct _HttpReceive *http_receive,
	const struct _HttpRequest *request
);

#endif