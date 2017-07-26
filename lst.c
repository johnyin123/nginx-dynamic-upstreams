typedef struct {
	ngx_str_t name;
	ngx_str_t server;
	ngx_int_t current_weight;
	ngx_int_t effective_weight;
	ngx_int_t weight;
	ngx_uint_t conns;
	ngx_uint_t max_conns;
	ngx_uint_t fails;
	/*time_t accessed;*/
	/*time_t checked;*/
	ngx_uint_t max_fails;
	time_t fail_timeout;
	ngx_msec_t slow_start;
	ngx_msec_t start_time;
	ngx_uint_t down;
	unsigned backup; /*is backup peers*/
} dyn_peer_info_t;

enum {TYPE_HTTP = 0, TYPE_STREAM};

typedef struct {
	unsigned type; /*http = 0,stream = 1*/
	ngx_str_t *name;
	ngx_uint_t number;
	ngx_uint_t total_weight;
	ngx_array_t *peer; /* dyn_peer_info_t */
} dyn_peers_info_t;

#if (NGX_DYN_UPSTREAM_STREAM_ZONE)
static ngx_int_t sdmp_upstreams(ngx_pool_t *pool, ngx_log_t *log, ngx_array_t *list, ngx_str_t *ups_name)
{
	dyn_peer_info_t *peer;
	dyn_peers_info_t *dpinfo;
	ngx_uint_t i, pos, count = 0;
	ngx_stream_upstream_srv_conf_t *suscf, **suscfp;
	ngx_stream_upstream_rr_peers_t *speers;
	ngx_stream_upstream_rr_peer_t *speer;
	ngx_stream_upstream_main_conf_t *sumcf;
	unsigned backup;
	sumcf = ngx_stream_cycle_get_module_main_conf(ngx_cycle, ngx_stream_upstream_module);
	if(sumcf == NULL) {
		return E_NO_UPS;
	}
	suscfp = sumcf->upstreams.elts;
	for (i = 0; i < sumcf->upstreams.nelts; i++) {
		suscf = suscfp[i];
		speers = suscf->peer.data;
		if (speers->shpool == NULL) {
			continue;
		}
		if (ups_name) {
			if (safe_ngx_strncmp(ups_name, speers->name) != 0) {
				continue;
			}
		}
		dpinfo = ngx_array_push(list);
		if (dpinfo == NULL) {
			return E_ARRAY_PUSH_NULL;
		}
		dpinfo->type = TYPE_STREAM;
		dpinfo->name = speers->name;
		dpinfo->number = speers->next ? speers->next->number + speers->number : speers->number;
		dpinfo->total_weight = speers->next ? speers->next->total_weight + speers->total_weight : speers->total_weight;
		dpinfo->peer = ngx_array_create(list->pool, dpinfo->number, sizeof(dyn_peer_info_t));
		if (dpinfo->peer == NULL) {
			return E_ARRAY_CREATE_NULL;
		}
		peer = ngx_array_push_n(dpinfo->peer, dpinfo->number);
		if (peer == NULL) {
			return E_ARRAY_PUSHN_NULL;
		}
		backup = 0;
		pos = 0;
		while(speers)
		{
			for (speer = speers->peer; pos < dpinfo->number && speer != NULL; speer = speer->next) {
				peer[pos].name = speer->name;
				peer[pos].server = speer->server;
				peer[pos].current_weight = speer->current_weight;
				peer[pos].effective_weight = speer->effective_weight;
				peer[pos].weight = speer->weight;
				peer[pos].conns = speer->conns;
				peer[pos].max_conns = speer->max_conns;
				peer[pos].fails = speer->fails;
				peer[pos].max_fails = speer->max_fails;
				peer[pos].fail_timeout = speer->fail_timeout;
				peer[pos].slow_start = speer->slow_start;
				peer[pos].start_time = speer->start_time;
				peer[pos].down = speer->down;
				peer[pos].backup = backup;
				count++;
				pos++;
			}
			speers = speers->next;
			backup = 1;
		}
	}
	return count;
}
#else
static ngx_int_t sdmp_upstreams(ngx_pool_t *pool, ngx_log_t *log, ngx_array_t *list, ngx_str_t *ups_name)
{
	return E_FUNC_NA;
}
#endif

