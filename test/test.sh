#!/bin/sh

echo "=================================================="
echo Working directory: $(pwd)

#
# Sample DB
mkdir db
tar -zxvf vector.tar.gz -C db


echo "=================================================="
echo -e "\n\n"



bash test1.sh
rc=$?
echo "exit status:" $rc
test $rc == 0 || exit 1
echo -e "\n\n"

bash test2.sh
rc=$?
echo "exit status:" $rc
test $rc == 0 || exit 1
echo -e "\n\n"

rm -rf db