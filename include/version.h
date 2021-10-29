#ifndef _SERVICE_VERSION_H_
#define _SERVICE_VERSION_H_

#define SERVICE_VERSION				"0.1"
#define SERVICE_VERSION_NAME		"Version 0.1"
#define SERVICE_VERSION_DATE		"28/10/2021"
#define SERVICE_VERSION_TIME		"23:46 CST"
#define SERVICE_VERSION_AUTHOR		"Erick Salas"

// print full service version information
extern void service_version_print_full (void);

// print the version id
extern void service_version_print_version_id (void);

// print the version name
extern void service_version_print_version_name (void);

#endif