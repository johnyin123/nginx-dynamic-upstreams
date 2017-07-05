static ngx_int_t parse_url(ngx_pool_t *pool, ngx_log_t *log, ngx_url_t *url, ngx_str_t *server)
{
	ngx_memzero(url, sizeof(ngx_url_t));
	url->url = *server;
	url->default_port = 80;
	//url.uri_part = 1;
	//url.no_resolve = 0;
	if (ngx_parse_url(pool, url) != NGX_OK) {
		if (url->err) {
			ngx_log_error(NGX_LOG_WARN, log, 0, "dyn_upstream stream :%s in upstream \"%V\"", url->err, &url->url);
		}
		return E_PARSE_URL;
	}
	return E_SUCCESS;
}
#if (NGX_DYN_UPSTREAM_STREAM_ZONE)
static ngx_int_t sadd_peer(ngx_pool_t *pool, ngx_log_t *log, ngx_str_t *ups_name, ngx_str_t *server, ngx_int_t weight, ngx_uint_t max_conns, ngx_uint_t max_fails, time_t fail_timeout, ngx_uint_t down, unsigned backup)
{
	ngx_int_t count;
	ngx_uint_t i, naddr;
	ngx_stream_upstream_rr_peer_t *speer;
	ngx_stream_upstream_rr_peers_t *speers, *tmp_peers;
	ngx_stream_upstream_srv_conf_t *suscf, **suscfp;
	ngx_stream_upstream_main_conf_t *sumcf;
	ngx_url_t url;
	if(E_SUCCESS != parse_url(pool, log, &url, server)) {
		return E_PARSE_URL;
	}
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
			if(speers->next == NULL) {
				tmp_peers = ngx_slab_calloc(speers->shpool, sizeof(ngx_stream_upstream_rr_peers_t));
				if (tmp_peers == NULL) {
					ngx_shmtx_unlock(&speers->shpool->mutex);
					return E_SLAB_CALLOC;
				}
				tmp_peers->name = speers->name;
				tmp_peers->shpool = speers->shpool;
				tmp_peers->rwlock = speers->rwlock;
				ngx_shmtx_lock(&speers->shpool->mutex);
				speers->next = tmp_peers;
				/* all other elements zero */
				ngx_shmtx_unlock(&speers->shpool->mutex);
			}
			speers = speers->next;
		}
		for(naddr = 0;naddr<url.naddrs;naddr++) {
			speer = ngx_slab_calloc(speers->shpool, sizeof(ngx_stream_upstream_rr_peer_t));
			if (speer == NULL) {
				return E_SLAB_CALLOC;
			}
			speer->socklen = url.addrs->socklen;
			speer->sockaddr = ngx_slab_alloc(speers->shpool, speer->socklen);
			if (speer->sockaddr == NULL) {
				return E_SLAB_ALLOC;
			}
			ngx_memcpy(speer->sockaddr, url.addrs[naddr].sockaddr, speer->socklen);
			ngx_inet_set_port(speer->sockaddr, url.port);

			speer->name.len = url.addrs[naddr].name.len;
			speer->name.data = ngx_slab_alloc(speers->shpool, speer->name.len);
			if (speer->name.data == NULL) {
				return E_SLAB_ALLOC;
			}
			ngx_memcpy(speer->name.data, url.addrs[naddr].name.data, speer->name.len);

			speer->server.len = url.url.len;
			speer->server.data = ngx_slab_alloc(speers->shpool, speer->server.len);
			if (speer->server.data == NULL) {
				return E_SLAB_ALLOC;
			}
			ngx_memcpy(speer->server.data, url.url.data, speer->server.len);
			speer->weight = weight;
			speer->effective_weight = weight;
			speer->max_conns = max_conns;
			speer->max_fails = max_fails;
			speer->fail_timeout = fail_timeout;
			speer->down = down;

			ngx_shmtx_lock(&speers->shpool->mutex);
			speers->number++;
			speers->total_weight+=weight;
			speers->single = (speers->number == 1);
			speer->next = speers->peer;
			speers->peer = speer;
			ngx_shmtx_unlock(&speers->shpool->mutex);
			count ++;
		}
	}
	return count;
}
#else
static ngx_int_t sadd_peer(ngx_pool_t *pool, ngx_log_t *log, ngx_str_t *ups_name, ngx_str_t *server, ngx_int_t weight, ngx_uint_t max_conns, ngx_uint_t max_fails, time_t fail_timeout, ngx_uint_t down, unsigned backup)
{
	return E_FUNC_NA;
}
#endif

