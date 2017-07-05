#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

#if (NGX_DYN_UPSTREAM_STREAM_ZONE)
#include <ngx_stream.h> 
#endif

#define DEBUG 0

#if (DEBUG)
#define dd(fmt, ...) do { fprintf(stderr, "[**********]:[%s:%d(%s)] " fmt "\n", __FUNCTION__, __LINE__, __FILE__, ##__VA_ARGS__); }while(0)
#else
#define dd(fmt, ...) do {} while(0)
#endif

static inline ngx_int_t in_shpool(ngx_slab_pool_t *shpool, void *p)
{
	if ((u_char *) p < shpool->start || (u_char *) p > shpool->end) {
		return 0;
	}
	return 1;
}
static inline char *safe_ngx_strcpy(char *dest, size_t dest_len, ngx_str_t *src)
{
	size_t len = dest_len > src->len ? src->len : dest_len - 1;
	ngx_memcpy(dest, src->data, len);
	dest[len] = 0;
	return dest;
}
static inline ngx_int_t safe_ngx_strncmp(ngx_str_t *s1, ngx_str_t *s2)
{
	if(s1 && s2) {
		if(s1->len == s2->len) {
			return ngx_strncmp(s1->data, s2->data, s1->len);
		}
	}
	return -1;
}

static inline ngx_int_t ngx_http_dyn_ups_parse_args(ngx_http_request_t *r, ngx_keyval_t *parms)
{
	ngx_int_t val = 0;
	if (r->args.len == 0) {
		return 0;
	}
	for ( /* void */ ; parms->key.len; parms++) {
		if (ngx_http_arg(r, parms->key.data, parms->key.len, &parms->value) == NGX_OK) {
			val ++;
		}
	}
	return val;
}

