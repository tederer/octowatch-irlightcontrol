#!/bin/bash

scriptDir=$(cd $(dirname $0) && pwd)
cd $scriptDir

if [ "$(whoami)" != "root" ]; then
   echo "you need to be root to execute to IR light control"
   exit 1
fi

./build/irlightcontrol
