#!/bin/bash

/home/code/build64_release/horoscope/storage/tools/init_redis_horoscope_attr \
    --storage_redis_ip=114.113.155.202 \
    --storage_redis_port=6379 \
    --storage_redis_index=0 \
    --storage_mysql_name=eyrie_farm \
    --storage_mysql_host=114.113.155.201 \
    --storage_mysql_port=3306 \
    --storage_mysql_user=root \
    --storage_mysql_pass=lannisport \