#if (NGX_DYN_UPSTREAM_HTTP_ZONE)
static ngx_int_t hdmp_upstreams(ngx_pool_t *pool, ngx_log_t *log, ngx_array_t *list, ngx_str_t *ups_name)
{
	dyn_peer_info_t *peer;
	dyn_peers_info_t *dpinfo;
	ngx_uint_t i, pos, count = 0;
	ngx_http_upstream_srv_conf_t *huscf, **huscfp;
	ngx_http_upstream_rr_peers_t *hpeers;
	ngx_http_upstream_rr_peer_t *hpeer;
	ngx_http_upstream_main_conf_t *humcf;
	unsigned backup;
	humcf = ngx_http_cycle_get_module_main_conf(ngx_cycle, ngx_http_upstream_module);
	if(humcf == NULL) {
		return E_NO_UPS;
	}
	huscfp = humcf->upstreams.elts;
	for (i = 0; i < humcf->upstreams.nelts; i++) {
		huscf = huscfp[i];
		hpeers = huscf->peer.data;
		if (hpeers->shpool == NULL) {
			continue;
		}
		if (ups_name) {
			if (safe_ngx_strncmp(ups_name, hpeers->name) != 0) {
				continue;
			}
		}
		dpinfo = ngx_array_push(list);
		if (dpinfo == NULL) {
			return E_ARRAY_PUSH_NULL;
		}
		dpinfo->type = TYPE_HTTP;
		dpinfo->name = hpeers->name;
		dpinfo->number = hpeers->next ? hpeers->next->number + hpeers->number : hpeers->number;
		dpinfo->total_weight = hpeers->next ? hpeers->next->total_weight + hpeers->total_weight : hpeers->total_weight;
		dpinfo->peer = ngx_array_create(list->pool, dpinfo->number, sizeof(dyn_peer_info_t));
		if (dpinfo->peer == NULL) {
			return E_ARRAY_CREATE_NULL;
		}
		peer = ngx_array_push_n(dpinfo->peer, dpinfo->number);
		if (peer == NULL) {
			return E_ARRAY_PUSHN_NULL;
		}
		backup = 0;
		pos = 0;
		while(hpeers)
		{
			for (hpeer = hpeers->peer; pos < dpinfo->number && hpeer != NULL; hpeer = hpeer->next) {
				peer[pos].name = hpeer->name;
				peer[pos].server = hpeer->server;
				peer[pos].current_weight = hpeer->current_weight;
				peer[pos].effective_weight = hpeer->effective_weight;
				peer[pos].weight = hpeer->weight;
				peer[pos].conns = hpeer->conns;
				peer[pos].max_conns = hpeer->max_conns;
				peer[pos].fails = hpeer->fails;
				peer[pos].max_fails = hpeer->max_fails;
				peer[pos].fail_timeout = hpeer->fail_timeout;
				peer[pos].slow_start = hpeer->slow_start;
				peer[pos].start_time = hpeer->start_time;
				peer[pos].down = hpeer->down;
				peer[pos].backup = backup;
				count++;
				pos++;
			}
			hpeers = hpeers->next;
			backup = 1;
		}
	}
	return count;

}
#else
static ngx_int_t hdmp_upstreams(ngx_pool_t *pool, ngx_log_t *log, ngx_array_t *list, ngx_str_t *ups_name)
{
	return E_FUNC_NA;
}
#endif