#if (NGX_DYN_UPSTREAM_HTTP_ZONE)
static ngx_int_t hadd_peer(ngx_pool_t *pool, ngx_log_t *log, ngx_str_t *ups_name, ngx_str_t *server, ngx_int_t weight, ngx_uint_t max_conns, ngx_uint_t max_fails, time_t fail_timeout, ngx_uint_t down, unsigned backup)
{
	ngx_int_t count;
	ngx_uint_t i, naddr;
	ngx_http_upstream_rr_peer_t *hpeer;
	ngx_http_upstream_rr_peers_t *hpeers, *tmp_peers;
	ngx_http_upstream_srv_conf_t *huscf, **huscfp;
	ngx_http_upstream_main_conf_t *humcf;
	ngx_url_t url;
	if(E_SUCCESS != parse_url(pool, log, &url, server)) {
		return E_PARSE_URL;
	}
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
			if(hpeers->next == NULL) {
				tmp_peers = ngx_slab_calloc(hpeers->shpool, sizeof(ngx_http_upstream_rr_peers_t));
				if (tmp_peers == NULL) {
					ngx_shmtx_unlock(&hpeers->shpool->mutex);
					return E_SLAB_CALLOC;
				}
				tmp_peers->name = hpeers->name;
				tmp_peers->shpool = hpeers->shpool;
				tmp_peers->rwlock = hpeers->rwlock;
				ngx_shmtx_lock(&hpeers->shpool->mutex);
				hpeers->next = tmp_peers;
				/* all other elements zero */
				ngx_shmtx_unlock(&hpeers->shpool->mutex);
			}
			hpeers = hpeers->next;
		}
		for(naddr = 0;naddr<url.naddrs;naddr++) {
			hpeer = ngx_slab_calloc(hpeers->shpool, sizeof(ngx_http_upstream_rr_peer_t));
			if (hpeer == NULL) {
				return E_SLAB_CALLOC;
			}
			hpeer->socklen = url.addrs->socklen;
			hpeer->sockaddr = ngx_slab_alloc(hpeers->shpool, hpeer->socklen);
			if (hpeer->sockaddr == NULL) {
				return E_SLAB_ALLOC;
			}
			ngx_memcpy(hpeer->sockaddr, url.addrs[naddr].sockaddr, hpeer->socklen);
			ngx_inet_set_port(hpeer->sockaddr, url.port);

			hpeer->name.len = url.addrs[naddr].name.len;
			hpeer->name.data = ngx_slab_alloc(hpeers->shpool, hpeer->name.len);
			if (hpeer->name.data == NULL) {
				return E_SLAB_ALLOC;
			}
			ngx_memcpy(hpeer->name.data, url.addrs[naddr].name.data, hpeer->name.len);

			hpeer->server.len = url.url.len;
			hpeer->server.data = ngx_slab_alloc(hpeers->shpool, hpeer->server.len);
			if (hpeer->server.data == NULL) {
				return E_SLAB_ALLOC;
			}
			ngx_memcpy(hpeer->server.data, url.url.data, hpeer->server.len);
			hpeer->weight = weight;
			hpeer->effective_weight = weight;
			hpeer->max_conns = max_conns;
			hpeer->max_fails = max_fails;
			hpeer->fail_timeout = fail_timeout;
			hpeer->down = down;

			ngx_shmtx_lock(&hpeers->shpool->mutex);
			hpeers->number++;
			hpeers->total_weight+=weight;
			hpeers->single = (hpeers->number == 1);
			hpeer->next = hpeers->peer;
			hpeers->peer = hpeer;
			ngx_shmtx_unlock(&hpeers->shpool->mutex);
			count ++;
		}
	}
	return count;
}
#else
static ngx_int_t hadd_peer(ngx_pool_t *pool, ngx_log_t *log, ngx_str_t *ups_name, ngx_str_t *server, ngx_int_t weight, ngx_uint_t max_conns, ngx_uint_t max_fails, time_t fail_timeout, ngx_uint_t down, unsigned backup)
{
	return E_FUNC_NA;
}
#endif

