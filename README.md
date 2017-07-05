# nginx-dynamic-upstreams
Nginx dynamic upstream(http &amp; stream) use http json

add(server,backup,down,weight,max_conns,max_fails,fail_timeout)
	server : must set
	backup noset : false
	down noset : false 
	weight>0 else : 1
	max_conns>0 else : 0
	max_fails>0 else : 1
	fail_timeout>0 else : 10
---------------------------------------
del(name,server,backup,down)
	name/server must set at least one
	backup noset : false
	down noset : false
edit(name,server,backup)
	name/server must set at least one
	backup noset : false
	others can modify, noset no modify


Example:
List:
{
	"method": "lst"
}
Add:
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
Del:
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
Modify:
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
