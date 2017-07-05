#!/bin/bash
DIRNAME=$(dirname `readlink -f $0`)
NGX_PATH="/home/johnyin/nginx-1.13.0/bin"
logrun() {
	url=$1
	parm=$2
	echo "start ------------------ " $parm
	cat $parm
	echo
	curl -4 -vvv -d "@$parm" $url
	#2>/dev/null | jq .
	echo
	echo "end  -------------------" $*
}

for fn in *.json
do
	logrun http://localhost:8800/ups/  $fn
done

curl -d "@lst-gen-all-1.json"  http://localhost:8800/ups/ 2>/dev/null | jq .

