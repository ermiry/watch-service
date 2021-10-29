#ifndef _CONTROLLERS_SERVICE_H_
#define _CONTROLLERS_SERVICE_H_

#define VERSION_BUFFER_SIZE		64

struct _HttpResponse;

extern struct _HttpResponse *missing_values;

extern struct _HttpResponse *watch_works;
extern struct _HttpResponse *current_version;

extern unsigned int watch_service_init (void);

extern void watch_service_end (void);

#endif