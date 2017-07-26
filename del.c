#if (NGX_DYN_UPSTREAM_STREAM_ZONE)
static ngx_int_t sdel_peer(ngx_pool_t *pool, ngx_log_t *log, ngx_str_t *ups_name, ngx_str_t *name, ngx_str_t *server, ngx_uint_t down, unsigned backup)
{
	ngx_int_t count;
	ngx_uint_t i;
	ngx_stream_upstream_rr_peer_t *speer, *pspeer;
	ngx_stream_upstream_rr_peers_t *speers;
	ngx_stream_upstream_srv_conf_t *suscf, **suscfp;
	ngx_stream_upstream_main_conf_t *sumcf;

	sumcf = ngx_stream_cycle_get_module_main_conf(ngx_cycle, ngx_stream_upstream_module);
	if(sumcf == NULL) {
		return E_NO_UPS;
	}
	suscfp = sumcf->upstreams.elts;
	count = 0;
	for (i = 0; i < sumcf->upstreams.nelts; i++) {
		suscf = suscfp[i];
		speers = suscf->peer.data;
		if(speers == NULL) {
			continue;
		}
		if ((safe_ngx_strncmp(ups_name, speers->name) != 0) || (speers->shpool == NULL)) {
			continue;
		}
		if(backup) {
			speers = speers->next;
			if(speers == NULL) {
				continue; /*can return nofound*/
			}
		}
		for(speer = speers->peer, pspeer = speers->peer; speer != NULL; pspeer = speer, speer = speer->next) {
			if((name) && (name->data)) {
				if (safe_ngx_strncmp(name, &(speer->name)) != 0) {
					continue;
				}
			}
			if((server)&&(server->data)) {
				if (safe_ngx_strncmp(server, &(speer->server)) != 0) {
					continue;
				}
			}
			if(speer->down != down) {
				continue;
			}
			/*if upstream has only one server, can not delete */
			if(speers->number == 1) { /*speers->single*/
				return E_UPS_LEFT_ONE;
			}
			ngx_shmtx_lock(&speers->shpool->mutex);
			speers->number--;
			speers->single = (speers->number == 1);
			speers->total_weight -= speer->weight;
			if(speer == speers->peer) { /*first*/
				speers->peer = speer->next;
				pspeer = speers->peer;
			}
			else {
				pspeer->next = speer->next;
			}
			if(in_shpool(speers->shpool, speer->sockaddr)) {
				ngx_slab_free_locked(speers->shpool, speer->sockaddr);
			}
			if(in_shpool(speers->shpool, speer->name.data)) {
				ngx_slab_free_locked(speers->shpool, speer->name.data);
			}
			if(in_shpool(speers->shpool, speer->server.data)) {
				ngx_slab_free_locked(speers->shpool, speer->server.data);
			}
			ngx_slab_free_locked(speers->shpool, speer);

			ngx_shmtx_unlock(&speers->shpool->mutex);
			/* speer freed so next loop it start with pspeer*/
			speer = pspeer;
			count ++;
		}
	}
	return count;
}
#else
static ngx_int_t sdel_peer(ngx_pool_t *pool, ngx_log_t *log, ngx_str_t *ups_name, ngx_str_t *name, ngx_str_t *server, ngx_uint_t down, unsigned backup)
{
	return E_FUNC_NA;
}
#endif

