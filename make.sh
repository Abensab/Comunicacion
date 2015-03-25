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

printf "\nCompilating Server\n"
gcc server.c -o server

printf "\nCompilating Client\n"
gcc client.c -o client

printf "\nCompilating superWav5:\n"
cd ./bin
sudo make
