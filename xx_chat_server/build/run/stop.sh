#!/bin/sh

kill -9 $(ps -ef|grep -E 'msg_server|db_proxy_server|route_server|login_server'|grep -v grep|awk '{print $2}')
