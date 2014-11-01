#!/bin/bash

log_dir="."
stamp=`date +%Y_%m_%d_%H_%M_%S`

cur=`date`
echo "[$stamp] begin autotest..." >> $log_dir/run.log
/home/code/build64_release/horoscope/mpserver/autotests/mpserver_autotest \
    --mp_address='115.28.234.213/online_mpapi' \
> $log_dir/$stamp.log
ret=$?
cur=`date`

if [ $ret -eq 0 ]
then
    echo "[$stamp] autotest success" >> $log_dir/run.log
else
    echo "[$stamp] autotest failed. ret $ret" >> $log_dir/run.log
fi

