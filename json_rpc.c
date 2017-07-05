#include "json_rpc.h"
#include "key.def"

//#define JSON_RPC_DEBUG 1
struct msg_list
{
    int proto;
    const char *name;
} msg_list[] =
{
	{ E_INVALID_REQUEST, "Invalid request" },
	{ E_INVALID_METHOD, "Invalid method" },
	{ E_UPS_LEFT_ONE, "Upstream left one peer" },
	{ E_ARRAY_PUSH_NULL, "Push array return null" },
	{ E_ARRAY_PUSHN_NULL, "Pushn array return null" },
	{ E_ARRAY_CREATE_NULL, "Array Create return null" },
	{ E_PARSE_URL, "Parse url error" },
	{ E_SLAB_ALLOC, "Slab alloc error" },
	{ E_SLAB_CALLOC, "Slab calloc error" },
	{ E_FUNC_NA, "function not compile in" },
	{ E_NO_ALLOW, "Not allow" },
	{ E_NO_UPS, "No upstream found" },
	{ 0, "success" },
};
const char *ups_errmsg(int proto)
{
    struct msg_list *lp;
	for (lp = msg_list; lp->proto != 0; ++lp) {
		if (proto == lp->proto) {
			return lp->name;
		}
	}
    return lp->name; /*the last one*/
}

void send_string(void *buf, copy_func cpy, int code, const char *message)
{
	char *str_json;
	json_t *err_json = json_create();
	json_addnumber(err_json, JRET_CODE, code);
	json_addstring(err_json, JRET_MSG, message);
	str_json = json_tostring(err_json);
#ifdef JSON_RPC_DEBUG
	fprintf(stderr, "response:\n%s\n", err_json);
#endif
	cpy(buf, str_json);
	if(str_json) {
		free(str_json);
	}
	json_delete(err_json);
}

static void send_json(void *buf, copy_func cpy, json_t *result)
{
	char *str_json = json_tostring(result);
#ifdef JSON_RPC_DEBUG
	fprintf(stderr, "response:\n%s\n", str_json);
#endif
	cpy(buf, str_json);
	if(str_json) {
		free(str_json);
	}
	json_delete(result);
}
int json_rpc(void *buf, copy_func cpy, json_t *req, codetable_t *func_tbl, int tbl_size, void *dummy_parm)
{
	rpc_func func = NULL;
	json_t *s_func, *parms;

	s_func = json_getobject_item(req, JREQ_METHOD);
	if (s_func != NULL && is_json_string(s_func))
	{
		parms = json_getobject_item(req, JREQ_PARAMS);
		if (parms == NULL || parms->type == json_array)
		{
#ifdef JSON_RPC_DEBUG
			char *str_json = json_tostring(req);
			fprintf(stderr, "request :\n%s\n", str_json);
			free(str_json);
#endif
			if(tbl_bsearch(func_tbl, tbl_size, s_func->valuestring, (void *)&func))
			{
				send_json(buf, cpy, func(parms, dummy_parm));
				return E_SUCCESS;
			}
			else
			{
				send_string(buf, cpy, E_INVALID_METHOD, ups_errmsg(E_INVALID_METHOD));
				return E_INVALID_METHOD;
			}
		}
	}
	send_string(buf, cpy, E_INVALID_REQUEST, ups_errmsg(E_INVALID_REQUEST));
	return E_INVALID_REQUEST;
}
/*
{
	"method":	"lst",
	"params":[
		{ "http":false },
		{ "http":true },
		{ "http":false },
		{ "stream":false  }
	]
}
*/
/*
#include "json_rpc.h"
#include <stdio.h>

static json_t *lst_upstream(json_t *parms, void *dummy_parm)
{
	json_t *res_json = json_create();
	json_addnumber(res_json, "code", 1000);
	json_addstring(res_json, "message", "my message");
	return res_json;
}
codetable_t services[] = {
	{ "lst", 3, &lst_upstream },
};
#define services_num sizeof(services)/sizeof(codetable_t)

static void preinit_codetbl()
{
	tbl_sort(services, services_num);
}
int main(int argc, char *argv[])
{
	json_t *req;
	char res_buf[4096];
	char req_buf[4096];
	const char *end_ptr;
	preinit_codetbl();
	fread(req_buf, sizeof(req_buf), 1, stdin);
	if ((req = json_parse(req_buf, &end_ptr, 0)) == NULL) {
		fprintf(stderr, "error json parse\n");
		return -1;
	}
	if (req->type == json_object) {
		json_rpc(res_buf, (copy_func)strcpy, req, services, services_num, NULL);
	}
	json_delete(req);
	fprintf(stdout, "\n-------------------------\n%s\n", res_buf);
	return 0;
}
*/
