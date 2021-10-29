#include <cerver/utils/log.h>

#include "version.h"

// print full service version information
void service_version_print_full (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Watch Service Version: %s", SERVICE_VERSION_NAME
	);

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Release Date & time: %s - %s",
		SERVICE_VERSION_DATE, SERVICE_VERSION_TIME
	);

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Author: %s\n", SERVICE_VERSION_AUTHOR
	);

}

// print the version id
void service_version_print_version_id (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Watch Service Version ID: %s\n", SERVICE_VERSION
	);

}

// print the version name
void service_version_print_version_name (void) {

	cerver_log_both (
		LOG_TYPE_NONE, LOG_TYPE_NONE,
		"Watch Service Version: %s\n", SERVICE_VERSION_NAME
	);

}