static json_t *lst_upstream(json_t *parms, void *dummy_parm)
{
	const char *str;
	char cstr_tmp[1024];
	dyn_peers_info_t *dpinfo;
	dyn_peer_info_t *peer;
	ngx_int_t ret_cnt, i, j;
	ngx_uint_t pos;
	ngx_array_t *ups;
	ngx_str_t upsname;
	json_t *array, *item, *json_peer, *json_peers;
	ngx_http_request_t *r = dummy_parm;
	json_t *res_json = json_create();
	ngx_pool_t *pool = ngx_create_pool(NGX_DEFAULT_POOL_SIZE, r->connection->log);
	if (pool == NULL) {
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "dyn_upstream: pool alloc error");
		json_addnumber(res_json, JRET_CODE, E_NO_MEM);
		json_addstring(res_json, JRET_MSG, "pool alloc error");
		return res_json;
	}
	ups = ngx_array_create(pool, 8, sizeof(dyn_peers_info_t));
	if (ups == NULL) {
		ngx_destroy_pool(pool);
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "dyn_upstream: array alloc error");
		json_addnumber(res_json, JRET_CODE, E_NO_MEM);
		json_addstring(res_json, JRET_MSG, "array alloc error");
		return res_json;
	}
	if(json_array_size(parms) == 0) {
		/*list all upstream(zone)*/
		ret_cnt = hdmp_upstreams(pool, r->connection->log, ups, NULL);
		if(ret_cnt<0) {
			ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: return=[%d:%s]", ret_cnt, ups_errmsg(ret_cnt));
		}
		ret_cnt = sdmp_upstreams(pool, r->connection->log, ups, NULL);
		if(ret_cnt<0) {
			ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: return=[%d:%s]", ret_cnt, ups_errmsg(ret_cnt));
		}
	}
	for(i=0;i<json_array_size(parms);i++) {
		array = json_getobject_item(json_getarray_item(parms, i), JI_HTTP);
		for(j=0;j<json_array_size(array);j++) {
			str = json_getstring(json_getobject_item(json_getarray_item(array, j), JI_UPSNAME));
			if (str) {
				upsname.data = (u_char *)str;
				upsname.len = strlen(str);
				ret_cnt = hdmp_upstreams(pool, r->connection->log, ups, &upsname);
				if(ret_cnt<0) {
					ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: failed %s [%s=%s], return=[%d:%s]", JI_HTTP, JI_UPSNAME, str, ret_cnt, ups_errmsg(ret_cnt));
				}
			}
		}
		array = json_getobject_item(json_getarray_item(parms, i), JI_STREAM);
		for(j=0;j<json_array_size(array);j++) {
			str = json_getstring(json_getobject_item(json_getarray_item(array, j), JI_UPSNAME));
			if(str) {
				upsname.data = (u_char *)str;
				upsname.len = strlen(str);
				ret_cnt = sdmp_upstreams(pool, r->connection->log, ups, &upsname);
				if(ret_cnt<0) {
					ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: failed %s [%s=%s], return=[%d:%s]", JI_STREAM, JI_UPSNAME, str, ret_cnt, ups_errmsg(ret_cnt));
				}
			}
		}
	}
	dpinfo = ups->elts;
	array = json_createarray();
	for (ngx_int_t i = 0; i < (int)ups->nelts; i++) {
		json_peers = json_createarray();
		peer = dpinfo[i].peer->elts;
		for (pos=0;pos<dpinfo[i].number;pos++) {
			json_peer = json_create();
			json_addstring(json_peer, JI_SRVNAME, safe_ngx_strcpy(cstr_tmp, sizeof(cstr_tmp), &(peer[pos].name)));
			json_addstring(json_peer, JI_SERVER,  safe_ngx_strcpy(cstr_tmp, sizeof(cstr_tmp), &(peer[pos].server)));
			json_addnumber(json_peer, JU_CUR_WEIGHT, peer[pos].current_weight);
			json_addnumber(json_peer, JU_EFF_WEIGHT, peer[pos].effective_weight);
			json_addnumber(json_peer, JI_WEIGHT, peer[pos].weight);
			json_addnumber(json_peer, JU_CONNS, peer[pos].conns);
			json_addnumber(json_peer, JI_MAX_CONNS, peer[pos].max_conns);
			json_addnumber(json_peer, JU_FAILS, peer[pos].fails);
			json_addnumber(json_peer, JI_MAX_FAILS, peer[pos].max_fails);
			json_addnumber(json_peer, JI_FAIL_TMOUT, peer[pos].fail_timeout);
			json_addnumber(json_peer, JU_SLOW_START, peer[pos].slow_start);
			json_addnumber(json_peer, JU_START_TM, peer[pos].start_time);
			json_addbool(json_peer, JI_DOWN, peer[pos].down);
			json_addbool(json_peer, JI_BACKUP, peer[pos].backup ? true : false);
			json_addarray(json_peers, json_peer);
		}
		item = json_create();
		json_addstring(item, JI_UPSNAME, safe_ngx_strcpy(cstr_tmp, sizeof(cstr_tmp), dpinfo[i].name));
		json_addnumber(item, JU_NUMBER, dpinfo[i].number);
		json_addnumber(item, JU_TOTAL_WEIGHT, dpinfo[i].total_weight);
		json_addstring(item, JU_TYPE, dpinfo[i].type == TYPE_STREAM ? JI_STREAM : JI_HTTP);
		json_addobject(item, JI_PEERS, json_peers);
		json_addarray(array, item);
	}
	json_addnumber(res_json, JRET_CODE, E_SUCCESS);
	json_addobject(res_json, JRET_MSG, array);
	ngx_destroy_pool(pool);
	return res_json;
}
