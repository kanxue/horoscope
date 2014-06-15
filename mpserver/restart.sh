#!/bin/bash

sbin_dir=/home/public/sbin
etc_dir=/home/public/etc
log_dir=/home/public/log
server=mpserver_main

killall -9 ${server}

nohup ${sbin_dir}/${server} --flagfile="${etc_dir}/${server}.flags" >> "${log_dir}/${server}_nohup.out" 2>&1 &
echo $! > pid

