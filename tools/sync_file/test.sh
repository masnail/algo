#!/bin/bash

sum_cnt=0
success_cnt=0

TestClient(){
    is_exist_server=`netstat  -tu|grep 9190|wc -l`
    if [ ${is_exist_server} -le 0 ]; then
        ./server/server &
    fi

    file_name="$1"
    ./client/client ${file_name}
    old_size=`ls -la "${file_name}"|awk '{print $5}'`
    new_size=`ls -la "${file_name}.copy"|awk '{print $5}'`

    let sum_cnt=sum_cnt+1
    if [ ${old_size} == ${new_size} ]; then
        let success_cnt=success_cnt+1
    fi
}

while true
do
    TestClient zh-hans_windows_xp_professional_with_service_pack_3_x86_cd_x14-80404.iso

    success_ratio=`echo "scale=2;${success_cnt}/${sum_cnt}"|bc`
    echo "---> run count:${sum_cnt}, success_ratio:${success_ratio}"
done