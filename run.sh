#! /bin/bash
set -e
sudo ${INCLUDEOS_HOME-$HOME/IncludeOS_install}/etc/create_bridge.sh

make -j
source ${INCLUDEOS_HOME-$HOME/IncludeOS_install}/etc/run.sh `make kernelfile`
