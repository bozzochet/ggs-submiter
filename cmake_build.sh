PWD=`pwd`
rm -fr ${STORAGE}/herd-vv-svn/build
mkdir ${STORAGE}/herd-vv-svn/build
cd ${STORAGE}/herd-vv-svn/build
cmake -DHerdSoftware_DIR=$HERDINSTALL/cmake -DEventAnalysis_DIR=/path/to/EventAnalysisInstallDir/cmake/ ../
cd ${PWD}

