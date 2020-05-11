DIR=${STORAGE}/HERD/analysis/herd-vv-svn/MCgeneration/Particles/submit/htc/output
#NAME="protons_10GeV_10000GeV_E-1"
NAME="electrons_1000GeV_10000GeV_E-1"

LISTNAME=${NAME}.list
LISTFILE=lists/${LISTNAME}

if [ -f $LISTFILE ]; then
    mv -v $LISTFILE $LISTFILE.bak
fi

for f in `ls ${DIR}/${NAME}/*.root`
do
    BASENAME=`basename $f .root`
    OUTFILE="output/$NAME/$BASENAME.dig.root"
    echo $OUTFILE
    if [ -f $OUTFILE ]; then
	echo "BASENAME already digitized"
    else
	echo "$f" >> $LISTFILE
    fi
done

N=`cat ${LISTFILE} | wc -l`
echo "$N files in $LISTFILE to be produced"
