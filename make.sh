#!/bin/bash

printf "\nChecking for missing packages...\n"
tmp=$(dpkg-query -W -f='${Package} ${Status} \n' libasound2-dev | grep "no package found")

if [[ $tmp =~ "no package found" ]]
then
printf "Working\n\n"
apt-get install $tmp
else
printf "Packages installed\n"
fi

printf "\nCompilating\n"
gcc SuperWavApp.c ./lib/connection.c -o SuperWavApp

printf "\nCompilating superWav5:\n"
cd ./lib
sudo make
