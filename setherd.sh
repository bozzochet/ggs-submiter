export STORAGE=/storage/gpfs_ams/ams/users/vvagelli
export COMMONSW=${STORAGE}/commonsw
source ${COMMONSW}/install/setcommon.sh


export HERDDIR=/storage/gpfs_ams/ams/users/vvagelli/HERD/
export HERDSOFT=${HERDDIR}/HerdSoftware
export HERDINSTALL=${HERDDIR}/HerdSoftware.install
export EA_PLUGIN_PATH=${HERDINSTALL}/plugin/:$EA_PLUGIN_PATH
export LD_LIBRARY_PATH=${HERDINSTALL}/lib/:$LD_LIBRARY_PATH
export CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}:${HERDINSTALL}/cmake
