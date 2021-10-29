#ifndef _SERVICE_H_
#define _SERVICE_H_

#include <stdbool.h>

#include "runtime.h"

extern RuntimeType RUNTIME;

extern unsigned int PORT;

extern unsigned int CERVER_RECEIVE_BUFFER_SIZE;
extern unsigned int CERVER_TH_THREADS;
extern unsigned int CERVER_CONNECTION_QUEUE;

// inits service main values
extern unsigned int service_init (void);

// ends service main values
extern unsigned int service_end (void);

#endif