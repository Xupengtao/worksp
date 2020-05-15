#!/bin/sh
echo "get /var/cache/rpm/*.rpm"
find /var/cache/yum/ -name *.rpm -exec cp '{}' /home/xpt/imzcyrpm-lib/ \;
ls /home/xpt/imzcyrpm-lib/