static json_t *add_upstream(json_t *parms, void *dummy_parm)
{
	ngx_http_request_t *r = dummy_parm;
	ngx_str_t ngx_upsname, ngx_server;
	const char *upsname, *server;
	ngx_int_t weight, max_conns, max_fails, fail_tm;
	bool down, backup;
	bool ishttp;
	json_t *tmp_peers, *tmp_array;
	ngx_int_t ret_success_cnt, ret_failed_cnt, ret_cnt;
	ret_success_cnt = ret_failed_cnt = ret_cnt = 0;
	json_t *res_json = json_create();
	ngx_pool_t *pool = ngx_create_pool(NGX_DEFAULT_POOL_SIZE, r->connection->log);
	if (pool == NULL) {
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "dyn_upstream: pool alloc error");
		json_addnumber(res_json, JRET_CODE, E_NO_MEM);
		json_addstring(res_json, JRET_MSG, "pool alloc error");
		return res_json;
	}

	for(ngx_int_t i=0;i<json_array_size(parms);i++) {
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
		for(ngx_int_t j=0;j<json_array_size(tmp_peers);j++) {
			//server = NULL; fail_tm = weight = max_conns = max_fails = 0; down = backup = false;
			if((server = json_getstring(json_getobject_item(json_getarray_item(tmp_peers, j), JI_SERVER))) == NULL) {
				ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: failed %s [%s = %s, %s = null]", (ishttp ? JI_HTTP : JI_STREAM), JI_UPSNAME, upsname, JI_SERVER);
				ret_failed_cnt ++;
				continue;
			}
			fail_tm   = json_getint(json_getobject_item(json_getarray_item(tmp_peers, j), JI_FAIL_TMOUT));
			weight    = json_getint(json_getobject_item(json_getarray_item(tmp_peers, j), JI_WEIGHT));
			max_conns = json_getint(json_getobject_item(json_getarray_item(tmp_peers, j), JI_MAX_CONNS));
			max_fails = json_getint(json_getobject_item(json_getarray_item(tmp_peers, j), JI_MAX_FAILS));
			down      = json_getbool(json_getobject_item(json_getarray_item(tmp_peers, j), JI_DOWN));
			backup    = json_getbool(json_getobject_item(json_getarray_item(tmp_peers, j), JI_BACKUP));
			/*default value from ngx_http_upstream.c*/
			if (weight <= 0) {
				weight = 1;
			}
			if (max_conns <= 0) {
				max_conns = 0;
			}
			if (max_fails <= 0) {
				max_fails = 1;
			}
			if (fail_tm <= 0) {
				fail_tm = 10;
			}
			ngx_upsname.data = (u_char *)upsname;
			ngx_upsname.len = strlen(upsname);
			ngx_server.data = (u_char *)server;
			ngx_server.len = strlen(server);
			if(ishttp) {
				ret_cnt = hadd_peer(pool, r->connection->log, &ngx_upsname, &ngx_server, weight, max_conns,  max_fails, (time_t)fail_tm, down ? 1 : 0, backup ? 1 : 0);
			}
			else {
				ret_cnt = sadd_peer(pool, r->connection->log, &ngx_upsname, &ngx_server, weight, max_conns,  max_fails, (time_t)fail_tm, down ? 1 : 0, backup ? 1 : 0);
			}
			if (ret_cnt > 0) {
				ret_success_cnt += ret_cnt;
			}
			else {
				ngx_log_error(NGX_LOG_WARN, r->connection->log, 0, "dyn_upstream: failed %s [%s=%s, %s=%s], return=[%d:%s]", (ishttp ? JI_HTTP : JI_STREAM), JI_UPSNAME, upsname, JI_SERVER, server, ret_cnt, ups_errmsg(ret_cnt));
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
