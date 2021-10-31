#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cerver/timer.h>

#include <cerver/threads/thread.h>
#include <cerver/threads/worker.h>

#include <cerver/utils/log.h>
#include <cerver/utils/sha256.h>

#include <credis/crud.h>
#include <credis/redis.h>

#include "service.h"
#include "worker.h"

#include "controllers/transactions.h"

static void *worker_handler_thread (void *null_ptr);

unsigned int worker_current_init (void) {

	unsigned int retval = 1;

	pthread_t thread_id = 0;
	if (!thread_create_detachable (&thread_id, worker_handler_thread, NULL)) {
		retval = 0;
	}

	return retval;

}

unsigned int worker_current_end (void) {

	return 0;

}

static double worker_handler_method_actual_work (
	Transaction *trans
) {

	size_t hash_input_len = 0;
	char hash_input[WORKER_HASH_INPUT_SIZE] = { 0 };

	double start_time = timer_get_current_time ();

	hash_input_len = (size_t) snprintf (
		hash_input, WORKER_HASH_INPUT_SIZE - 1,
		"%s-%ld",
		trans->id, trans->date
	);

	#ifdef SERVICE_DEBUG
	(void) printf ("Hash input: %s\n", hash_input);
	#endif

	sha256_generate (trans->result, hash_input, hash_input_len);

	return timer_get_current_time () - start_time;

}

static void worker_handler_method (void *data_ptr) {

	Transaction *trans = (Transaction *) data_ptr;

	double start_time = timer_get_current_time ();
	double waiting_time = 0;
	double process_time = 0;
	double complete_time = 0;

	// service_data_set_current_worker_trans (trans->id);

	cerver_log_success ("Got trans %s in worker!", trans->id);

	waiting_time = start_time - trans->worker_time;

	// update waiting time stats
	// service_data_update_waiting_time (waiting_time);

	cerver_log_debug (
		"Trans %s waited for %fs in WORKER queue",
		trans->id, waiting_time
	);

	trans->waiting_time = waiting_time;

	// do actual work
	process_time = worker_handler_method_actual_work (trans);

	trans->process_time = process_time;

	// update service stats
	// service_data_update_process_time (process_time);

	cerver_log_success (
		"Worker done with trans %s in %fs",
		trans->id, process_time
	);

	// remove current worker trans
	// service_data_clean_current_worker_trans ();

	// set complete process time
	complete_time = timer_get_current_time () - start_time;

	trans->complete_time = complete_time;

	// update complete process times
	// service_data_update_complete_time (complete_time);

	// service_data_update_processed_trans_count ();

	// update trans values in db
	(void) transaction_update_result (trans);

	cerver_log_success (
		"Trans %s complete work took %fs", trans->id, complete_time
	);

}

static void *worker_handler_thread (void *null_ptr) {

	CredisClient *client = NULL;
	redisReply *reply = NULL;

	do {
		client = credis_client_get ();
		if (client) {
			reply = (redisReply *) redisCommand (
				client->redis_context,
				"blpop worker:queue %d", WORKER_WAIT_TIME
			);

			if (reply) {
				cerver_log_debug (
					"worker_handler_thread () - "
					"Fetched %lu elements!", reply->elements
				);

				for (size_t idx = 0; idx < reply->elements; idx++) {
					(void) printf (
						"[%lu]: %d - %s\n",
						idx, reply->element[idx]->type, reply->element[idx]->str
					);
				}

				// TODO: do actual work

				freeReplyObject (reply);
			}

			credis_client_return (client);
		}
	} while (running);

	return NULL;

}
