#!/bin/bash
function rand_line() {
	file=$1
	maxline=$(sed -n '$=' $file)
	let "num=(RANDOM % $maxline) + 1"
	sed -n "${num}p" $file
}

grep "#define" ../key.def | awk '{ print $2 "=" $3}' > key.def
source key.def
rm -f key.def

for seq in $(seq 1000)
do

T1=$(rand_line JI_TYPE)
T2=$(rand_line JI_TYPE)

cat<<EOF>add-gen-${seq}.json
{
	"${JREQ_METHOD}": "${JFUNC_ADD}",
	"${JREQ_PARAMS}": [
		{ "${T1}":{ "${JI_UPSNAME}" : "${T1}_$(rand_line JI_UPSNAME)", "${JI_PEERS}":[
			{ "${JI_SERVER}":"$(rand_line JI_SERVER)", "${JI_WEIGHT}":$(rand_line JI_WEIGHT), "${JI_MAX_CONNS}":$(rand_line JI_MAX_CONNS), "${JI_MAX_FAILS}":$(rand_line JI_MAX_FAILS), "${JI_FAIL_TMOUT}":$(rand_line JI_FAIL_TMOUT), "${JI_BACKUP}": $(rand_line JI_BACKUP), "${JI_DOWN}": $(rand_line JI_DOWN)},
			{ "${JI_SERVER}":"$(rand_line JI_SERVER)", "${JI_WEIGHT}":$(rand_line JI_WEIGHT), "${JI_MAX_CONNS}":$(rand_line JI_MAX_CONNS), "${JI_MAX_FAILS}":$(rand_line JI_MAX_FAILS), "${JI_FAIL_TMOUT}":$(rand_line JI_FAIL_TMOUT), "${JI_BACKUP}": $(rand_line JI_BACKUP), "${JI_DOWN}": $(rand_line JI_DOWN)}
		]}
		},
		{ "${T2}":{ "${JI_UPSNAME}" : "${T2}_$(rand_line JI_UPSNAME)", "${JI_PEERS}":[
			{ "${JI_SERVER}":"$(rand_line JI_SERVER)", "${JI_WEIGHT}":$(rand_line JI_WEIGHT), "${JI_MAX_CONNS}":$(rand_line JI_MAX_CONNS), "${JI_MAX_FAILS}":$(rand_line JI_MAX_FAILS), "${JI_FAIL_TMOUT}":$(rand_line JI_FAIL_TMOUT), "${JI_BACKUP}": $(rand_line JI_BACKUP), "${JI_DOWN}": $(rand_line JI_DOWN)},
			{ "${JI_SERVER}":"$(rand_line JI_SERVER)", "${JI_WEIGHT}":$(rand_line JI_WEIGHT), "${JI_MAX_CONNS}":$(rand_line JI_MAX_CONNS), "${JI_MAX_FAILS}":$(rand_line JI_MAX_FAILS), "${JI_FAIL_TMOUT}":$(rand_line JI_FAIL_TMOUT), "${JI_BACKUP}": $(rand_line JI_BACKUP), "${JI_DOWN}": $(rand_line JI_DOWN)}
		]}
		}
	]
}
EOF
done

for seq in $(seq 8000)
do

T1=$(rand_line JI_TYPE)
T2=$(rand_line JI_TYPE)

cat<<EOF>del-gen-${seq}.json
{
	"${JREQ_METHOD}": "${JFUNC_DEL}",
	"${JREQ_PARAMS}": [
		{ "${T1}":{ "${JI_UPSNAME}" : "${T1}_$(rand_line JI_UPSNAME)", "${JI_PEERS}":[
			{ "${JI_SRVNAME}":"$(rand_line JI_SRVNAME)", "${JI_SERVER}":"$(rand_line JI_SERVER)", "${JI_BACKUP}":$(rand_line JI_BACKUP), "${JI_DOWN}":$(rand_line JI_DOWN) },
			{}
		]}
		},
		{ "${T2}":{ "${JI_UPSNAME}" : "${T2}_$(rand_line JI_UPSNAME)", "${JI_PEERS}":[
			{ "${JI_SRVNAME}":"$(rand_line JI_SRVNAME)", "${JI_SERVER}":"$(rand_line JI_SERVER)", "${JI_BACKUP}":$(rand_line JI_BACKUP), "${JI_DOWN}":$(rand_line JI_DOWN) },
			{}
		]}
		}
	]
}
EOF

done

cat<<EOF>lst-gen-all-1.json
{
	"${JREQ_METHOD}": "${JFUNC_LST}"
}
EOF

