PWD=`pwd`
cd ${STORAGE}/herd-vv-svn/build
make -j4
make install
cd ${PWD}

