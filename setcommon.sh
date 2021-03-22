#!/bin/bash

#source scl_source enable devtoolset-7
source /opt/rh/devtoolset-7/enable
export COMMON_SET=1
export _condor_SCHEDD_HOST=sn-01.cr.cnaf.infn.it

export COMMONINSTALLDIR=/storage/gpfs_ams/ams/users/mduranti/MoriStuff/install

export STORAGE=/storage/gpfs_ams/ams/users/mduranti
export COMMONSW=${STORAGE}/MoriStuff
source ${COMMONSW}/install/setenvHERD_master.sh

export DTPSOFT=/storage/gpfs_ams/ams/users/mduranti/MoriStuff/install/DTP_build
export DTPINSTALL=/storage/gpfs_ams/ams/users/mduranti/MoriStuff/install/DTP_install
export LD_LIBRARY_PATH=${DTPINSTALL}/lib/:$LD_LIBRARY_PATH
