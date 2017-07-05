#ifndef __JSON_RPC_INC_
#define __JSON_RPC_INC_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#define E_SUCCESS           (0)
#define E_INVALID_REQUEST   (-1)
#define E_INVALID_METHOD    (-2)
#define E_NO_MEM            (-3)
#define E_UPS_LEFT_ONE      (-1000)
#define E_ARRAY_PUSH_NULL   (-1001)
#define E_ARRAY_PUSHN_NULL  (-1002)
#define E_ARRAY_CREATE_NULL (-1003)
#define E_PARSE_URL         (-1004)
#define E_SLAB_ALLOC        (-1005)
#define E_SLAB_CALLOC       (-1006)
#define E_FUNC_NA           (-1007)
#define E_NO_ALLOW          (-1008)
#define E_NO_UPS			(-1009)
const char *ups_errmsg(int proto);

typedef struct codetable_t {
	const char *key_str;
	int key_len;
	void *data;
}codetable_t;

static inline int tbl_cmp(const void *m1, const void *m2) {
	codetable_t *ct1 = (codetable_t *) m1;
	codetable_t *ct2 = (codetable_t *) m2;
	return strcmp(ct1->key_str, ct2->key_str);
}

static inline void tbl_sort(codetable_t *tbl, size_t size) {
	qsort(tbl, size, sizeof(codetable_t), tbl_cmp);
}

/*二分查找*/
static inline bool tbl_bsearch(codetable_t *tbl, size_t tbl_size, const char *key, void **data) {
	if(key == NULL) return false;
	codetable_t searchkey;
	//searchkey.key_str = strdup(key);
	searchkey.key_str = key;
	codetable_t *ret = bsearch(&searchkey, tbl, tbl_size, sizeof(codetable_t), tbl_cmp);
	//free(searchkey.key_str);
	if(ret == NULL) {
		return false;
	}
	if(data) {
		*data = ret->data;
	}
	return true;
}

#include <stdio.h>
#include "cJSON.h"

typedef cJSON json_t;
#define json_false  cJSON_False
#define json_true   cJSON_True
#define json_null   cJSON_NULL
#define json_number cJSON_Number
#define json_string cJSON_String
#define json_array  cJSON_Array
#define json_object cJSON_Object

static inline json_t *json_createarray() {
	return cJSON_CreateArray();
}
static inline json_t *json_create() {
	return cJSON_CreateObject();
}
static inline void json_delete(json_t *json) {
	cJSON_Delete(json);
}
static inline char *json_tostring(json_t *item) {
	return cJSON_PrintUnformatted(item);
}
static inline void json_addbool(json_t *object, const char *string, bool item) {
	cJSON_AddItemToObject(object, string, item ? cJSON_CreateTrue() :  cJSON_CreateFalse());
}
static inline void json_addnumber(json_t *object, const char *string, double item) {
	cJSON_AddNumberToObject(object, string, item);
}
static inline void json_addstring(json_t *object, const char *string, const char *item) {
	cJSON_AddStringToObject(object, string, item);
}
static inline void json_addobject(json_t *object, const char *string, json_t *item) {
	cJSON_AddItemToObject(object, string, item);
}
static inline void json_addarray(json_t *array, json_t *item) {
	cJSON_AddItemToArray(array, item);
}
static inline bool is_json_string(json_t *json) {
	if(json == NULL) return false;
	return (json->type == cJSON_String);
}
static inline bool is_json_number(json_t *json) {
	if(json == NULL) return false;
	return (json->type == cJSON_Number);
}
static inline bool is_json_bool(json_t *json) {
	if(json == NULL) return false;
	return ((json->type == cJSON_False) || (json->type == cJSON_True));
}
static inline int json_array_size(json_t *json) {
	if(json==NULL) return 0;
	return cJSON_GetArraySize(json);
}
static inline json_t *json_getarray_item(json_t *json, int item) {
	if(json==NULL) return NULL;
	return cJSON_GetArrayItem(json, item);
}
static inline json_t *json_getobject_item(json_t *json, const char *item) {
	if(json==NULL) return NULL;
	return cJSON_GetObjectItem(json, item);
}
static inline int json_getint(json_t *s) {
	if ((s == NULL) || (!is_json_number(s))) return 0;
	return s->valueint;
}
static inline bool json_getbool(json_t *s) {
	if(!is_json_bool(s)) return false;
	if (s->type == cJSON_True)
		return true;
	return false;
}
static inline const char *json_getstring(json_t *s) {
	if ((s == NULL) || (!is_json_string(s))) return NULL;
	return s->valuestring;
}
static inline json_t *json_parse(const char *value, const char **return_parse_end, int require_null_terminated) {
	return cJSON_ParseWithOpts(value, return_parse_end, require_null_terminated);
}

typedef json_t* (*rpc_func)(json_t *params, void *dummy_parm);
typedef void (*copy_func)(void *dest, const char *src);

int json_rpc(void *outbuf, copy_func cpy, json_t *req, codetable_t *func_tbl, int tbl_size, void *dummy_parm);
void send_string(void *buf, copy_func cpy, int code, const char *message);

#endif
