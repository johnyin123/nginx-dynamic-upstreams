#include "common.h"
#include "json_rpc.h"

static ngx_str_t content_type = ngx_string("application/json");

#include "key.def"

static ngx_int_t dyups_http_output(ngx_http_request_t *r, ngx_buf_t *b, ngx_uint_t status, ngx_str_t *mime_type)
{
	ngx_int_t rc;
	ngx_chain_t out;
	rc = ngx_http_discard_request_body(r);
	if (rc != NGX_OK) {
		return rc;
	}
	r->headers_out.status = status;
	r->headers_out.content_type = *mime_type;
	if(b) {
		//b->last_buf = 1;
		b->last_buf = (r == r->main) ? 1 : 0;
		r->headers_out.content_length_n = ngx_buf_size(b);
	} else {
		r->headers_out.content_length_n = 0;
	}
	if (r->headers_out.content_length_n == 0) {
		r->header_only = 1;
	}
	rc = ngx_http_send_header(r);
	if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
		ngx_http_finalize_request(r, rc);
		return rc;
	}
	out.buf = b;
	out.next = NULL;
	ngx_http_finalize_request(r, ngx_http_output_filter(r, &out));
	return rc;
}
static ngx_buf_t *ngx_dyups_read_body(ngx_http_request_t *r)
{
	size_t len;
	ngx_buf_t *buf, *next, *body;
	ngx_chain_t *cl;
	/*Content Length 0*/
	if(r->request_body->bufs == NULL) {
		return NULL;
	}
	cl = r->request_body->bufs;
	buf = cl->buf;
	if (cl->next == NULL) {
		return buf;
	} else {
		next = cl->next->buf;
		len = ngx_buf_size(buf)/*(buf->last - buf->pos)*/ + ngx_buf_size(next)/*(next->last - next->pos)*/;
		body = ngx_create_temp_buf(r->pool, len);
		if (body == NULL) {
			return NULL;
		}
		body->last = ngx_cpymem(body->last, buf->pos, ngx_buf_size(buf)/*buf->last - buf->pos*/);
		body->last = ngx_cpymem(body->last, next->pos, ngx_buf_size(next)/*next->last - next->pos*/);
	}
	return body;
}

static ngx_buf_t *ngx_dyups_read_body_from_file(ngx_http_request_t *r)
{
	size_t len;
	ssize_t size;
	ngx_buf_t *buf, *body;
	ngx_chain_t*cl;
	len = 0;
	/*Content Length 0*/
	if(r->request_body->bufs == NULL) {
		return NULL;
	}
	cl = r->request_body->bufs;
	while (cl) {
		/*buf = cl->buf;
		if (buf->in_file) {
			len += buf->file_last - buf->file_pos;
		} else {
			len += buf->last - buf->pos;
		}*/
		len += ngx_buf_size(cl->buf);
		cl = cl->next;
	}
	body = ngx_create_temp_buf(r->pool, len);
	if (body == NULL) {
		return NULL;
	}
	cl = r->request_body->bufs;
	while (cl) {
		buf = cl->buf;
		if(!ngx_buf_in_memory(buf)){
		/*if (buf->in_file) {*/
			size = ngx_read_file(buf->file, body->last, ngx_buf_size(buf)/*buf->file_last - buf->file_pos*/, buf->file_pos);
			if (size == NGX_ERROR) {
				return NULL;
			}
			body->last += size;
		} else {
			body->last = ngx_cpymem(body->last, buf->pos, ngx_buf_in_memory(buf)/*buf->last - buf->pos*/);
		}
		cl = cl->next;
	}
	return body;
}
/*------------*/
#include "lst.c"
#include "add.c"
#include "del.c"
#include "mdf.c"
/*------------*/

codetable_t services[] = {
	{ JFUNC_LST, sizeof(JFUNC_LST), &lst_upstream },
	{ JFUNC_ADD, sizeof(JFUNC_ADD), &add_upstream },
	{ JFUNC_DEL, sizeof(JFUNC_DEL), &del_upstream },
	{ JFUNC_MDF, sizeof(JFUNC_MDF), &mdf_upstream },
};
#define services_num sizeof(services)/sizeof(codetable_t)

static void *json_malloc_fn(size_t sz)
{
	return ngx_alloc(sz, ngx_cycle->log);
}
static void json_free_fn(void *ptr)
{
	ngx_free(ptr);
}
cJSON_Hooks json_hook = {
	.malloc_fn = json_malloc_fn,
	.free_fn = json_free_fn
};

