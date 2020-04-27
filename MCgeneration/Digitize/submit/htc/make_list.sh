DIR=${STORAGE}/HERD/analysis/herd-vv-svn/MCgeneration/Particles/submit/htc/output
NAME="protons_10GeV_10000GeV_E-1"
LISTNAME=${NAME}.list
LISTFILE=lists/${LISTNAME}

if [ -f $LISTFILE ]; then
    mv -v $LISTFILE $LISTFILE.bak
fi

ls ${DIR}/${NAME}/*.root >> $LISTFILE
N=`cat ${LISTFILE} | wc -l`
echo "$N files in $LISTFILE"
