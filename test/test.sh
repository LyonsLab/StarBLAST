#!/bin/sh

echo "=================================================="
echo Working directory: $(pwd)

#
# Download sample DB
mkdir db
wget ftp://ftp.ncbi.nlm.nih.gov/blast/db/vector.tar.gz
tar -zxvf vector.tar.gz -C db
rm vector.tar.gz


#
# Install BLAST
wget ftp://ftp.ncbi.nlm.nih.gov/blast/executables/blast+/LATEST/ncbi-blast-2.9.0+-x64-linux.tar.gz
tar -xvf ncbi-blast-2.9.0+-x64-linux.tar.gz
rm ncbi-blast-2.9.0+-x64-linux.tar.gz
export PATH=$PATH:ncbi-blast-2.9.0+/bin

#
# sequence

echo "=================================================="
echo -e "\n\n"



bash test1.sh
test $? == 0 || exit 1
echo -e "\n\n"

bash test2.sh
test $? == 0 || exit 1
echo -e "\n\n"
