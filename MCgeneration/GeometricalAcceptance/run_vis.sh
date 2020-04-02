echo "Converting ... "
GGSWolowitz -g ../../build/plugin/libHerdMCParametricGeo.dylib -t vgm -o HerdMCParametricGeo.root

echo "Visualising ... "
GGSLeonard -g HerdMCParametricGeo.root
