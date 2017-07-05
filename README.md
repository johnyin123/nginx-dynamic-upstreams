# nginx-dynamic-upstreams<br>

# Description
Nginx dynamic upstream(http &amp; stream) use http json<br>

# add(server,backup,down,weight,max_conns,max_fails,fail_timeout)
	server : must set
	backup noset : false
	down noset : false
	weight>0 else : 1
	max_conns>0 else : 0
	max_fails>0 else : 1
	fail_timeout>0 else : 10
# del(name,server,backup,down)
	name/server must set at least one
	backup noset : false
	down noset : false
# edit(name,server,backup)
	name/server must set at least one
	backup noset : false
	others can modify, noset no modify


# Example:

## List:
	{
		"method": "lst"
	}
## Add:
	{
		"method": "add",
		"params": [
			{ "http":{ "upsname" : "upsname", "peers":[
				{ "server":"10.0.2.11:9900", "weight":10, "max_conns":0, "max_fails":3, "fail_timeout":60, "backup": true , "down": false },
				{}
			]}
			},
			{ "stream":{ "upsname" : "stream_ups", "peers":[
				{ "server":"10.0.2.11:9900", "weight":-10, "max_conns":0, "max_fails":3, "fail_timeout":60, "backup": true , "down": false },
				{}
			]}
			}
		]
	}
## Del:
	{
		"method": "del",
		"params": [
			{ "http":{ "upsname" : "upsname", "peers":[
				{ "name":"[::1]:10001", "server":"localhost:10001", "backup": false, "down": false },
				{}
			]}
			},
			{ "stream":{ "upsname" : "stream_ups", "peers": [
				{ "name":"[::1]:10001", "server":"localhost:10001", "backup": false, "down": false },
				{}
			]}
			}
		]
	}

## Modify:
	{
		"method": "edit",
		"params": [
			{ "http":{ "upsname" : "upsname", "peers":[
				{ "name":"127.0.0.1:10001", "server":"localhost:10001", "backup": true, "weight":10, "max_conns":0, "max_fails":3, "fail_timeout":60, "down": false },
				{}
			]}
			},
			{ "stream":{ "upsname" : "stream_ups", "peers":[
				{ "name":"127.0.0.1:10001", "server":"localhost:10001", "backup": true, "weight":-10, "max_conns":0, "max_fails":3, "fail_timeout":60, "down": false },
				{}
			]}
			}
		]
	}
## return Result(list)
	{
		"code": 0,
		"message": [
			{
				"upsname": "http_upsname",
				"number": 2,
				"total_weight": 2,
				"type": "http",
				"peers": [
					{
						"name": "127.0.0.1:10001",
						"server": "localhost:10001",
						"current_weight": 0,
						"effective_weight": 1,
						"weight": 1,
						"conns": 0,
						"max_conns": 0,
						"fails": 0,
						"max_fails": 1,
						"fail_timeout": 10,
						"slow_start": 0,
						"start_time": 0,
						"down": false,
						"backup": false
					},
					{
						"name": "[::1]:10001",
						"server": "localhost:10001",
						"current_weight": 0,
						"effective_weight": 1,
						"weight": 1,
						"conns": 0,
						"max_conns": 0,
						"fails": 0,
						"max_fails": 1,
						"fail_timeout": 10,
						"slow_start": 0,
						"start_time": 0,
						"down": false,
						"backup": false
					}
				]
			},
			{
				"upsname": "stream_upsname",
				"number": 1,
				"total_weight": 1,
				"type": "stream",
				"peers": [
					{
						"name": "127.0.0.1:10002",
						"server": "127.0.0.1:10002",
						"current_weight": 0,
						"effective_weight": 1,
						"weight": 1,
						"conns": 0,
						"max_conns": 0,
						"fails": 0,
						"max_fails": 1,
						"fail_timeout": 10,
						"slow_start": 0,
						"start_time": 0,
						"down": false,
						"backup": false
					}
				]
			}
		]
	}
## return Result(add/modify/del)
### success(code == 0)
	{
		"code": 0,
		"message": {
			"success": 0,
			"failed": 4
		}
	}
### failed(code != 0)
	{
		"code": -1009,
		"message": "No upstream found"
	}

[我的博客](http://blog.chinaunix.net/uid/16979052.html)  



