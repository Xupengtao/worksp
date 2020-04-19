#!/bin/sh
echo "get /var/cache/rpm/*.rpm"
find /var/cache/yum/ -name *.rpm -exec cp '{}' /home/xpt/WorkSp/Download/rpm/ \;
ls /home/xpt/WorkSp/Download/rpm/
