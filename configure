#!/bin/sh
sh autogen.sh || { echo "autogen.sh failed."; rm -f ./configure; cp ./generateandrun.sh ./configure; exit 1; }
./configure