static void preinit_codetbl()
{
	cJSON_InitHooks(&json_hook);
	tbl_sort(services, services_num);
	//resp = ngx_pcalloc(r->pool, sizeof(uc_post_resp_t));
	//void *ngx_palloc(ngx_pool_t *pool, size_t size);
	//void *ngx_pcalloc(ngx_pool_t *pool, size_t size);
	//ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);
	//
	// define cJSON malloc/free
}
////////////////////////////////////////////////////////////////////////////////////////////////////
struct __j2b_parm_t
{
	ngx_buf_t *buf;
	ngx_pool_t *pool;
	ngx_log_t *log;
};
static void copy_json2buf(struct __j2b_parm_t *parm, const char *src)
{
	size_t size = strlen(src);
	parm->buf = ngx_create_temp_buf(parm->pool, size);
	if (parm->buf == NULL) {
		ngx_log_error(NGX_LOG_ERR, parm->log, 0, "dyn_upstream: memory error [size = $lu]", size);
		return;
	}
	parm->buf->last = ngx_cpymem(parm->buf->last, src, size);
}
#if (DEBUG)
static void parse_err(const char *errptr, ngx_http_request_t *r, ngx_buf_t **out, ngx_buf_t *body)
{
	ngx_int_t pos = (u_char *)errptr - body->start;
	u_char *ptr = pos > 10 ? body->start + pos - 10 : body->start;
	ngx_uint_t len = body->last - ptr > 20 ? 20 : (body->last - ptr);

	*out = ngx_create_temp_buf(r->pool, 64);
	if (*out == NULL) {
		return;
	}
	(*out)->last = ngx_snprintf((*out)->last, (*out)->end - (*out)->last,
			"parse error[pos = %d],[%*s]", pos, len, ptr);
}
#endif
static void ngx_dyups_body_handler(ngx_http_request_t *r)
{
	ngx_buf_t *body;
	json_t *req;
	const char *end_ptr = NULL;
	struct __j2b_parm_t parm;
	parm.buf = NULL;
	parm.pool = r->pool;
	parm.log = r->connection->log;

	if (r->request_body->temp_file) {
		body = ngx_dyups_read_body_from_file(r);
	} else {
		body = ngx_dyups_read_body(r);
	}
	if(body == NULL) {
		ngx_http_finalize_request(r, NGX_HTTP_NO_CONTENT);
		return;
	}
	req = json_parse((char *)body->start, &end_ptr, 0);
	if ((req == NULL) || (req->type != json_object) || ((u_char *)end_ptr > body->last)) {
		json_delete(req);
#if (DEBUG)
		parse_err(cJSON_GetErrorPtr(), r, &(parm.buf), body);
#else
		send_string(&parm, (copy_func)copy_json2buf, E_INVALID_REQUEST, ups_errmsg(E_INVALID_REQUEST));
#endif
		dyups_http_output(r, parm.buf, NGX_HTTP_BAD_REQUEST, &content_type);
		ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: parse error[pos = %d][end_ptr = %p]", (u_char *)cJSON_GetErrorPtr() - body->start, end_ptr);
		return;
	}
	json_rpc(&parm, (copy_func)copy_json2buf, req, services, services_num, r);
	json_delete(req);
	if (parm.buf == NULL) {
		ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
		return;
	}
	dyups_http_output(r, parm.buf, NGX_HTTP_OK, &content_type);
}

static ngx_int_t ngx_dyups_do_post(ngx_http_request_t *r)
{
	ngx_int_t rc;
	rc = ngx_http_read_client_request_body(r, ngx_dyups_body_handler);
	if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
		return rc;
	}
	return NGX_DONE;
}
static ngx_int_t ngx_http_dyn_upstream_handler(ngx_http_request_t *r)
{
	ngx_uint_t status;
	struct __j2b_parm_t parm;
	parm.buf = NULL;
	parm.pool = r->pool;
	parm.log = r->connection->log;

	switch (r->method) {
		case NGX_HTTP_POST:
			return ngx_dyups_do_post(r);
			break;
		case NGX_HTTP_GET:
			status = NGX_HTTP_OK;
			send_string(&parm, (copy_func)copy_json2buf, E_SUCCESS, ups_errmsg(E_SUCCESS));
			break;
		case NGX_HTTP_HEAD:
			status = NGX_HTTP_OK;
			break;
		default:
			status = NGX_HTTP_NOT_ALLOWED;
			send_string(&parm, (copy_func)copy_json2buf, E_NO_ALLOW, ups_errmsg(E_NO_ALLOW));
			break;
	}
	if (parm.buf == NULL) {
		ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
		return NGX_DONE;
	}
	return dyups_http_output(r, parm.buf, status, &content_type);
}
static char *ngx_http_dyn_upstream(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_core_loc_conf_t *clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	clcf->handler = ngx_http_dyn_upstream_handler;
	preinit_codetbl();
	return NGX_CONF_OK;
}

static ngx_command_t dyn_upstream_commands[] = {
	{ ngx_string("ups_mgr"),
	NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
	ngx_http_dyn_upstream,
	NGX_HTTP_LOC_CONF_OFFSET,
	0,
	NULL },

	ngx_null_command
};
static ngx_http_module_t dyn_upstream_module_ctx = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

ngx_module_t ngx_dyn_upstream_module = {
	NGX_MODULE_V1,
	&dyn_upstream_module_ctx,
	dyn_upstream_commands,
	NGX_HTTP_MODULE,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NGX_MODULE_V1_PADDING
};
