#include <stdlib.h>

#include <time.h>

#include <cerver/types/string.h>

#include <cerver/collections/pool.h>

#include <cerver/timer.h>

#include <cerver/utils/log.h>

#include <cmongo/crud.h>
#include <cmongo/select.h>

#include "errors.h"

#include "models/transaction.h"

#include "controllers/transactions.h"

Pool *trans_pool = NULL;

const bson_t *trans_no_user_query_opts = NULL;
static CMongoSelect *trans_no_user_select = NULL;

void service_trans_return (void *trans_ptr);

static unsigned int service_trans_init_pool (void) {

	unsigned int retval = 1;

	trans_pool = pool_create (transaction_delete);
	if (trans_pool) {
		pool_set_create (trans_pool, transaction_new);
		pool_set_produce_if_empty (trans_pool, true);
		if (!pool_init (trans_pool, transaction_new, DEFAULT_TRANS_POOL_INIT)) {
			retval = 0;
		}

		else {
			cerver_log_error ("Failed to init trans pool!");
		}
	}

	else {
		cerver_log_error ("Failed to create trans pool!");
	}

	return retval;

}

static unsigned int service_trans_init_query_opts (void) {

	unsigned int retval = 1;

	trans_no_user_select = cmongo_select_new ();
	(void) cmongo_select_insert_field (trans_no_user_select, "title");
	(void) cmongo_select_insert_field (trans_no_user_select, "amount");
	(void) cmongo_select_insert_field (trans_no_user_select, "date");

	trans_no_user_query_opts = mongo_find_generate_opts (trans_no_user_select);

	if (trans_no_user_query_opts) retval = 0;

	return retval;

}

unsigned int service_trans_init (void) {

	unsigned int errors = 0;

	errors |= service_trans_init_pool ();

	errors |= service_trans_init_query_opts ();

	return errors;

}

Transaction *service_trans_get (void) {

	return (Transaction *) pool_pop (trans_pool);

}

unsigned int service_trans_get_all (
	char **json, size_t *json_len
) {

	return transactions_get_all_to_json (
		trans_no_user_query_opts,
		json, json_len
	);

}

Transaction *service_trans_get_by_id (
	const char *trans_id
) {

	Transaction *trans = NULL;

	if (trans_id) {
		trans = (Transaction *) pool_pop (trans_pool);
		if (trans) {
			bson_oid_init_from_string (&trans->oid, trans_id);

			if (transaction_get_by_oid (
				trans,
				&trans->oid,
				NULL
			)) {
				service_trans_return (trans);
				trans = NULL;
			}
		}
	}

	return trans;

}

u8 service_trans_get_by_id_to_json (
	const char *trans_id,
	const bson_t *query_opts,
	char **json, size_t *json_len
) {

	u8 retval = 1;

	if (trans_id) {
		bson_oid_t trans_oid = { 0 };
		bson_oid_init_from_string (&trans_oid, trans_id);

		retval = transaction_get_by_oid_to_json (
			&trans_oid,
			query_opts,
			json, json_len
		);
	}

	return retval;

}

void service_trans_return (void *trans_ptr) {

	if (trans_ptr) {
		(void) memset (trans_ptr, 0, sizeof (Transaction));
		(void) pool_push (trans_pool, trans_ptr);
	}

}

void service_trans_end (void) {

	cmongo_select_delete (trans_no_user_select);
	bson_destroy ((bson_t *) trans_no_user_query_opts);

	pool_delete (trans_pool);
	trans_pool = NULL;

}
