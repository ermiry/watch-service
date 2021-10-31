#ifndef _SERVICE_WORKER_H_
#define _SERVICE_WORKER_H_

#define WORKER_WAIT_TIME			1

#define WORKER_HASH_INPUT_SIZE		256

extern unsigned int worker_current_init (void);

extern unsigned int worker_current_end (void);

#endif