#if (NGX_DYN_UPSTREAM_HTTP_ZONE)
static ngx_int_t hdel_peer(ngx_pool_t *pool, ngx_log_t *log, ngx_str_t *ups_name, ngx_str_t *name, ngx_str_t *server, ngx_uint_t down, unsigned backup)
{
	ngx_int_t count;
	ngx_uint_t i;
	ngx_http_upstream_rr_peer_t *hpeer, *phpeer;
	ngx_http_upstream_rr_peers_t *hpeers;
	ngx_http_upstream_srv_conf_t *huscf, **huscfp;
	ngx_http_upstream_main_conf_t *humcf;

	humcf = ngx_http_cycle_get_module_main_conf(ngx_cycle, ngx_http_upstream_module);
	if(humcf == NULL) {
		return E_NO_UPS;
	}
	huscfp = humcf->upstreams.elts;
	count = 0;
	for (i = 0; i < humcf->upstreams.nelts; i++) {
		huscf = huscfp[i];
		hpeers = huscf->peer.data;
		if(hpeers == NULL) {
			continue;
		}
		if ((safe_ngx_strncmp(ups_name, hpeers->name) != 0) || (hpeers->shpool == NULL)) {
			continue;
		}
		if(backup) {
			hpeers = hpeers->next;
			if(hpeers == NULL) {
				continue; /*can return nofound*/
			}
		}
		for(hpeer = hpeers->peer, phpeer = hpeers->peer; hpeer != NULL; phpeer = hpeer, hpeer = hpeer->next) {
			if((name) && (name->data)) {
				if (safe_ngx_strncmp(name, &(hpeer->name)) != 0) {
					continue;
				}
			}
			if((server)&&(server->data)) {
				if (safe_ngx_strncmp(server, &(hpeer->server)) != 0) {
					continue;
				}
			}
			if(hpeer->down != down) {
				continue;
			}
			/*if upstream has only one server, can not delete */
			if(hpeers->number == 1) { /*hpeers->single*/
				return E_UPS_LEFT_ONE;
			}
			ngx_shmtx_lock(&hpeers->shpool->mutex);
			hpeers->number--;
			hpeers->single = (hpeers->number == 1);
			hpeers->total_weight -= hpeer->weight;
			if(hpeer == hpeers->peer) { /*first*/
				hpeers->peer = hpeer->next;
				phpeer = hpeers->peer;
			}
			else {
				phpeer->next = hpeer->next;
			}
			if(in_shpool(hpeers->shpool, hpeer->sockaddr)) {
				ngx_slab_free_locked(hpeers->shpool, hpeer->sockaddr);
			}
			if(in_shpool(hpeers->shpool, hpeer->name.data)) {
				ngx_slab_free_locked(hpeers->shpool, hpeer->name.data);
			}
			if(in_shpool(hpeers->shpool, hpeer->server.data)) {
				ngx_slab_free_locked(hpeers->shpool, hpeer->server.data);
			}
			ngx_slab_free_locked(hpeers->shpool, hpeer);

			ngx_shmtx_unlock(&hpeers->shpool->mutex);
			/* hpeer freed so next loop it start with phpeer*/
			hpeer = phpeer;
			count ++;
		}
	}
	return count;
}
#else
static ngx_int_t hdel_peer(ngx_pool_t *pool, ngx_log_t *log, ngx_str_t *ups_name, ngx_str_t *name, ngx_str_t *server, ngx_uint_t down, unsigned backup)
{
	return E_FUNC_NA;
}
#endif
static json_t *del_upstream(json_t *parms, void *dummy_parm)
{
	ngx_http_request_t *r = dummy_parm;
	ngx_str_t ngx_upsname, ngx_srvname, ngx_server;
	const char *upsname, *srvname, *server;
	bool down, backup;
	bool ishttp;
	json_t *tmp_peers, *tmp_array;
	ngx_int_t ret_success_cnt, ret_failed_cnt, ret_cnt, i, j;
	ret_success_cnt = ret_failed_cnt = ret_cnt = 0;
	json_t *res_json = json_create();
	ngx_pool_t *pool = ngx_create_pool(NGX_DEFAULT_POOL_SIZE, r->connection->log);
	if (pool == NULL) {
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "dyn_upstream: pool alloc error");
		json_addnumber(res_json, JRET_CODE, E_NO_MEM);
		json_addstring(res_json, JRET_MSG, "pool alloc error");
		return res_json;
	}

	for(i=0;i<json_array_size(parms);i++) {
		if ((tmp_array = json_getobject_item(json_getarray_item(parms, i), JI_HTTP)) != NULL) {
			ishttp = true;
		}
		else if ((tmp_array = json_getobject_item(json_getarray_item(parms, i), JI_STREAM)) != NULL) {
			ishttp = false;
		}
		else {
			ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: failed %s/%s need", JI_HTTP, JI_STREAM);
			ret_failed_cnt ++;
			continue;
		}
		if ((upsname = json_getstring(json_getobject_item(tmp_array, JI_UPSNAME))) == NULL) {
			ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: failed %s [%s = null]", (ishttp ? JI_HTTP : JI_STREAM), JI_UPSNAME);
			ret_failed_cnt ++;
			continue;
		}
		if ((tmp_peers = json_getobject_item(tmp_array, JI_PEERS)) == NULL) {
			ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: failed %s [%s = %s, %s = null]", (ishttp ? JI_HTTP : JI_STREAM), JI_UPSNAME, upsname, JI_PEERS);
			ret_failed_cnt ++;
			continue;
		}
		for(j=0;j<json_array_size(tmp_peers);j++) {
			srvname = json_getstring(json_getobject_item(json_getarray_item(tmp_peers, j), JI_SRVNAME));
			server = json_getstring(json_getobject_item(json_getarray_item(tmp_peers, j), JI_SERVER));
			if((server == NULL) && (srvname == NULL)) {
				ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: failed %s [%s=%s, %s=null and %s=null]", (ishttp ? JI_HTTP : JI_STREAM), JI_UPSNAME, upsname, JI_SERVER, JI_SRVNAME);
				ret_failed_cnt ++;
				continue;
			}
			down      = json_getbool(json_getobject_item(json_getarray_item(tmp_peers, j), JI_DOWN));
			backup    = json_getbool(json_getobject_item(json_getarray_item(tmp_peers, j), JI_BACKUP));
			ngx_upsname.data = (u_char *)upsname;
			ngx_upsname.len = strlen(upsname);
			ngx_srvname.data = (u_char *)srvname;
			ngx_srvname.len = (srvname ? strlen(srvname) : 0);
			ngx_server.data = (u_char *)server;
			ngx_server.len = (server ? strlen(server) : 0);
			if(ishttp) {
				ret_cnt = hdel_peer(pool, r->connection->log, &ngx_upsname, &ngx_srvname, &ngx_server, down ? 1 : 0, backup ? 1 : 0);
			}
			else {
				ret_cnt = sdel_peer(pool, r->connection->log, &ngx_upsname, &ngx_srvname, &ngx_server, down ? 1 : 0, backup ? 1 : 0);
			}
			if (ret_cnt > 0) {
				ret_success_cnt += ret_cnt;
			}
			else {
				ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: failed %s [%s=%s, %s=%s, %s=%s], return=[%d:%s]", (ishttp ? JI_HTTP : JI_STREAM), JI_UPSNAME, upsname, JI_SRVNAME, srvname ? srvname : "null", JI_SERVER, server ? server : "null", ret_cnt, ups_errmsg(ret_cnt));
				ret_failed_cnt ++;
			}
		}
	}
	tmp_peers = json_create();
	json_addnumber(tmp_peers, JRET_SUCC, ret_success_cnt);
	json_addnumber(tmp_peers, JRET_FAIL, ret_failed_cnt);
	json_addnumber(res_json, JRET_CODE, E_SUCCESS);
	json_addobject(res_json, JRET_MSG, tmp_peers);
	ngx_destroy_pool(pool);
	return res_json;
}
