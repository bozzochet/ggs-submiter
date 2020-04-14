DIR=${STORAGE}/HERD/analysis/herd-vv-svn/MCgeneration/Particles/submit/htc/output
NAME="electrons_sphere_10GeV_1000GeV_E-1"
LISTNAME=${NAME}.list
LISTFILE=lists/${LISTNAME}

if [ -f $LISTFILE ]; then
    mv -v $LISTFILE $LISTFILE.bak
fi

ls ${DIR}/${NAME}/*.root >> $LISTFILE
N=`cat ${LISTFILE} | wc -l`
echo "$N files in $LISTFILE"
