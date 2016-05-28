#!/bin/bash

printf "\nChecking for missing packages...\n"
tmp=$(dpkg-query -W -f='${Package} ${Status} \n' libasound2-dev | grep "no package found")

if [[ $tmp =~ "no package found" ]]
then
printf "Working\n\n"
sudo apt-get install libasound2-dev -y
else
printf "Packages installed\n"
fi


printf "\nChecking for libconfig.h packages...\n"
FILE="/usr/local/include/libconfig.h"
if [ -f $FILE ];
then
   echo "File $FILE exists"
   echo "Libconfig package installed"

else
   echo "File $FILE does not exists"
   printf "Working\n\n"
   tar -xvf ./../../bin/configlib/libconfig-1.5.tar.gz -C /tmp
   cd /tmp/libconfig-1.5/
   $(sudo ./configure && sudo make && sudo make check && sudo make install && sudo ldconfig -v)
fi


printf "Location:\n"
route=$(pwd)
echo $route


printf "\nCompilating SuperWavClient...\n"
#sudo make all
sudo make debug
