#ifndef _CONTROLLERS_TRANSACTIONS_H_
#define _CONTROLLERS_TRANSACTIONS_H_

#include <bson/bson.h>

#include <cerver/types/string.h>

#include <cerver/collections/pool.h>

#include "errors.h"

#include "models/transaction.h"

#define DEFAULT_TRANS_POOL_INIT			32

extern Pool *trans_pool;

extern const bson_t *trans_no_user_query_opts;

extern unsigned int service_trans_init (void);

extern Transaction *service_trans_get (void);

extern unsigned int service_trans_get_all (
	char **json, size_t *json_len
);

extern Transaction *service_trans_get_by_id (
	const char *trans_id
);

extern u8 service_trans_get_by_id_to_json (
	const char *trans_id,
	const bson_t *query_opts,
	char **json, size_t *json_len
);

extern void service_trans_return (void *trans_ptr);

extern void service_trans_end (void);

#endif