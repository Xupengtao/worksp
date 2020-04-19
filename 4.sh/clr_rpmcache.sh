#!/bin/sh
echo "clear /var/cache/rpm/..."
yum clean all
rm -rf /var/cache/yum/
