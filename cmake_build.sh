PWD=`pwd`
rm -fr build
mkdir build
cd build
cmake -DHerdSoftware_DIR=$HERDINSTALL/cmake -DEventAnalysis_DIR=/path/to/EventAnalysisInstallDir/cmake/ ../
cd ${PWD}

