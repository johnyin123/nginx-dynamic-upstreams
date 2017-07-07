#!/bin/bash
./configure --with-debug --with-cc-opt="-O0" --prefix=/home/johnyin/nginx-1.13.0/bin/ \
--with-stream \
--with-stream_ssl_module \
--add-module=dyn_upstream

